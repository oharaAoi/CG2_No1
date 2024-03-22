#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

#include "VertexData.h"

#include "Mesh.h"
#include "Material.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif // _DEBUG

class Triangle {
public: // メンバ関数

	Triangle();
	~Triangle();

public: // accesser

	void Init(ID3D12Device* device, const Vertices& vertex);

	void Update(const kTransform& transform, const Matrix4x4 viewprojectionMatrix, const Vector4& color);

	void Draw(ID3D12GraphicsCommandList* commandList);

private:

	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_;

	kTransform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 wvpMatrix_;

	Vector4 color_;

};