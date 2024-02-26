#include <Windows.h>
#include <memory>
#include "MySystem.h"

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	OutputDebugStringA("Hello, DirectX");

	std::unique_ptr<MySystem> system = std::make_unique<MySystem>();
	system->Initialize(kWindowWidth, kWindowHeight);

	while (system->ProcessMessage()) {
		system->BeginFrame();

		system->EndFrame();
	}

	return 0;
}