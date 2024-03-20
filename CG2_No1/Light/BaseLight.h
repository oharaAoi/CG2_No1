#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "MyMath.h"

class BaseLight{
protected:

	Vector4 color_;
	Vector3 direction_;
	float intensity_;

public:

	BaseLight() = default;
	~BaseLight() = default;
	
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

};

