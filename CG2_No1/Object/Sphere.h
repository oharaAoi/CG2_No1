#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

#include "Mesh.h"
#include "Material.h"
#include "VertexData.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>


class Sphere {
public:

	struct Light {
		Vector4 color_;
		Vector3 direction_;
		float intensity_;
	};
	
public:

	Sphere();
	~Sphere();

public:

	void Init(ID3D12Device* device, const uint32_t& division);

	void Update(const kTransform& transform, const Matrix4x4 viewprojectionMatrix, const Vector4& color);

	void Draw(ID3D12GraphicsCommandList* commandList);

public:

	uint32_t GetVertexCount() const { return vertexCount_; }

private:

	// 頂点バッファやインデックスバッファを持つ
	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_;

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightBuffer_;

	kTransform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 wvpMatrix_;

	Vector4 color_;

	// 頂点数
	uint32_t vertexCount_;
};

