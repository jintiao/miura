#pragma once

#include "GLHeader.h"


class CCamera
{
public:
	struct SUpdateParams
	{
		float offsetH;
		float offsetV;
		float rotateH;
		float rotateV;
		bool boost;
		SUpdateParams (float oh = 0, float ov = 0, float rh = 0, float rv = 0, bool b = false) : offsetH (oh), offsetV (ov), rotateH (rh), rotateV (rv), boost (b) {}
	};

public:
    CCamera (float fov, float ratio, float near, float far);

    void Update (float deltaTime, SUpdateParams params);
    
    const glm::mat4 &GetViewMatrix () const { return mViewMatrix; }
    const glm::mat4 &GetProjectionMatrix () const { return mProjectionMatrix; }

private:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    glm::vec3 mPosition = glm::vec3 (0, 50, -150);

    float mHorizontalAngle = 0.0f;
    float mVerticalAngle = -0.3f;
    
    float mSpeedBoost = 5.0f;
    float mSpeed = 3.0f; // meters / second
    float mRotateSpeed = 1.57f; // radians / second
};
