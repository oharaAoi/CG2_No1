#pragma once
#include "Vector4.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "MyMatrix.h"
#include "Function/DirectXUtils.h"
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <wrl.h>

class Material {
public: // 構造体

	struct kMaterial {
		Vector4 color;
		uint32_t enableLigthing;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct MaterialData {
		std::string textureFilePath;
	};

public:

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

public:

	Material();
	~Material();

	void Init(ID3D12Device* device);

	void Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& wvpMatrix, const Vector4& color);

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuffer_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformBuffer_;

	kMaterial* materialData;
	TramsformationMatrix* transformationData;

};

