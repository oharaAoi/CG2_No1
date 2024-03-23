#include "Sprite.h"

Sprite::Sprite() {}

Sprite::~Sprite() {}

void Sprite::Init(ID3D12Device* device, const RectVetex& vertex) {
	// ---------------------------------------------------------------
	// ↓Vetrtexの設定
	// ---------------------------------------------------------------
	//  VertexBufferResourceを作成する
	vertexBuffer_ = CreateBufferResource(device, sizeof(VertexData) * 4);
	// 頂点バッファビューを作成する
	vBView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分
	vBView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vBView_.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	//頂点データを設定する 一枚目
	vertexDataSprite[0].pos = vertex.leftBottom;	// 左下
	vertexDataSprite[0].texcord = { 0.0f, 1.0f };
	vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[1].pos = vertex.leftTop;		// 左上
	vertexDataSprite[1].texcord = { 0.0f, 0.0f };
	vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[2].pos = vertex.rightBottom;	// 右下
	vertexDataSprite[2].texcord = { 1.0f, 1.0f };
	vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };

	vertexDataSprite[3].pos = vertex.rightTop;		// 右上
	vertexDataSprite[3].texcord = { 1.0f, 0.0f };
	vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };

	// ---------------------------------------------------------------
	// ↓Indexの設定
	// ---------------------------------------------------------------
	indexBuffer_ = CreateBufferResource(device, sizeof(uint32_t) * 6);
	iBView_.BufferLocation = indexBuffer_->GetGPUVirtualAddress();
	iBView_.SizeInBytes = sizeof(uint32_t) * 6;
	iBView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexDataSprite = nullptr;
	indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;

	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	// Resourceの作成
	materialBuffer_ = CreateBufferResource(device, sizeof(MaterialSprite));
	// マッピング
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	// 色を決める
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// ライトの有無
	materialData_->enableLigthing = false;
	// UV
	kTransform uvTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData_->uvTransform = uvTransformMatrix;

	// ---------------------------------------------------------------
	// ↓Transformationの設定
	// ---------------------------------------------------------------
	transfomationBuffer_ = CreateBufferResource(device, sizeof(TramsformationMatrix));
	transformationData_ = nullptr;
	transfomationBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));
	transformationData_->WVP = MakeIdentity4x4();
	transformationData_->World = MakeIdentity4x4();
	
}

void Sprite::Update(const kTransform& uvTransform, 
	const kTransform& transform, 
	const Matrix4x4 viewprojectionMatrix, 
	const Vector4& color) {

	// Materialの更新
	materialData_->color = color;
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData_->uvTransform = uvTransformMatrix;

	// Transformの設定
	transformationData_->WVP = Multiply(MakeAffineMatrix(transform), viewprojectionMatrix);
	transformationData_->World = MakeAffineMatrix(transform);
}

void Sprite::Draw(ID3D12GraphicsCommandList* commandList) {
	// vertexの描画
	commandList->IASetVertexBuffers(0, 1, &vBView_);
	// indexの描画
	commandList->IASetIndexBuffer(&iBView_);
	// CVB(Material)
	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
	// CVB(Transform)
	commandList->SetGraphicsRootConstantBufferView(1, transfomationBuffer_->GetGPUVirtualAddress());
}

