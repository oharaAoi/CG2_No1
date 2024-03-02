#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

struct kTriangle {
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
};