#include "Material.h"

Material::Material() {
}

Material::~Material() {
}

void Material::Init(ID3D12Device* device) {
	// ---------------------------------------------------------------
	// ↓Materialの設定
	// ---------------------------------------------------------------
	materialBuffer_ = CreateBufferResource(device, sizeof(kMaterial));
	materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 色を決める
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// lightの有無
	materialData->enableLigthing = false;
	// UVTransform
	materialData->uvTransform = MakeIdentity4x4();

	// ---------------------------------------------------------------
	// ↓Transformの設定
	// ---------------------------------------------------------------
	transformBuffer_ = CreateBufferResource(device, sizeof(TramsformationMatrix));
	transformationData = nullptr;
	transformBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData));
	transformationData->WVP = MakeIdentity4x4();
	transformationData->World = MakeIdentity4x4();
}

void Material::Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& wvpMatrix, const Vector4& color) {
	materialData->color = color;
	transformationData->WVP = wvpMatrix;

	commandList->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transformBuffer_->GetGPUVirtualAddress());
}

Material::MaterialData Material::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + fileName);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}