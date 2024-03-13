#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

struct kSprite {
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResourceSprite;
};

