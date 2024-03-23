#include "Triangle.h"

Triangle::Triangle() {}

Triangle::~Triangle() {}

void Triangle::Init(ID3D12Device* device, const Vertices& vertex) {
	mesh_ = std::make_unique<Mesh>();
	material_ = std::make_unique<Material>();

	mesh_->Init(device, sizeof(VertexData) * 3, sizeof(uint32_t) * 3);
	material_->Init(device, false);

	// vertexの設定
	VertexData* vertexData = mesh_->GetVertexData();
	// 左下
	vertexData[0].pos = vertex.vertex1;
	vertexData[0].texcord = { 0.0f, 1.0f };
	vertexData[0].normal = { 0.0f, 0.0f, -1.0f };
	// 上
	vertexData[1].pos = vertex.vertex2;
	vertexData[1].texcord = { 0.5f, 0.0f };
	vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
	// 右下
	vertexData[2].pos = vertex.vertex3;
	vertexData[2].texcord = { 1.0f, 1.0f };
	vertexData[2].normal = { 0.0f, 0.0f, -1.0f };

	// indexの設定
	uint32_t* indexData = nullptr;
	indexData = mesh_->GetIndexData();
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

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
