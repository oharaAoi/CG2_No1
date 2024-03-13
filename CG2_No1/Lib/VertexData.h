#pragma once
#include "Vector4.h"
#include "Vector2.h"

struct VertexData {
	Vector4 pos;
	Vector2 texcord;
};

struct Vertices {
	Vector4 vertex1;
	Vector4 vertex2;
	Vector4 vertex3;
};

struct RectVetex {
	Vector4 leftTop;
	Vector4 rightTop;
	Vector4 leftBottom;
	Vector4 rightBottom;
};