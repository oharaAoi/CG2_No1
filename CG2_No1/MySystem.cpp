#include "MySystem.h"

MySystem::MySystem(){
	
}

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

	useMonstorBall_ = false;
}

void MySystem::Finalize(){
	CoUninitialize();
	imGuiManager_->Finalize();
}

void MySystem::BeginFrame(){
	imGuiManager_->Begin();

    dxCommon_->BeginFrame();

	ImGui::Begin("system");
	ImGui::Checkbox("useMonstorBall", &useMonstorBall_);
	ImGui::End();
}

void MySystem::EndFrame(){
	imGuiManager_->End();
	imGuiManager_->Draw();

    dxCommon_->EndFrame();

	/*sprite_.clear();*/
}

void MySystem::CreateTriangle(Triangle* triangle, const Vertices& vertex) {
	triangle->Init(dxCommon_->GetDevice().Get(), vertex);
}

void MySystem::CreateSprite(Sprite* sprite, const RectVetex& vertex) {
	sprite->Init(dxCommon_->GetDevice().Get(), vertex);
}

void MySystem::DrawTriangle(Triangle* triangle) {
	triangle->Draw(dxCommon_->GetCommandList().Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(0));
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void MySystem::DrawSprite(Sprite* sprite) {
	sprite->Draw(dxCommon_->GetCommandList().Get());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(0));
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void MySystem::DrawTriangle(const Matrix4x4& wvpMatrix, const Vertices& vertex){

	//int index = static_cast<int>(triangle_.size());

	//triangle_.emplace_back();
	//// ---------------------------------------------------------------
	//// ↓Vetrtexの設定
	//// ---------------------------------------------------------------
	//// VertexBufferViewを作成する 
	//triangle_[index].vertexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 3);
	//// リソースの先頭のアドレスから使う
	//triangle_[index].vertexBufferView_.BufferLocation = triangle_[index].vertexResource_->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点3つ分のサイズ
	//triangle_[index].vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	//// 1頂点当たりのサイズ
	//triangle_[index].vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//// Resourceにデータを書き込む 
	//VertexData* vertexData = nullptr;
	//// アドレスを取得
	//triangle_[index].vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//// 左下
	//vertexData[0].pos = vertex.vertex1;
	//vertexData[0].texcord = { 0.0f, 1.0f };
	//vertexData[0].normal = { 0.0f, 0.0f, -1.0f };
	//// 上
	//vertexData[1].pos = vertex.vertex2;
	//vertexData[1].texcord = { 0.5f, 0.0f };
	//vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
	//// 右下
	//vertexData[2].pos = vertex.vertex3;
	//vertexData[2].texcord = { 1.0f, 1.0f };
	//vertexData[2].normal = { 0.0f, 0.0f, -1.0f };
	//// ---------------------------------------------------------------

	//// ---------------------------------------------------------------
	//// ↓Materialの設定
	//// ---------------------------------------------------------------
	//// resourceの作成
	//triangle_[index].materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//Material* materialData = nullptr;
	//// アドレスを取得
	//triangle_[index].materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//// 色を決める
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLigthing = false;
	//materialData->uvTransform = MakeIdentity4x4();

	//// ---------------------------------------------------------------
	//// ↓Transformationの設定
	//// ---------------------------------------------------------------
	//// resourceの作成
	//triangle_[index].wvpResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Matrix4x4));
	//Matrix4x4* wvpData = nullptr;
	//triangle_[index].wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//*wvpData = wvpMatrix;

	//// コマンドを積む ------------------------------------------------------------------------------
	//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &triangle_[index].vertexBufferView_);
	//// materialCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, triangle_[index].materialResource_->GetGPUVirtualAddress());
	//// wvpCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, triangle_[index].wvpResource_->GetGPUVirtualAddress());
	//// どのtextureを読むのかをコマンドに積む
	//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(0));

	//dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void MySystem::DrawSprite(const Matrix4x4& worldMatrix, const Matrix4x4& wvpMatrix, const RectVetex& vertex, const kTransform& transrom) {
	//// VertexBufferViewを作成する 
	//sprite_.vertexResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 4);
	//// 頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = sprite_.vertexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点6つ分
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//// 1頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexDataSprite = nullptr;
	//sprite_.vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	//// 頂点データを設定する
	//// 一枚目
	//vertexDataSprite[0].pos = vertex.leftBottom;	// 左下
	//vertexDataSprite[0].texcord = { 0.0f, 1.0f };
	//vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };

	//vertexDataSprite[1].pos = vertex.leftTop;		// 左上
	//vertexDataSprite[1].texcord = { 0.0f, 0.0f };
	//vertexDataSprite[1].normal = { 0.0f, 0.0f, -1.0f };

	//vertexDataSprite[2].pos = vertex.rightBottom;	// 右下
	//vertexDataSprite[2].texcord = { 1.0f, 1.0f };
	//vertexDataSprite[2].normal = { 0.0f, 0.0f, -1.0f };

	//vertexDataSprite[3].pos = vertex.rightTop;	// 右上
	//vertexDataSprite[3].texcord = { 1.0f, 0.0f };
	//vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };

	//// 二枚目
	////vertexDataSprite[3].pos = vertex.leftTop;	// 左上
	////vertexDataSprite[3].texcord = { 0.0f, 0.0f };
	////vertexDataSprite[3].normal = { 0.0f, 0.0f, -1.0f };
	////vertexDataSprite[4].pos = vertex.rightTop;	// 右上
	////vertexDataSprite[4].texcord = { 1.0f, 0.0f };
	////vertexDataSprite[4].normal = { 0.0f, 0.0f, -1.0f };
	////vertexDataSprite[5].pos = vertex.rightBottom;	// 右下
	////vertexDataSprite[5].texcord = { 1.0f, 1.0f };
	////vertexDataSprite[5].normal = { 0.0f, 0.0f, -1.0f };

	//// ---------------------------------------------------------------
	//// ↓Indexの設定
	//// ---------------------------------------------------------------
	//sprite_.indexResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);
	//D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//indexBufferView.BufferLocation = sprite_.indexResourceSprite->GetGPUVirtualAddress();
	//indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//uint32_t* indexDataSprite = nullptr;
	//sprite_.indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//indexDataSprite[0] = 0;
	//indexDataSprite[1] = 1;
	//indexDataSprite[2] = 2;

	//indexDataSprite[3] = 1;
	//indexDataSprite[4] = 3;
	//indexDataSprite[5] = 2;

	//// ---------------------------------------------------------------
	//// ↓Materialの設定
	//// ---------------------------------------------------------------
	//// resourceの作成
	//sprite_.materialResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//// データを書く
	//Material* materialData = nullptr;
	//// アドレスを取得
	//sprite_.materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//// 色を決める
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLigthing = false;

	//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(transrom.scale);
	//uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(transrom.rotate.z));
	//uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(transrom.translate));
	//materialData->uvTransform = uvTransformMatrix;
	//

	//// ---------------------------------------------------------------
	//// ↓Transformationの設定
	//// ---------------------------------------------------------------
	//sprite_.transfomationMatrixResourceSprite = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TramsformationMatrix));
	//TramsformationMatrix* transformationMatrixDataSprite = nullptr;
	//sprite_.transfomationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//transformationMatrixDataSprite->WVP = wvpMatrix;
	//transformationMatrixDataSprite->World = worldMatrix;

	//// ---------------------------------------------------------------
	//// ↓Lightの設定
	//// ---------------------------------------------------------------
	//

	//// ---------------------------------------------------------------
	//// ↓ コマンドを積む
	//// ---------------------------------------------------------------
	//// spriteの描画
	//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	////
	//dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//// materialCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sprite_.materialResource_->GetGPUVirtualAddress());
	//// TransformationMatrixCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite_.transfomationMatrixResourceSprite->GetGPUVirtualAddress());
	//// どのtextureを読むのかをコマンドに積むcoo
	//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetSRVHandleGPU(0));
	//// 描画
	////dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);
	//dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void MySystem::DrawSphere(const Matrix4x4& worldMatrix, const Matrix4x4& wvpMatrix){
	//const uint32_t kSubdivision = 16;
	//const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);// fai
	//const float kLatEvery = float(M_PI) / float(kSubdivision); // theta

	//size = (int(kSubdivision / kLatEvery)) * (int(kSubdivision / kLonEvery)) * 6;
	////size = kSubdivision * kSubdivision * 6;

	//// ---------------------------------------------------------------
	//// ↓Vetrtexの設定
	//// ---------------------------------------------------------------
	//sphere_.vertexResource = CreateBufferResource(dxCommon_->GetDevice(), size * sizeof(VertexData));
	//// 頂点バッファビューを作成する 
	//sphere_.vertexBufferView.BufferLocation = sphere_.vertexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点6つ分
	//sphere_.vertexBufferView.SizeInBytes = size * sizeof(VertexData);
	//// 1頂点当たりのサイズ
	//sphere_.vertexBufferView.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexData = nullptr;
	//sphere_.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//uint32_t start = 0;

	//// ---------------------------------------------------------------
	//// ↓Indexの設定
	//// ---------------------------------------------------------------
	//sphere_.indexResource = CreateBufferResource(dxCommon_->GetDevice(), size * (sizeof(uint32_t)));
	//D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	//indexBufferView.BufferLocation = sphere_.indexResource->GetGPUVirtualAddress();
	//indexBufferView.SizeInBytes = size * (sizeof(uint32_t));
	//indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//uint32_t* indexData = nullptr;
	//sphere_.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	//uint32_t startIndex = 0;

	//// 頂点データの設定
	//// 緯度方向に分割
	//for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
	//	float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // theta
	//	// 経度方向に分割
	//	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
	//		start = (latIndex * kSubdivision + lonIndex) * 4;
	//		float lon = lonIndex * kLonEvery; // fai
	//		// 頂点データを入力
	//		// a 左下
	//		VertexData keepVertexData{};
	//		keepVertexData.pos.x = std::cos(lat) * std::cos(lon);
	//		keepVertexData.pos.y = std::sin(lat);
	//		keepVertexData.pos.z = std::cos(lat) * std::sin(lon);
	//		keepVertexData.pos.w = 1.0f;
	//		keepVertexData.texcord.x = float(lonIndex) / float(kSubdivision);
	//		keepVertexData.texcord.y = 1.0f - float(latIndex) / float(kSubdivision);
	//		keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
	//		vertexData[start] = keepVertexData;

	//		// b 左上
	//		keepVertexData.pos.x = std::cos(lat + kLatEvery) * std::cos(lon);
	//		keepVertexData.pos.y = std::sin(lat + kLatEvery);
	//		keepVertexData.pos.z = std::cos(lat + kLatEvery) * std::sin(lon);
	//		keepVertexData.pos.w = 1.0f;
	//		keepVertexData.texcord.x = float(lonIndex) / float(kSubdivision);
	//		keepVertexData.texcord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
	//		keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
	//		vertexData[start + 1] = keepVertexData;

	//		// c 右下
	//		keepVertexData.pos.x = std::cos(lat) * std::cos(lon + kLonEvery);
	//		keepVertexData.pos.y = std::sin(lat);
	//		keepVertexData.pos.z = std::cos(lat) * std::sin(lon + kLonEvery);
	//		keepVertexData.pos.w = 1.0f;
	//		keepVertexData.texcord.x = float(lonIndex + 1) / float(kSubdivision);
	//		keepVertexData.texcord.y = 1.0f - float(latIndex) / float(kSubdivision);
	//		keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
	//		vertexData[start + 2] = keepVertexData;

	//		// d 右上
	//		keepVertexData.pos.x = std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery);
	//		keepVertexData.pos.y = std::sin(lat + kLatEvery);
	//		keepVertexData.pos.z = std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery);
	//		keepVertexData.pos.w = 1.0f;
	//		keepVertexData.texcord.x = float(lonIndex + 1) / float(kSubdivision);
	//		keepVertexData.texcord.y = 1.0f - float(latIndex + 1) / float(kSubdivision);
	//		keepVertexData.normal = { keepVertexData.pos.x, keepVertexData.pos.y, keepVertexData.pos.z };
	//		vertexData[start + 3] = keepVertexData;

	//		//
	//		startIndex = (latIndex * kSubdivision + lonIndex) * 6;
	//		indexData[startIndex] = start;
	//		indexData[startIndex + 1] = start + 1;
	//		indexData[startIndex + 2] = start + 2;

	//		indexData[startIndex + 3] = start + 1;
	//		indexData[startIndex + 4] = start + 3;
	//		indexData[startIndex + 5] = start + 2;
	//	}
	//}

	//// ---------------------------------------------------------------
	//// ↓Materialの設定
	//// ---------------------------------------------------------------
	//// resourceの作成
	//sphere_.materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//// データを書く
	//Material* materialData = nullptr;
	//// アドレスを取得
	//sphere_.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//// 色を決める
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLigthing = true;
	//materialData->uvTransform = MakeIdentity4x4();

	//// ---------------------------------------------------------------
	//// ↓Transformationの設定
	//// ---------------------------------------------------------------
	//sphere_.transfomationMatrixResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TramsformationMatrix));
	//TramsformationMatrix* transformationMatrixDataSprite = nullptr;
	//sphere_.transfomationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//transformationMatrixDataSprite->WVP = wvpMatrix;
	//transformationMatrixDataSprite->World = worldMatrix;

	//// ---------------------------------------------------------------
	//// ↓Lightingの設定
	//// ---------------------------------------------------------------
	///*DirectionalLight* rawPointer = directionalLight_.get();
	//sphere_.directionalLightResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	//sphere_.directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	//directionalLight_->SetLight(
	//	{ 1.0f,1.0f, 1.0f, 1.0f },
	//	{ 0.0f, -1.0f, 0.0f },
	//	1.0f
	//);*/

	//Light* light = nullptr;
	//sphere_.directionalLightResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Light));
	//sphere_.directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&light));
	//light->color_ = { 1.0f,1.0f, 1.0f, 1.0f };
	//light->direction_ = { 0.0f, -1.0f, 0.0f };
	//light->intensity_ = 1.0f;

	//// ---------------------------------------------------------------
	//// ↓ コマンドを積む
	//// ---------------------------------------------------------------
	//// 三角形の描画がある前提の設定なため不完全
	//// spriteの描画
	//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &sphere_.vertexBufferView);
	////
	//dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//// materialCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, sphere_.materialResource->GetGPUVirtualAddress());
	//// TransformationMatrixCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, sphere_.transfomationMatrixResource->GetGPUVirtualAddress());
	//// 
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, sphere_.directionalLightResource->GetGPUVirtualAddress());
	//// どのtextureを読むのかをコマンドに積む
	//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonstorBall_ ? textureManager_->GetSRVHandleGPU(1) : textureManager_->GetSRVHandleGPU(0));
	//// 描画
	////dxCommon_->GetCommandList()->DrawInstanced(1536, 1, 0, 0);
	//dxCommon_->GetCommandList()->DrawIndexedInstanced(size, 1, 0, 0, 0);
}

void MySystem::DrawModel(const Matrix4x4& worldMatrix, const Matrix4x4& wvpMatrix) {
	////
	//modelData_ = LoadObjFile("Resource", "axis.obj");
	//// ---------------------------------------------------------------
	//// ↓Vetrtexの設定
	//// ---------------------------------------------------------------
	//model_.vertexResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	//// 頂点バッファビューを作成する 
	//model_.vertexBufferView.BufferLocation = model_.vertexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズは頂点6つ分
	//model_.vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//// 1頂点当たりのサイズ
	//model_.vertexBufferView.StrideInBytes = sizeof(VertexData);

	//VertexData* vertexData = nullptr;
	//model_.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	//// ---------------------------------------------------------------
	//// ↓Materialの設定
	//// ---------------------------------------------------------------
	//// resourceの作成
	//model_.materialResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	//// データを書く
	//Material* materialData = nullptr;
	//// アドレスを取得
	//model_.materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//// 色を決める
	//materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLigthing = true;
	//materialData->uvTransform = MakeIdentity4x4();

	//// ---------------------------------------------------------------
	//// ↓Transformationの設定
	//// ---------------------------------------------------------------
	//model_.transfomationMatrixResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(TramsformationMatrix));
	//TramsformationMatrix* transformationMatrixDataSprite = nullptr;
	//model_.transfomationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//transformationMatrixDataSprite->WVP = wvpMatrix;
	//transformationMatrixDataSprite->World = worldMatrix;

	//// ---------------------------------------------------------------
	//// ↓Lightingの設定
	//// ---------------------------------------------------------------
	///*DirectionalLight* rawPointer = directionalLight_.get();
	//sphere_.directionalLightResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(DirectionalLight));
	//sphere_.directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	//directionalLight_->SetLight(
	//	{ 1.0f,1.0f, 1.0f, 1.0f },
	//	{ 0.0f, -1.0f, 0.0f },
	//	1.0f
	//);*/

	//Light* light = nullptr;
	//model_.directionalLightResource = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Light));
	//model_.directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&light));
	//light->color_ = { 1.0f,1.0f, 1.0f, 1.0f };
	//light->direction_ = { 0.0f, -1.0f, 0.0f };
	//light->intensity_ = 1.0f;

	//// ---------------------------------------------------------------
	//// ↓ コマンドを積む
	//// ---------------------------------------------------------------
	//// 三角形の描画がある前提の設定なため不完全
	//dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &model_.vertexBufferView);
	////
	////dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//// materialCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, model_.materialResource->GetGPUVirtualAddress());
	//// TransformationMatrixCBufferの設定
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, model_.transfomationMatrixResource->GetGPUVirtualAddress());
	//// 
	//dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, model_.directionalLightResource->GetGPUVirtualAddress());
	//// どのtextureを読むのかをコマンドに積む
	//dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonstorBall_ ? textureManager_->GetSRVHandleGPU(1) : textureManager_->GetSRVHandleGPU(0));
	//// 描画
	//dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
	////dxCommon_->GetCommandList()->DrawIndexedInstanced(size, 1, 0, 0, 0);
}
