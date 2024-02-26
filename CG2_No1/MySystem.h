#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

class MySystem{
public: // メンバ関数

	/// <summary>
	/// ループの判定
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	void Initialize(uint32_t backBufferWidth, int32_t backBufferHeight);

	void BeginFrame();
	void EndFrame();

private:

	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

};

