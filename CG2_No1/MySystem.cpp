#include "MySystem.h"

bool MySystem::ProcessMessage(){
    return  winApp_->ProcessMessage();
}

void MySystem::Initialize(uint32_t backBufferWidth, int32_t backBufferHeight){
    // ↓インスタンスの生成
    winApp_ = WinApp::GetInstance();
    dxCommon_ = DirectXCommon::GetInstacne();

    // ↓各初期化
    winApp_->CreateGameWindow();
    dxCommon_->Initialize(winApp_, backBufferWidth, backBufferHeight);
}

void MySystem::BeginFrame(){
    dxCommon_->BeginFrame();
}

void MySystem::EndFrame(){
    dxCommon_->EndFrame();
}
