#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

// 
#include "Triangle.h"
#include "Sprite.h"

// 
#include "MyMatrix.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

class MySystem{
public: // メンバ関数

	MySystem() = default;
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

public: // メンバ関数

	void DrawTriangle(const Matrix4x4& wvpMatrix, const Vertices& vertex);

	void DrawSprite(const Matrix4x4& wvpMatrix, const RectVetex& vertex);

	void DrawSphere(const Matrix4x4& wvpMatrix);

private:

	struct kSphere {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
		Microsoft::WRL::ComPtr<ID3D12Resource> transfomationMatrixResource;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	};

private:

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;
	ImGuiManager* imGuiManager_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	// 
	std::vector<kTriangle> triangle_;
	kSprite sprite_;
	kSphere sphere_;

	uint32_t size;
};

