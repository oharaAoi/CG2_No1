#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

#include "Function/DirectXUtils.h"
#include "VertexData.h"

class Mesh final {

public:

	Mesh();
	~Mesh();

public:

	void Init(ID3D12Device* device, const uint32_t& vBSize, const uint32_t& iBSize, const Vertices& vertex);

	void Draw(ID3D12GraphicsCommandList* commandList);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
};

