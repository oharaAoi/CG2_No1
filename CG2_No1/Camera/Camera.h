#pragma once
#include "MyMatrix.h"
#include "Transform.h"
#include "ImGuiManager.h"

class Camera{
private:

	kTransform cameraTransform_;

	Matrix4x4 cameraMatrix_;
	Matrix4x4 prijectionMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 vpMatrix_;

	Matrix4x4 prijectionMatrix2D_;
	Matrix4x4 viewMatrix2D_;
	Matrix4x4 vpMatrix2D_;

public:

	Camera();
	~Camera();

	void Init();
	void Update();
	void Draw();

	/// accsser
	Matrix4x4 GetVpMatrix() const { return vpMatrix_; }

	Matrix4x4 GetVpMatrix2D() const { return vpMatrix2D_; }
};

