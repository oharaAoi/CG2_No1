#include "TextureManager.h"

TextureManager* TextureManager::GetInstacne() {
	static TextureManager instance;
	return &instance;
}

//=================================================================================================================
//	↓初期化
//=================================================================================================================
void TextureManager::Initialize(DirectXCommon* dxCommon){
	assert(dxCommon);

	dxCommon_ = dxCommon;

	srvData_.clear();

	ModelData modelData = LoadObjFile("Resource", "axis.obj");

	CreateShaderResource("Resource/uvChecker.png");
	//CreateShaderResource("Resource/monsterBall.png");
	CreateShaderResource(modelData.material.textureFilePath);
}

void TextureManager::Finalize(){
}

void TextureManager::CreateShaderResource(const std::string& filePath) {

	SRVData data{};

	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImage.GetMetadata();
	data.textureResource_ = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	data.intermediateResource_ = UploadTextureData(data.textureResource_, mipImage, dxCommon_->GetDevice(), dxCommon_->GetCommandList());

	// ------------------------------------------------------------
	// metadataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// ------------------------------------------------------------
	// SRVを作成するDescriptorHeapの場所を求める
	data.srvHandleCPU_ = GetCPUDescriptorHandle(dxCommon_->GetSRVDiscriptorHeap().Get(), dxCommon_->GetDescriptorSizeSRV(), (int(srvData_.size()) + 1));
	data.srvHandleGPU_ = GetGPUDescriptorHandle(dxCommon_->GetSRVDiscriptorHeap().Get(), dxCommon_->GetDescriptorSizeSRV(), (int(srvData_.size()) + 1));
	// 先頭はImGuiが使っている溜めその次を使う
	data.srvHandleCPU_.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	data.srvHandleGPU_.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 配列に入れる
	srvData_.push_back(data);

	// 生成
	dxCommon_->GetDevice()->CreateShaderResourceView(data.textureResource_.Get(), &srvDesc, data.srvHandleCPU_);
}

//=================================================================================================================
//	↓Textrueデータを読む
//=================================================================================================================
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath){
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertWString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}

//=================================================================================================================
//	↓Resourceの生成
//=================================================================================================================
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata){
	// metaDataを元にResourceを設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metadata.width);								// Textureの幅
	desc.Height = UINT(metadata.height);							// Textureの高さ
	desc.MipLevels = UINT16(metadata.mipLevels);					// mipmapの数
	desc.DepthOrArraySize = UINT16(metadata.arraySize);				// 奥行き　or 配列Textureの配数
	desc.Format = metadata.format;									// TextureのFormat
	desc.SampleDesc.Count = 1;										// サンプリングカウント
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);	// Textureの次元数

	// HEAPの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,					// Heapの設定
		D3D12_HEAP_FLAG_NONE,				// Heapの特殊の設定
		&desc,								// Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	// 初回のResourceState Textureは木基本読むだけ
		nullptr,							// 初回のResourceState Textureは木基本読むだけ
		IID_PPV_ARGS(&resource)				// 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture,
	const DirectX::ScratchImage& mipImage,
	Microsoft::WRL::ComPtr<ID3D12Device> device,
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList){

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device.Get(), mipImage.GetImages(), mipImage.GetImageCount(), mipImage.GetMetadata(), subresources); // subresourceの生成
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));						// 必要なサイズを求める
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);

	// データ転送をコマンドに積む
	UpdateSubresources(commandList.Get(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}
