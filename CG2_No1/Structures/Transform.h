#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

struct TramsformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct kTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};