#include <Windows.h>
#include <memory>
#include "MySystem.h"

#include "Camera/Camera.h"

#include "Transform.h"
#include "VertexData.h"

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

	kTransform spriteTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	kTransform spriteTransform2 = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {640.0f, 0.0f, 0.0f} };
	Matrix4x4 spriteWorldMatrix{};
	Matrix4x4 spriteWvpMatrix{};

	kTransform sphereTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Matrix4x4 sphereWorldMatrix{};
	Matrix4x4 sphereWvpMatrix{};

	Vertices vertex1 = {
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{ 0.0f, 0.5f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.0f, 1.0f }
	};

	Vertices vertex2 = {
		{-0.5f, -0.5f, 0.5f, 1.0f},
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f }
	};

	RectVetex rect = {
		{0.0f, 0.0f, 0.0f, 1.0f},
		{640.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 360.0f, 0.0f, 1.0f},
		{640.0f, 360.0f, 0.0f, 1.0f}
	};

	while (system->ProcessMessage()) {
		system->BeginFrame();
		
		ImGui::ShowDemoWindow();

		camera->Update();

		// move ----------------------------------------------------------
		transform.rotate.y += 0.01f;
		worldMatrix = MakeAffineMatrix(transform);
		wvpMatrix = Multiply(worldMatrix, camera->GetVpMatrix());
		// ---------------------------------------------------------------

		spriteWorldMatrix = MakeAffineMatrix(spriteTransform);
		spriteWvpMatrix = Multiply(spriteWorldMatrix, camera->GetVpMatrix2D());
		// ---------------------------------------------------------------
		sphereTransform.rotate.y += 0.01f;
		sphereWorldMatrix = MakeAffineMatrix(sphereTransform);
		sphereWvpMatrix = Multiply(sphereWorldMatrix, camera->GetVpMatrix());

		// 三角形の描画
		/*system->DrawTriangle(wvpMatrix, vertex1);
		system->DrawTriangle(wvpMatrix, vertex2);*/

		system->DrawSphere(sphereWvpMatrix);

		// 2d描画
		system->DrawSprite(spriteWvpMatrix, rect);

		ImGui::Begin("Sprite");
		ImGui::DragFloat3("transform", &spriteTransform.translate.x, 1);
		ImGui::End();

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}