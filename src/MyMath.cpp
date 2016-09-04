#include "GlobalHeader.h"

#include "MyMath.h"


namespace Math
{


float Degree2Radians (float degree)
{
    return glm::radians(degree);
}


Matrix4 CreatePerspectiveMatrix (float fov, float ratio, float near, float far)
{
    return glm::perspective (fov, ratio, near, far);
}


Matrix4 CreateLookatMatrix (const Vector3 &eye, const Vector3 &at, const Vector3 &up)
{
    return glm::lookAt (eye, at, up);
}


Vector3 Cross (const Vector3 &lhs, const Vector3 &rhs)
{
    return glm::cross (lhs, rhs);
}


float Dot (const Vector2 &lhs, const Vector2 &rhs)
{
    return glm::dot (lhs, rhs);
}


float Length (const Vector2 &v)
{
    return glm::length (v);
}


Vector2 Normalize (const Vector2 &v)
{
    return glm::normalize (v);
}


Vector3 Normalize (const Vector3 &v)
{
    return glm::normalize (v);
}


Matrix4 Invert (const Matrix4 &m)
{
    return glm::inverse(m);
}


Matrix4 Transpose (const Matrix4 &m)
{
    return glm::transpose(m);
}


} // namespace Math