#include "Triangle.h"

Triangle::Triangle() {}

Triangle::~Triangle() {}

void Triangle::Init(ID3D12Device* device, const Vertices& vertex) {
	mesh_ = std::make_unique<Mesh>();
	material_ = std::make_unique<Material>();

	mesh_->Init(device, sizeof(VertexData) * 3, sizeof(uint32_t) * 3, vertex);
	material_->Init(device);

	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void Triangle::Update(const kTransform& transform, const Matrix4x4 viewprojectionMatrix, const Vector4& color) {
	transform_ = transform;
	worldMatrix_ = MakeAffineMatrix(transform_);
	wvpMatrix_ = Multiply(worldMatrix_, viewprojectionMatrix);

	color_ = color;
}

void Triangle::Draw(ID3D12GraphicsCommandList* commandList) {
	mesh_->Draw(commandList);
	material_->Draw(commandList, wvpMatrix_, color_);
}
