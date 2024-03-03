#include <Windows.h>
#include <memory>
#include "MySystem.h"

#include "Camera/Camera.h"

#include "Transform.h"

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	OutputDebugStringA("Hello, DirectX");

	std::unique_ptr<MySystem> system = std::make_unique<MySystem>();
	system->Initialize(kWindowWidth, kWindowHeight);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Init();

	// 変数
	kTransform transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Matrix4x4 worldMatrix{};
	Matrix4x4 wvpMatrix{};

	while (system->ProcessMessage()) {
		system->BeginFrame();
		
		ImGui::ShowDemoWindow();

		// move ----------------------------------------------------------
		transform.rotate.y += 0.01f;
		worldMatrix = MakeAffineMatrix(transform);
		wvpMatrix = Multiply(worldMatrix, camera->GetVpMatrix());
		// ---------------------------------------------------------------

		// 三角形の描画
		system->DrawTriangle(wvpMatrix);

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}