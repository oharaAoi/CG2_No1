#include "MySystem.h"

MySystem::~MySystem(){

}

bool MySystem::ProcessMessage(){
    return  winApp_->ProcessMessage();
}

void MySystem::Initialize(uint32_t backBufferWidth, int32_t backBufferHeight){
	kClientWidth_ = backBufferWidth;
	kClientHeight_ = backBufferHeight;

    // ↓インスタンスの生成
    winApp_ = WinApp::GetInstance();
    dxCommon_ = DirectXCommon::GetInstacne();
	imGuiManager_ = ImGuiManager::GetInstacne();

    // ↓各初期化
    winApp_->CreateGameWindow();
    dxCommon_->Initialize(winApp_, backBufferWidth, backBufferHeight);
	imGuiManager_->Init(winApp_, dxCommon_);

    // 三角形

}

void MySystem::Finalize(){
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
}

void MySystem::DrawTriangle(const Matrix4x4 worldMatrix){
	// ---------------------------------------------------------------
	// ↓Vetrtexの設定
	// ---------------------------------------------------------------
	// VertexBufferViewを作成する 
	triangle_.vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4) * 3);
	// リソースの先頭のアドレスから使う
	triangle_.vertexBufferView_.BufferLocation = triangle_.vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	triangle_.vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;
	// 1頂点当たりのサイズ
	triangle_.vertexBufferView_.StrideInBytes = sizeof(Vector4);

	// Resourceにデータを書き込む 
	Vector4* vertexData = nullptr;
	// アドレスを取得
	triangle_.vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	// 左下
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	// 上
	vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f };
	// 右下
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };
	// ---------------------------------------------------------------

	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	// resourceの作成
	triangle_.materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector4));
	// データを書く
	Vector4* materialData = nullptr;
	// アドレスを取得
	triangle_.materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は赤
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// ---------------------------------------------------------------

	// ---------------------------------------------------------------
	// ↓Transformationの設定
	// ---------------------------------------------------------------
	// resourceの作成
	triangle_.wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	Matrix4x4* wvpData = nullptr;
	triangle_.wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 今回は赤
	*wvpData = worldMatrix;

	// ---------------------------------------------------------------
	// ↓ViewportとScissor
	// ---------------------------------------------------------------
	// ビューポート
	// クライアント領域のサイズと一緒にして画面全体を表示
	triangle_.viewport_.Width = static_cast<float>(kClientWidth_);
	triangle_.viewport_.Height = static_cast<float>(kClientHeight_);
	triangle_.viewport_.TopLeftX = 0;
	triangle_.viewport_.TopLeftY = 0;
	triangle_.viewport_.MinDepth = 0.0f;
	triangle_.viewport_.MaxDepth = 1.0f;

	// シザー矩形
	// 基本的にビューポートと同じ矩形が構成されるようにする
	triangle_.scissorRect_.left = 0;
	triangle_.scissorRect_.right = static_cast<LONG>(kClientWidth_);
	triangle_.scissorRect_.top = 0;
	triangle_.scissorRect_.bottom = static_cast<LONG>(kClientHeight_);
	// ---------------------------------------------------------------

	// コマンドを積む ------------------------------------------------------------------------------
	dxCommon_->GetCommandList()->RSSetViewports(1, &triangle_.viewport_);
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &triangle_.scissorRect_);
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(dxCommon_->GetRootSignature().Get());
	dxCommon_->GetCommandList()->SetPipelineState(dxCommon_->GetPSO().Get());
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &triangle_.vertexBufferView_);
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// materialCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, triangle_.materialResource_->GetGPUVirtualAddress());
	// wvpCBufferの設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, triangle_.wvpResource_->GetGPUVirtualAddress());

	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}
