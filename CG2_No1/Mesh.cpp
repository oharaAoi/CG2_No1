#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh() {}

void Mesh::Init(ID3D12Device* device, const uint32_t& vBSize, const uint32_t& iBSize, const Vertices& vertex) {
	// ---------------------------------------------------------------
	// ↓Vetrtexの設定
	// ---------------------------------------------------------------
	// VertexBufferViewを作成する 
	vertexBuffer_ = CreateBufferResource(device, vBSize);
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = vBSize;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む 
	VertexData* vertexData = nullptr;
	// アドレスを取得
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
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

	// ---------------------------------------------------------------
	// ↓Indexの設定
	// ---------------------------------------------------------------
	indexBuffer_ = CreateBufferResource(device, iBSize);
	indexBufferView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = iBSize;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
}

void Mesh::Draw(ID3D12GraphicsCommandList* commandList) {
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList->IASetIndexBuffer(&indexBufferView_);
}
