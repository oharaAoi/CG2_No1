#include "Camera.h"

Camera::Camera(){
	Init();
}

Camera::~Camera(){
}

//===========================================================================================================================
//	初期化
//===========================================================================================================================
void Camera::Init(){
	cameraTransform_ = { {1.0f, 1.0f,1.0f},{0.0f, 0.0f,0.0f}, {0.0f, 0.0f, -10.0f} };

	cameraMatrix_ = MakeAffineMatrix(cameraTransform_);

	viewMatrix_ = Inverse(cameraMatrix_);

	prijectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);

	vpMatrix_ = Multiply(viewMatrix_, prijectionMatrix_);

	prijectionMatrix2D_ = MakeOrthograhicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	viewMatrix2D_ = MakeIdentity4x4();
	vpMatrix2D_ = Multiply(viewMatrix2D_, prijectionMatrix2D_);
}

//===========================================================================================================================
//	更新
//===========================================================================================================================
void Camera::Update(){
	cameraMatrix_ = MakeAffineMatrix(cameraTransform_);

	viewMatrix_ = Inverse(cameraMatrix_);

	prijectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);

	vpMatrix_ = Multiply(viewMatrix_, prijectionMatrix_);

	prijectionMatrix2D_ = MakeOrthograhicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	viewMatrix2D_ = MakeIdentity4x4();
	vpMatrix2D_ = Multiply(viewMatrix2D_, prijectionMatrix2D_);

	ImGui::Begin("Camera");
	ImGui::DragFloat3("transform", &cameraTransform_.translate.x, 0.1f);
	ImGui::End();
}

//===========================================================================================================================
//	描画
//===========================================================================================================================
void Camera::Draw(){

}
