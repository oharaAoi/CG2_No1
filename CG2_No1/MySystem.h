#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

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

private:

	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;

};

