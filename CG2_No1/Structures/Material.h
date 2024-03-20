#pragma once
#include "Vector4.h"
#include "Matrix3x3.h"
#include <cstdint>

struct Material {
	Vector4 color;
	uint32_t enableLigthing;
	float padding[3];
	Matrix4x4 uvTransform;
};