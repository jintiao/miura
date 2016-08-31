#include "Camera.h"


CCamera::CCamera (float fov, float ratio, float near, float far)
{
    mProjectionMatrix = glm::perspective (fov, ratio, near, far);
    Update (0, 0, 0, 0, 0);
}


void CCamera::Update (float deltaTime, float offsetH, float offsetV, float rotateX, float rotateY)
{
    mHorizontalAngle += mRotateSpeed * rotateX * deltaTime;
	mVerticalAngle   += mRotateSpeed * rotateY * deltaTime;

	glm::vec3 forward (
		std::cos(mVerticalAngle) * std::sin(mHorizontalAngle),
		std::sin(mVerticalAngle),
		std::cos(mVerticalAngle) * std::cos(mHorizontalAngle)
	);

	glm::vec3 right = glm::vec3 (std::sin(mHorizontalAngle - 3.14f / 2.0f), 0, std::cos(mVerticalAngle - 3.14f / 2.0f));
	
    mPosition += right * mSpeed * offsetH * deltaTime;
    mPosition += forward * mSpeed * offsetV * deltaTime;
    
	// Up vector
	glm::vec3 up = glm::cross (right, forward);
    
    mViewMatrix = glm::lookAt (mPosition, mPosition + forward, up);
}
