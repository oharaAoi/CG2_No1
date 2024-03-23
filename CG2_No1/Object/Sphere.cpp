#include "Sphere.h"

Sphere::Sphere() {}

Sphere::~Sphere() {}

void Sphere::Init(ID3D12Device* device, const uint32_t& division) {
	const float kLonEvery = float(M_PI) * 2.0f / float(division);// fai
	const float kLatEvery = float(M_PI) / float(division); // theta

	//vertexCount_ = (int(division / kLatEvery)) * (int(division / kLonEvery)) * 6;
	vertexCount_ = division * division * 6;

	mesh_ = std::make_unique<Mesh>();
	material_ = std::make_unique<Material>();

	mesh_->Init(device, sizeof(VertexData) * vertexCount_, sizeof(uint32_t) * vertexCount_);
	material_->Init(device, true);

	// vertexの設定
	VertexData* vertexData = mesh_->GetVertexData();
	uint32_t start = 0;
	// indexの設定
	uint32_t* indexData = mesh_->GetIndexData();
	uint32_t startIndex = 0;

	// 緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < division; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // theta
		// 経度方向に分割
		for (uint32_t lonIndex = 0; lonIndex < division; ++lonIndex) {
			start = (latIndex * division + lonIndex) * 4;
			float lon = lonIndex * kLonEvery; // fai
			// 頂点データを入力
			// a 左下
			VertexData keepVertexData{};
			keepVertexData.pos.x = std::cos(lat) * std::cos(lon);
			keepVertexData.pos.y = std::sin(lat);
			keepVertexData.pos.z = std::cos(lat) * std::sin(lon);
			keepVertexData.pos.w = 1.0f;
			keepVertexData.texcord.x = float(lonIndex) / float(division);
			keepVertexData.texcord.y = 1.0f - float(latIndex) / float(division);
			keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
			vertexData[start] = keepVertexData;

			// b 左上
			keepVertexData.pos.x = std::cos(lat + kLatEvery) * std::cos(lon);
			keepVertexData.pos.y = std::sin(lat + kLatEvery);
			keepVertexData.pos.z = std::cos(lat + kLatEvery) * std::sin(lon);
			keepVertexData.pos.w = 1.0f;
			keepVertexData.texcord.x = float(lonIndex) / float(division);
			keepVertexData.texcord.y = 1.0f - float(latIndex + 1) / float(division);
			keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
			vertexData[start + 1] = keepVertexData;

			// c 右下
			keepVertexData.pos.x = std::cos(lat) * std::cos(lon + kLonEvery);
			keepVertexData.pos.y = std::sin(lat);
			keepVertexData.pos.z = std::cos(lat) * std::sin(lon + kLonEvery);
			keepVertexData.pos.w = 1.0f;
			keepVertexData.texcord.x = float(lonIndex + 1) / float(division);
			keepVertexData.texcord.y = 1.0f - float(latIndex) / float(division);
			keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
			vertexData[start + 2] = keepVertexData;

			// d 右上
			keepVertexData.pos.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
			keepVertexData.pos.y = std::sin(lat + kLatEvery);
			keepVertexData.pos.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
			keepVertexData.pos.w = 1.0f;
			keepVertexData.texcord.x = float(lonIndex + 1) / float(division);
			keepVertexData.texcord.y = 1.0f - float(latIndex + 1) / float(division);
			keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
			vertexData[start + 3] = keepVertexData;

			//
			startIndex = (latIndex * division + lonIndex) * 6;
			indexData[startIndex] = start;
			indexData[startIndex + 1] = start + 1;
			indexData[startIndex + 2] = start + 2;

			indexData[startIndex + 3] = start + 1;
			indexData[startIndex + 4] = start + 3;
			indexData[startIndex + 5] = start + 2;
		}
	}

	Light* light = nullptr;
	directionalLightBuffer_ = CreateBufferResource(device, sizeof(Light));
	directionalLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&light));
	light->color_ = { 1.0f,1.0f, 1.0f, 1.0f };
	light->direction_ = { 0.0f, -1.0f, 0.0f };
	light->intensity_ = 1.0f;
}

void Sphere::Update(const kTransform& transform, const Matrix4x4 viewprojectionMatrix, const Vector4& color) {
	transform_ = transform;
	worldMatrix_ = MakeAffineMatrix(transform_);
	wvpMatrix_ = Multiply(worldMatrix_, viewprojectionMatrix);

	color_ = color;
}

void Sphere::Draw(ID3D12GraphicsCommandList* commandList) {
	mesh_->Draw(commandList);
	material_->Draw(commandList, wvpMatrix_, color_);

	commandList->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGPUVirtualAddress());
}
