#pragma once

#include "GLHeader.h"


class CCamera
{
public:
    CCamera (float fov, float ratio, float near, float far);

    void Update (float deltaTime, float offsetH, float offsetV, float rotateH, float rotateV);
    
    const glm::mat4 &GetViewMatrix () const { return mViewMatrix; }
    const glm::mat4 &GetProjectionMatrix () const { return mProjectionMatrix; }

private:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    glm::vec3 mPosition = glm::vec3 (0, 10, 20);

    float mHorizontalAngle = 3.14f;
    float mVerticalAngle = 0.0f;
    
    float mSpeed = 3.0f;
    float mRotateSpeed = 0.005f; 
};
