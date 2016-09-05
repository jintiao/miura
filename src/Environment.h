#pragma once


struct SOceanParams
{
	Math::Vector2 windDirection;
	float windSpeed;

	SOceanParams (float angle = 0.4f, float speed = 31.0f) :
        windDirection (std::cosf (angle), std::sinf (angle)),
        windSpeed (speed)
    {
    }
};


struct SEnvironmentParams : public SOceanParams
{
    Math::Vector3 sunDirection;

    SEnvironmentParams (Math::Vector3 dir = Math::Vector3 (1.0f, 1.0f, 1.0f)) :
        sunDirection (Math::Normalize (dir))
    {
    }
};
