#pragma once


struct SWaveParams
{
	Math::Vector2 windDirection;
	float windSpeed;
    float waveHeightMax;
    SWaveParams (float angle = 0.4f, float speed = 1.0f, float height = 200.0f) :
        windDirection (std::cosf (angle), std::sinf (angle)),
        windSpeed (speed),
        waveHeightMax (height)
    {
    }
};


struct SEnvironmentParams : public SWaveParams
{
    Math::Vector3 sunDirection;
    Math::Vector3 ambientColor;
    SEnvironmentParams (Math::Vector3 dir = Math::Vector3 (1.0f, 1.0f, 1.0f), Math::Vector3 amb = Math::Vector3 (0.1f, 0.1f, 0.1f)) :
        sunDirection (dir),
        ambientColor (amb)
    {
    }
};
