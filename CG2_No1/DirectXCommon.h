#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <wrl/client.h>

#include "WinApp.h"
#include "Function/Convert.h"
#include "Function/DirectXUtils.h"

class DirectXCommon{
public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstacne();


public:

	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight);

public: // 色々な設定をしているメンバ関数

	/// <summary>
	/// DXGTデバイス初期化
	/// </summary>
	void InitializeDXGDevice();

	void BeginFrame();

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

private:

	WinApp* winApp_;
	int32_t kClientWidth_;
	int32_t kClientHeight_;

	/// <summary>///
	/// objectが不要になった時に自動でRelese
	/// </summary>///
	/// <typeparam name="T"></typeparam>
	template<typename T>
	using Comptr = Microsoft::WRL::ComPtr <T>;

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