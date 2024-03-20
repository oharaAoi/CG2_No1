#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(){
}

DirectionalLight::~DirectionalLight(){
}

void DirectionalLight::Init(){}

void DirectionalLight::Update(){}

void DirectionalLight::Draw(){}

void DirectionalLight::SetLight(const Vector4& color, const Vector3& dire, const float& intensity){
	color_ = color;
	direction_ = (dire);
	intensity_ = intensity;
}