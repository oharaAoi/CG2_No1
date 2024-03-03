#pragma once
#include "DirectXCommon.h"
#include "Function/DirectXUtils.h"
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImGuiManager{

public: // 静的メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstacne();

	static bool ImGuiHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="dxCommon"></param>
	void Init(WinApp* winApp, DirectXCommon* dxCommon);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 使う
	/// </summary>
	void Begin();

	/// <summary>
	/// ゲームの処理が終わった後
	/// </summary>
	void End();

	/// <summary>
	/// 描画する
	/// </summary>
	void Draw();

private:

	DirectXCommon* dxCommon_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDiscriptorHeap_ = nullptr;

};

