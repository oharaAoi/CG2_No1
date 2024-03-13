#include "MySystem.h"

MySystem::~MySystem(){

}

bool MySystem::ProcessMessage(){
    return  winApp_->ProcessMessage();
}

void MySystem::Initialize(uint32_t backBufferWidth, int32_t backBufferHeight){
	kClientWidth_ = backBufferWidth;
	kClientHeight_ = backBufferHeight;

	// COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

    // ↓インスタンスの生成
    winApp_ = WinApp::GetInstance();
    dxCommon_ = DirectXCommon::GetInstacne();
	imGuiManager_ = ImGuiManager::GetInstacne();
	textureManager_ = TextureManager::GetInstacne();

    // ↓各初期化
    winApp_->CreateGameWindow();
    dxCommon_->Initialize(winApp_, backBufferWidth, backBufferHeight);
	imGuiManager_->Init(winApp_, dxCommon_);
	textureManager_->Initialize(dxCommon_);
}

void MySystem::Finalize(){
	CoUninitialize();
	imGuiManager_->Finalize();
}

void MySystem::BeginFrame(){
	imGuiManager_->Begin();

    dxCommon_->BeginFrame();

}

void MySystem::EndFrame(){
	imGuiManager_->End();
	imGuiManager_->Draw();

    dxCommon_->EndFrame();

	triangle_.clear();
	/*sprite_.clear();*/
}

void MySystem::DrawTriangle(const Matrix4x4& wvpMatrix, const Vertices& vertex){

	int index = static_cast<int>(triangle_.size());

	triangle_.emplace_back();

	// ---------------------------------------------------------------
	// ↓Vetrtexの設定
	// ---------------------------------------------------------------
	// VertexBufferViewを作成する 
	triangle_[index].vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 3);
	// リソースの先頭のアドレスから使う
	triangle_[index].vertexBufferView_.BufferLocation = triangle_[index].vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	triangle_[index].vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	// 1頂点当たりのサイズ
	triangle_[index].vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// Resourceにデータを書き込む 
	VertexData* vertexData = nullptr;
	// アドレスを取得
	triangle_[index].vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0].pos = vertex.vertex1;
	vertexData[0].texcord = { 0.0f, 1.0f };
	// 上
	vertexData[1].pos = vertex.vertex2;
	vertexData[1].texcord = { 0.5f, 0.0f };
	// 右下
	vertexData[2].pos = vertex.vertex3;
	vertexData[2].texcord = { 1.0f, 1.0f };
	// ---------------------------------------------------------------

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	// resourceの作成
	triangle_[index].materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	// データを書く
	Vector4* materialData = nullptr;
	// アドレスを取得
	triangle_[index].materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色を決める
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// ---------------------------------------------------------------

	// ---------------------------------------------------------------
	// ↓Transformationの設定
	// ---------------------------------------------------------------
	// resourceの作成
	triangle_[index].wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	Matrix4x4* wvpData = nullptr;
	triangle_[index].wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	
	*wvpData = wvpMatrix;

	// コマンドを積む ------------------------------------------------------------------------------
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &triangle_[index].vertexBufferView_);
	// materialCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, triangle_[index].materialResource_->GetGPUVirtualAddress());
	// wvpCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, triangle_[index].wvpResource_->GetGPUVirtualAddress());
	// どのtextureを読むのかをコマンドに積む
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU());

	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void MySystem::DrawSprite(const Matrix4x4& wvpMatrix, const RectVetex& vertex) {
	// VertexBufferViewを作成する 
	sprite_.vertexResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = sprite_.vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	sprite_.vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	// 頂点データを設定する
	// 一枚目
	vertexDataSprite[0].pos = vertex.leftBottom;	// 左下
	vertexDataSprite[0].texcord = { 0.0f, 1.0f };
	vertexDataSprite[1].pos = vertex.leftTop;		// 左上
	vertexDataSprite[1].texcord = { 0.0f, 0.0f };
	vertexDataSprite[2].pos = vertex.rightBottom;	// 右下
	vertexDataSprite[2].texcord = { 1.0f, 1.0f };

	// 二枚目
	vertexDataSprite[3].pos = vertex.leftTop;	// 左上
	vertexDataSprite[3].texcord = { 0.0f, 0.0f };
	vertexDataSprite[4].pos = vertex.rightTop;	// 右上
	vertexDataSprite[4].texcord = { 1.0f, 0.0f };
	vertexDataSprite[5].pos = vertex.rightBottom;	// 右下
	vertexDataSprite[5].texcord = { 1.0f, 1.0f };

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	// resourceの作成
	sprite_.materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	// データを書く
	Vector4* materialData = nullptr;
	// アドレスを取得
	sprite_.materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色を決める
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

	// ---------------------------------------------------------------
	// ↓Transformationの設定
	// ---------------------------------------------------------------
	sprite_.transfomationMatrixResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	Matrix4x4* transformationMatrixDataSprite = nullptr;
	sprite_.transfomationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	*transformationMatrixDataSprite = wvpMatrix;

	// ---------------------------------------------------------------
	// ↓ コマンドを積む
	// ---------------------------------------------------------------
	// 三角形の描画がある前提の設定なため不完全
	// spriteの描画
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// materialCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite_.materialResource_->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite_.transfomationMatrixResourceSprite->GetGPUVirtualAddress());
	// どのtextureを読むのかをコマンドに積む
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU());
	// 描画
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
}
