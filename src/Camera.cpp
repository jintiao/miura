#include "Camera.h"

#include <algorithm>
#include <cmath>


CCamera::CCamera (float fov, float ratio, float near, float far)
{
    mProjectionMatrix = glm::perspective (fov, ratio, near, far);
    Update (0, SUpdateParams ());
}


void CCamera::Update (float deltaTime, SUpdateParams params)
{
    static glm::vec3 DEFAULT_UP = { 0.0f, 1.0f, 0.0f };
	static float ANGLE_MAX = (float)(M_PI / 2.0), ANGLE_MIN = -ANGLE_MAX;

    mHorizontalAngle += mRotateSpeed * params.rotateH * deltaTime;
	mVerticalAngle += mRotateSpeed * params.rotateV * deltaTime;
	mVerticalAngle = std::min (std::max (mVerticalAngle, ANGLE_MIN), ANGLE_MAX);

	glm::vec3 forward (std::cosf (mVerticalAngle) * std::sinf (mHorizontalAngle), std::sinf (mVerticalAngle), std::cosf (mVerticalAngle) * std::cosf (mHorizontalAngle));
    glm::vec3 right = glm::cross (forward, DEFAULT_UP);
	glm::vec3 up = glm::cross (right, forward);
	
    mPosition += right * mSpeed * params.offsetH * deltaTime * (params.boost ? mSpeedBoost : 1.0f);
    mPosition += forward * mSpeed * params.offsetV * deltaTime * (params.boost ? mSpeedBoost : 1.0f);

    mViewMatrix = glm::lookAt (mPosition, mPosition + forward, up);
}
