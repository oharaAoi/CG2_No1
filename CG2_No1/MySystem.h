#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

// 
#include "Triangle.h"

// 
#include "MyMatrix.h"

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

	void DrawTriangle(const Matrix4x4 worldMatrix);

private:

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;

	// 
	kTriangle triangle_;

};

