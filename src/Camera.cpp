#include "GlobalHeader.h"

#include "Camera.h"

#include <algorithm>


CCamera::CCamera (float fov, float ratio, float near, float far)
{
    mProjectionMatrix = Math::CreatePerspectiveMatrix (fov, ratio, near, far);
    Update (0, SUpdateParams ());
}


void CCamera::Update (float deltaTime, SUpdateParams params)
{
    static Math::Vector3 DEFAULT_UP = { 0.0f, 1.0f, 0.0f };
	static float ANGLE_MAX = (float)(Math::pi * 0.5f), ANGLE_MIN = -ANGLE_MAX;

    mHorizontalAngle += mRotateSpeed * params.rotateH * deltaTime;
	mVerticalAngle += mRotateSpeed * params.rotateV * deltaTime;
	mVerticalAngle = std::min (std::max (mVerticalAngle, ANGLE_MIN), ANGLE_MAX);

	Math::Vector3 forward (std::cosf (mVerticalAngle) * std::sinf (mHorizontalAngle), std::sinf (mVerticalAngle), std::cosf (mVerticalAngle) * std::cosf (mHorizontalAngle));
    Math::Vector3 right = Math::Cross (forward, DEFAULT_UP);
	Math::Vector3 up = Math::Cross (right, forward);
	
    mPosition += right * mSpeed * params.offsetH * deltaTime * (params.boost ? mSpeedBoost : 1.0f);
    mPosition += forward * mSpeed * params.offsetV * deltaTime * (params.boost ? mSpeedBoost : 1.0f);

    mViewMatrix = Math::CreateLookatMatrix (mPosition, mPosition + forward, up);
}
