#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>

// dxc
#include <dxcapi.h>
#include <vector>

#include "Function/Convert.h"

#include <wrl.h>

/// <summary>
/// CompileShader
/// </summary>
/// <param name=""></param>
IDxcBlob* CompilerShader(
	const std::wstring& filePath,
	const wchar_t* profile,
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler
);

/// <summary>
/// DiscriptorHeapの作成
/// </summary>
/// <param name="device"></param>
/// <param name="heapType"></param>
/// <param name="numDescriptor"></param>
/// <param name="shaderVisible"></param>
/// <returns></returns>
/// template<typename T>
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> device,
	const D3D12_DESCRIPTOR_HEAP_TYPE& heapType,
	const UINT& numDescriptor,
	const bool& shaderVisible);

/// <summary>
/// 
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const size_t& sizeInBytes);

/// <summary>
/// 深度情報を格納するリソースの生成
/// </summary>
/// <param name="device"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const int32_t& width, const int32_t& height);

/// <summary>
/// ログを出す
/// </summary>
/// <param name="message"></param>
void Log(const std::string& message);