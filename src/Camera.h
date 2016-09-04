#pragma once


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
        bool IsEmpty () { return (offsetH == 0 && offsetV == 0 && rotateH == 0 && rotateV == 0); }
	};

public:
    CCamera (float fov, float ratio, float near, float far);

    void Update (float deltaTime, SUpdateParams params);
    
    const Math::Matrix4 &GetViewMatrix () const { return mViewMatrix; }
    const Math::Matrix4 &GetViewNormalMatrix () const { return mViewNormalMatrix; }
    const Math::Matrix4 &GetProjectionMatrix () const { return mProjectionMatrix; }

private:
    Math::Matrix4 mViewMatrix;
    Math::Matrix4 mViewNormalMatrix;
    Math::Matrix4 mProjectionMatrix;

    Math::Vector3 mPosition = Math::Vector3 (0, 200, -350);

    float mHorizontalAngle = 0.1f;
    float mVerticalAngle = -0.6f;
    
    float mSpeedBoost = 10.0f;
    float mSpeed = 10.0f; // meters / second
    float mRotateSpeed = 1.0f; // radians / second
};
