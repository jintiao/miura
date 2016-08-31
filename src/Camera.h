#pragma once

#include "GLHeader.h"


class CCamera
{
public:
    CCamera (float fov, float ratio, float near, float far);

    void Update (float deltaTime, float offsetH, float offsetV, float rotateH, float rotateV, bool boost);
    
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
