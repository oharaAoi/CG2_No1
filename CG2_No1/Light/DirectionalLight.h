#pragma once
#include "BaseLight.h"

class DirectionalLight
: public BaseLight {

private:



public:

	DirectionalLight();
	~DirectionalLight();

	void Init() override;
	void Update() override;
	void Draw() override;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="color"></param>
	/// <param name="dire"></param>
	/// <param name="intensity"></param>
	void SetLight(const Vector4& color, const Vector3& dire, const float& intensity);

};

