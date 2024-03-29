#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <memory>

// 
#include "DirectionalLight.h"

// 
#include "Transform.h"
#include "Sprite.h"
#include "Model.h"

#include "Triangle.h"
#include "Sphere.h"

// 
#include "Material.h"
//#include "Transform.h"

// 
#include "MyMatrix.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

class MySystem{
public: // メンバ関数

	MySystem();
	~MySystem();

	/// <summary>
	/// ループの判定
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	void Initialize(uint32_t backBufferWidth, int32_t backBufferHeight);

	void Finalize();

	void BeginFrame();
	void EndFrame();

public:// 設定する関数

	void CreateTriangle(Triangle* triangle, const Vertices& vertex);

	void CreateSprite(Sprite* sprite, const RectVetex& vertex);

	void CreateShpere(Sphere* sphere, const uint32_t& division);

public:// 描画する関数

	void DrawTriangle(Triangle* triangle);

	void DrawSprite(Sprite* sprite);

	void DrawSphere(Sphere* sphere);

public: // メンバ関数

	void DrawTriangle(const Matrix4x4& wvpMatrix, const Vertices& vertex);

	void DrawSprite(const Matrix4x4& worldMatrix,
		const Matrix4x4& wvpMatrix,
		const RectVetex& vertex, const kTransform& transrom);

	void DrawSphere(const Matrix4x4& worldMatrix, const Matrix4x4& wvpMatrix);

	void DrawModel(const Matrix4x4& worldMatrix, const Matrix4x4& wvpMatrix);

private:

	struct kSprite {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResourceSprite;
		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	};

	struct kSphere {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};

	struct Model {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};

	struct Light {
		Vector4 color_;
		Vector3 direction_;
		float intensity_;
	};

private:

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	uint32_t size;

	ModelData modelData_;

	bool useMonstorBall_;

private:

	//struct Light {
	//	std::unique_ptr<DirectionalLight> directionalLight_;
	//};

	//std::unique_ptr<Light> light_;

	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;
	ImGuiManager* imGuiManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;

private:
	
	kSprite sprite_;
	kSphere sphere_;
	Model model_;

	std::vector<std::unique_ptr<Triangle>> triangleArray_;
};

