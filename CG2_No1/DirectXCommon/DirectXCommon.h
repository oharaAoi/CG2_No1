#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include "dxgidebug.h"
#include <cassert>

#include <wrl.h>

#include "WinApp.h"

#include "Function/Convert.h"
#include "Function/DirectXUtils.h"
#include "Function/Debug.h"

#include "VertexData.h"

class DirectXCommon{
public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstacne();

public:

	DirectXCommon() = default;
	~DirectXCommon();
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight);

	void Finalize();

public: // accsesser

	UINT GetBufferCount() const { return bufferCount_; }

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return device_; }

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList_; }

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_; }

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPSO() { return graphicsPipelineState_; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRVDiscriptorHeap() { return srvDiscriptorHeap_; }

public: // 色々な設定をしているメンバ関数

	/// <summary>
	/// DXGTデバイス初期化
	/// </summary>
	void InitializeDXGDevice();

	/// <summary>
	/// DierctXCompilerの初期化
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// フレームの開始
	/// </summary>
	void BeginFrame();

	/// <summary>
	/// フレームの終了
	/// </summary>
	void EndFrame();

public: // 生成を行うメンバ関数

	/// <summary>
	/// コマンドの生成
	/// </summary>
	void CreateCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// レンダーターゲットビュー
	/// </summary>
	void CreateRTV();

	/// <summary>
	/// Fenceの生成
	/// </summary>
	void CrateFence();
	

//=================================================================================================================
//	↓PSOの内容
//=================================================================================================================
	/// <summary>
	/// RootSignatureの生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// InputLayoutの生成
	/// </summary>
	void CreateInputLayout();

	/// <summary>
	/// Shaderをcompileする
	/// </summary>
	void ShaderCompile();

	/// <summary>
	/// BlendStateの設定
	/// </summary>
	D3D12_BLEND_DESC SetBlendState();

	/// <summary>
	/// RasterizerStateの設定
	/// </summary>
	D3D12_RASTERIZER_DESC SetRasterizerState();

	/// <summary>
	/// PSOの生成
	/// </summary>
	void CreatePSO();

//=================================================================================================================

private:

	D3DResourceLeakChecker leakCheck;

	WinApp* winApp_;
	int32_t kClientWidth_;
	int32_t kClientHeight_;

	UINT bufferCount_; 

	/// <summary>///
	/// objectが不要になった時に自動でRelese
	/// </summary>///
	/// <typeparam name="T"></typeparam>
	template<typename T>
	using Comptr = Microsoft::WRL::ComPtr <T>;

	// デバック
	Comptr<ID3D12Debug1> debugController_ = nullptr;

	// DirectXの初期化に使う物
	Comptr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Comptr<IDXGIAdapter4> useAdapter_ = nullptr;
	Comptr<ID3D12Device> device_ = nullptr;

	// コマンド系
	Comptr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Comptr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Comptr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	// swapChaim
	Comptr<IDXGISwapChain4> swapChain_ = nullptr;
	Comptr<ID3D12Resource> swapChainResource_[2];

	// DescriptorHeap
	Comptr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	
	// rtv
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// barrier
	D3D12_RESOURCE_BARRIER barrier_;

	// Fence & Event
	Comptr<ID3D12Fence> fence_ = nullptr;
	uint64_t  fenceValue_;
	HANDLE fenceEvent_;

	// DXCで使う
	Comptr<IDxcUtils> dxcUtils_ = nullptr;
	Comptr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Comptr<IDxcIncludeHandler> includeHandler_ = nullptr;

//=================================================================================================================
//	↓PSOの内容
//=================================================================================================================
	// rootSignature
	Comptr<ID3DBlob> signatureBlob_ = nullptr;
	Comptr<ID3DBlob> errorBlob_ = nullptr;
	Comptr<ID3D12RootSignature> rootSignature_ = nullptr;

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;

	// Shader
	Comptr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Comptr<IDxcBlob> pixelShaderBlob_ = nullptr;

	// PSO
	Comptr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_;
//=================================================================================================================

	// SRV用のDiscriptorHeap
	Comptr<ID3D12DescriptorHeap> srvDiscriptorHeap_ = nullptr;

public:
	/// <summary>
	///  Comptrのスマートポインタで定義した物のポインタを取得するための変換関数
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="comptr"></param>
	/// <returns></returns>
	template<typename T>
	T* ConvertComPtr(const Comptr<T>& comptr) {
		return comptr.Get();
	}
};