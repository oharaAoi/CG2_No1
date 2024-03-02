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

	InitializeDXGDevice();

	CreateCommand();

	CreateSwapChain();

	CreateRTV();

	CrateFence();
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

	// 描画先のRTVを設定
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	// 色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	// ---------------------------------------------------------------
	// ↓RTVの画面から画面表示できるようにする
	// ---------------------------------------------------------------
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// 張る
	commandList_->ResourceBarrier(1, &barrier_);
	// ---------------------------------------------------------------
}

void DirectXCommon::EndFrame(){
	HRESULT hr = S_FALSE;

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
