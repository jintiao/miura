#pragma once

#ifdef _WIN
#pragma warning(disable : 4201)
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Math
{

static const float pi = (float)M_PI;

typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::mat4 Matrix4;

float Degree2Radians (float degree);

Matrix4 CreatePerspectiveMatrix (float fov, float ratio, float near, float far);

Matrix4 CreateLookatMatrix (const Vector3 &eye, const Vector3 &at, const Vector3 &up);

Vector3 Cross (const Vector3 &lhs, const Vector3 &rhs);

float Dot (const Vector2 &lhs, const Vector2 &rhs);

float Dot (const Vector3 &lhs, const Vector3 &rhs);

float Length (const Vector2 &v);

Vector2 Normalize (const Vector2 &v);

Vector3 Normalize (const Vector3 &v);

Matrix4 Invert (const Matrix4 &m);

Matrix4 Transpose (const Matrix4 &m);

} // namespace Math
