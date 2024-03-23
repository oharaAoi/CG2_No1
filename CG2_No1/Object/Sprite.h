#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

#include "Mesh.h"
#include "Material.h"
#include "VertexData.h"

class Sprite {
public:

	struct MaterialSprite {
		Vector4 color;
		uint32_t enableLigthing;
		float padding[3];
		Matrix4x4 uvTransform;
	};

public:

	Sprite();
	~Sprite();

	void Init(ID3D12Device* device, const RectVetex& vertex);

	void Update(const kTransform& uvTransform,
		const kTransform& transform,
		const Matrix4x4 viewprojectionMatrix,
		const Vector4& color);

	void Draw(ID3D12GraphicsCommandList* commandList);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transfomationBuffer_;

	D3D12_VERTEX_BUFFER_VIEW vBView_{};
	D3D12_INDEX_BUFFER_VIEW iBView_{};

private:

	MaterialSprite* materialData_;
	TramsformationMatrix* transformationData_;

};
