#include "Camera.h"


CCamera::CCamera (float fov, float ratio, float near, float far)
{
    mProjectionMatrix = glm::perspective (fov, ratio, near, far);
    Update (0, 0, 0, 0, 0, false);
}


void CCamera::Update (float deltaTime, float offsetH, float offsetV, float rotateX, float rotateY, bool boost)
{
    static glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };

    mHorizontalAngle += mRotateSpeed * rotateX * deltaTime;
	mVerticalAngle += mRotateSpeed * rotateY * deltaTime;

	glm::vec3 forward (
		std::cos(mVerticalAngle) * std::sin(mHorizontalAngle),
		std::sin(mVerticalAngle),
		std::cos(mVerticalAngle) * std::cos(mHorizontalAngle)
	);
    
    glm::vec3 right = glm::cross (forward, worldUp);
	
    mPosition += right * mSpeed * offsetH * deltaTime * (boost ? mSpeedBoost : 1.0f);
    mPosition += forward * mSpeed * offsetV * deltaTime * (boost ? mSpeedBoost : 1.0f);
    
	// Up vector
	glm::vec3 up = glm::cross (right, forward);
    
    mViewMatrix = glm::lookAt (mPosition, mPosition + forward, up);
}
