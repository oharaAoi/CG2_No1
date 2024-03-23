#include <Windows.h>
#include <memory>
#include "MySystem.h"

#include "Camera/Camera.h"

#include "Transform.h"
#include "VertexData.h"

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

enum ObjectKind {
	kTriangle
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	OutputDebugStringA("Hello, DirectX");

	std::unique_ptr<MySystem> system = std::make_unique<MySystem>();
	system->Initialize(kWindowWidth, kWindowHeight);

	std::unique_ptr<Camera> camera = std::make_unique<Camera>();
	camera->Init();

	// triangle
	std::unique_ptr<Triangle> triangle1 = std::make_unique<Triangle>();
	std::unique_ptr<Triangle> triangle2 = std::make_unique<Triangle>();

	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();

	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();

	int kind = ObjectKind::kTriangle;

	// 変数
	kTransform triangleTransform1 = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector4 triangleColor1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vertices vertex1 = {
		{-0.5f, -0.5f, 0.0f, 1.0f},
		{ 0.0f, 0.5f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, 0.0f, 1.0f }
	};

	kTransform triangleTransform2 = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector4 triangleColor2 = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vertices vertex2 = {
		{-0.5f, -0.5f, 0.5f, 1.0f},
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f }
	};

	kTransform spriteTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	kTransform uvTransformSprite = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector4 spriteColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	RectVetex rect = {
		{0.0f, 0.0f, 0.0f, 1.0f},
		{640.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 360.0f, 0.0f, 1.0f},
		{640.0f, 360.0f, 0.0f, 1.0f}
	};
	

	kTransform sphereTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Vector4 sphereColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 sphereWorldMatrix{};
	Matrix4x4 sphereWvpMatrix{};



	// triangleの初期化
	system->CreateTriangle(triangle1.get(), vertex1);
	system->CreateTriangle(triangle2.get(), vertex2);

	system->CreateSprite(sprite.get(), rect);

	system->CreateShpere(sphere.get(), 16);

	while (system->ProcessMessage()) {
		system->BeginFrame();
		
		ImGui::ShowDemoWindow();

		camera->Update();

		// ---------------------------------------------------------------
		//	↓ 更新処理
		// ---------------------------------------------------------------
		// 三角形の更新
		triangle1->Update(triangleTransform1, camera->GetVpMatrix(), triangleColor1);
		triangle2->Update(triangleTransform2, camera->GetVpMatrix(), triangleColor2);

		// ---------------------------------------------------------------
		// spriteの更新
		sprite->Update(uvTransformSprite, spriteTransform, camera->GetVpMatrix2D(), spriteColor);

		// ---------------------------------------------------------------
		// sphereの更新
		sphereTransform.rotate.y += 0.01f;
		sphere->Update(sphereTransform, camera->GetVpMatrix(), sphereColor);

		// ---------------------------------------------------------------
		//	↓ 描画処理
		// ---------------------------------------------------------------
		/*system->DrawTriangle(triangle1.get());
		system->DrawTriangle(triangle2.get());

		system->DrawSprite(sprite.get());*/

		system->DrawSphere(sphere.get());

		// 三角形の描画
		/*system->DrawTriangle(wvpMatrix, vertex1);
		system->DrawTriangle(wvpMatrix, vertex2);*/

		//system->DrawSphere(sphereWorldMatrix, sphereWvpMatrix);
		//system->DrawModel(sphereWorldMatrix, sphereWvpMatrix);

		// 2d描画
		//system->DrawSprite(spriteWorldMatrix, spriteWvpMatrix, rect, uvTransformSprite);

#ifdef _DEBUG

		ImGui::Begin("Setting");
		ImGui::Combo("object", &kind, "Triangle\0Sprite\0cccc\0dddd\0eeee\0\0");
		ImGui::End();


		ImGui::Begin("Triangle");
		ImGui::DragFloat3("scale", &triangleTransform1.scale.x, 0.01f);
		ImGui::DragFloat3("rotate", &triangleTransform1.rotate.x, 0.01f);
		ImGui::DragFloat3("translate", &triangleTransform1.translate.x, 0.01f);
		ImGui::ColorPicker4("color", &triangleColor1.x);
		ImGui::End();

		ImGui::Begin("Sprite");
		ImGui::DragFloat2("translate", &spriteTransform.translate.x, 1);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -1.0f, 10.0f);
		ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -1.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		ImGui::ColorPicker4("color", &spriteColor.x);

		ImGui::End();

		ImGui::Begin("Sphere");
		ImGui::DragFloat3("scale", &sphereTransform.scale.x, 0.1f);
		ImGui::DragFloat3("rotate", &sphereTransform.rotate.x, 0.1f);
		ImGui::DragFloat3("translate", &sphereTransform.translate.x, 0.1f);

		ImGui::End();
#endif // _DEBUG

		system->EndFrame();
	}

	system->Finalize();
	return 0;
}