#include "DirectXCommon.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

DirectXCommon* DirectXCommon::GetInstacne() {
	static DirectXCommon instance;
	return &instance;
}

DirectXCommon::~DirectXCommon(){
	Finalize();
}

//=================================================================================================================
//	↓DirectXを使うための初期化
//=================================================================================================================
void DirectXCommon::Initialize(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight){
	assert(win);
	winApp_ = win;
	kClientWidth_ = backBufferWidth;
	kClientHeight_ = backBufferHeight;

	bufferCount_ = 2;

	// deviceの初期化
	InitializeDXGDevice();

	// commandの初期化
	CreateCommand();

	// swapChainの初期化
	CreateSwapChain();

	// RTVの初期化
	CreateRTV();

	// srvHeapの作成
	srvDiscriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// FenceとEventの初期化
	CrateFence();

	// compilerの初期化
	InitializeDXC();

	CreateDSV();

	// PSOの生成
	CreatePSO();

	// viewportの設定
	SetViewport();
}

void DirectXCommon::Finalize(){
	CloseHandle(fenceEvent_);
}

//=================================================================================================================
//	↓DirectXを使うための初期化
//=================================================================================================================
void DirectXCommon::InitializeDXGDevice(){

#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {
		// デバックレイヤーを有効化する
		debugController_->EnableDebugLayer();
		// さらにGPU側でもチェック
		debugController_->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	dxgiFactory_ = nullptr;
	// HRESULはwidows系のエラーコードであり
	// 関数が成功したかどうかをSUCCEEDEマクロ判定で判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));

	// --------------------------------------------------------------------------------
	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	useAdapter_ = nullptr;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {

		// アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr)); // 取得出来なかった時用
		// ソフトウェアのアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報の情報をログに出力.wstrinfgの方
			Log(ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}

		useAdapter_ = nullptr; // ソフトウェアアダプタの場合は見なかったことにする
	}

	// 適切なアダプタが見つからなかったら軌道できない
	assert(useAdapter_ != nullptr);

	// ------------------------------------------------------------------------------
	// 機能レベルとログ出力四の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelString[] = { "12.2", "12.1", "12.0" };

	// 高い順に生成できるか試していく
	for (size_t levels = 0; levels < _countof(featureLevels); ++levels) {
		// 採用したアダプターでデバイスを生成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[levels], IID_PPV_ARGS(&device_));

		// 指定した機能レベルでデバイスが生成出来たか確認
		if (SUCCEEDED(hr)) {
			// 生成できたためログ出力を行ってループを抜ける
			Log((std::format("FeatureLevel : {}\n", featureLevelString[levels])));
			break;
		}
	}

	// デバイスの生成がうまく行かなかった時
	assert(device_ != nullptr);
	Log("complete create D3D12Device\n");

#ifdef _DEBUG

	Comptr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// ヤバいエラージに止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID ------------------------------
		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバックレイヤーとDX12デバックレイヤーの相互作用にバグによるメッセージ
			// https://stackoverflow.com.questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定してメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}

#endif // DEBUG
}

//=================================================================================================================
//	↓HLSLからDXILに変換するCompilerの初期化
//=================================================================================================================
void DirectXCommon::InitializeDXC(){
	HRESULT hr = S_FALSE;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));

	// includeに対応するための設定
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

//=================================================================================================================
//	↓フレーム系
//=================================================================================================================
void DirectXCommon::BeginFrame(){
	HRESULT hr = S_FALSE;
	// インデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// ---------------------------------------------------------------
	// ↓barrierを張る 
	// ---------------------------------------------------------------
	// Transitionzで張る
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// barrierを張る対象のリソース
	barrier_.Transition.pResource = swapChainResource_[backBufferIndex].Get();
	// 遷移前のリソース
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// 張る
	commandList_->ResourceBarrier(1, &barrier_);
	// ---------------------------------------------------------------

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	// 描画先のRTVを設定
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);
	// 色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDiscriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	// 
	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
	commandList_->SetGraphicsRootSignature(rootSignature_.Get());
	commandList_->SetPipelineState(graphicsPipelineState_.Get());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DirectXCommon::EndFrame(){
	HRESULT hr = S_FALSE;

	// ---------------------------------------------------------------
	// ↓RTVの画面から画面表示できるようにする
	// ---------------------------------------------------------------
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// 張る
	commandList_->ResourceBarrier(1, &barrier_);
	// ---------------------------------------------------------------

	// 確定させる
	hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* commadLists[] = { commandList_.Get()};
	commandQueue_->ExecuteCommandLists(1, commadLists);
	// GPUとOSに画面の交換をするようにする
	swapChain_->Present(1, 0);

	// fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いた時に,fenceの値を指定した値に第謬するようにsignelを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);

	// Fenceの値が指定したSignal値にたどりついているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定下Signalにたどりついていないので、たどりつくまで松ようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);

		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のコマンドリストの準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

//=================================================================================================================
//	↓生成や設定を行う関数
//=================================================================================================================

/// <summary>
/// command
/// </summary>
void DirectXCommon::CreateCommand(){
	HRESULT hr = S_FALSE;
	// GPUに命令を投げてくれる人　--------------------------
	// コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// CommandAllocatorの生成 --------------------------------
	// コマンドアロケータを生成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	// コマンドリストを生成する ----------------------------
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));
}

/// <summary>
/// swapChain
/// </summary>
void DirectXCommon::CreateSwapChain(){
	HRESULT hr = S_FALSE;
	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width = kClientWidth_;							// 画面の幅
	desc.Height = kClientHeight_;						// 画面の高さ
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;			// 色の形式
	desc.SampleDesc.Count = 1;							// マルチサンプルしない
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用する
	desc.BufferCount = 2;								// ダブルバッファ
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに移したら中身を破棄
	
	// コマンドキュー、ウィンドウハンドルを設定して生成する
	/*Comptr<IDXGISwapChain1> swapChain1;*/
	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &desc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//// SwapChain4を得る
	//swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	//assert(SUCCEEDED(hr));

	// resourceも一緒に作る
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResource_[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResource_[1]));
	assert(SUCCEEDED(hr));
}

/// <summary>
/// 
/// </summary>
void DirectXCommon::CreateRTV(){
	// ヒープの生成
	rtvDescriptorHeap = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// RTVの生成
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	// 先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	// 一つ目のDiscriptaの生成
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResource_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	// 二つ目の生成
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device_->CreateRenderTargetView(swapChainResource_[1].Get(), &rtvDesc_, rtvHandles_[1]);
}

/// <summary>
/// FenceとEventの生成
/// </summary>
void DirectXCommon::CrateFence(){
	HRESULT hr = S_FALSE;
	fenceValue_ = 0;
	hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	// Fenceのsignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, false, false, NULL);
	assert(fenceEvent_ != nullptr);
}

/// <summary>
/// 
/// </summary>
void DirectXCommon::CreateDSV(){
	depthStencilResource_ = CreateDepthStencilTextureResource(device_, kClientWidth_, kClientHeight_);

	// heapを作る
	dsvDescriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// heap上にDSCを構築
	D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device_->CreateDepthStencilView(depthStencilResource_.Get(), &desc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

/// <summary>
/// 
/// </summary>
void DirectXCommon::SetViewport(){
	// ビューポート
	// クライアント領域のサイズと一緒にして画面全体を表示
	viewport_.Width = static_cast<float>(kClientWidth_);
	viewport_.Height = static_cast<float>(kClientHeight_);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	// シザー矩形
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(kClientWidth_);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(kClientHeight_);
}

//=================================================================================================================
//	↓PSOの内容
//=================================================================================================================
/// <summary>
/// RootSignature
/// </summary>
void DirectXCommon::CreateRootSignature(){
	HRESULT hr = S_FALSE;
	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// 複数のtextureを読むための
	D3D12_DESCRIPTOR_RANGE descriptorRange[1]{};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// rootParameterの作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;					// レジスタ番号とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; 
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// DescriptorTable
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		// DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;					// PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;				// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);	// Tableで利用する数

	desc.pParameters = rootParameters;
	desc.NumParameters = _countof(rootParameters);

	// Samplerの設定 -------------------------------------------
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	desc.pStaticSamplers = staticSamplers;
	desc.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	hr = D3D12SerializeRootSignature(&desc,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	hr = device_->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

/// <summary>
/// inputLayout
/// </summary>
void DirectXCommon::CreateInputLayout(){
	D3D12_INPUT_ELEMENT_DESC desc[1] = {};
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = desc;
	inputLayoutDesc_.NumElements = _countof(desc);
}

/// <summary>
/// ShaderCompile
/// </summary>
void DirectXCommon::ShaderCompile(){
	vertexShaderBlob_ = CompilerShader(L"HLSL/Object3d.VS.hlsl", L"vs_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompilerShader(L"HLSL/Object3d.PS.hlsl", L"ps_6_0", dxcUtils_.Get(), dxcCompiler_.Get(), includeHandler_.Get());
	assert(pixelShaderBlob_ != nullptr);
}

/// <summary>
/// BlendStateの設定
/// </summary>
D3D12_BLEND_DESC DirectXCommon::SetBlendState(){
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDesc;
}

/// <summary>
/// RasterizerStateの設定
/// </summary>
D3D12_RASTERIZER_DESC DirectXCommon::SetRasterizerState(){
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	return rasterizerDesc;
}

/// <summary>
/// DepthStencilStateの設定
/// </summary>
/// <returns></returns>
D3D12_DEPTH_STENCIL_DESC DirectXCommon::SetDepthStencilState(){
	D3D12_DEPTH_STENCIL_DESC desc{};
	// Depthの機能を有効化する
	desc.DepthEnable = true;
	// 書き込み
	desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 地下駆ければ描画
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	return desc;
}

/// <summary>
/// PSOの生成
/// </summary>
void DirectXCommon::CreatePSO(){
	HRESULT hr = S_FALSE;

	CreateRootSignature();
	/*CreateInputLayout();*/
	ShaderCompile();

	// --------------------------------------------------------------------
	D3D12_INPUT_ELEMENT_DESC desc[2] = {};
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	desc[1].SemanticName = "TEXCOORD";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = desc;
	inputLayoutDesc_.NumElements = _countof(desc);
	// --------------------------------------------------------------------

	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };
	graphicsPipelineStateDesc_.BlendState = SetBlendState();
	graphicsPipelineStateDesc_.RasterizerState = SetRasterizerState();
	graphicsPipelineStateDesc_.DepthStencilState = SetDepthStencilState();
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// 実際に生成
	hr = device_->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));
}
//=================================================================================================================
