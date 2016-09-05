#pragma once

#include "Environment.h"
#include "WaveSimulator.h"

class CCamera;


class COceanObject
{
public:
	COceanObject (SEnvironmentParams env);
	~COceanObject ();

	void Update (float currentTime);
	void Render (const CCamera &camera);
    
    void DebugSave (const char *filename);

private:
	void InitBuffer ();
	void InitShader ();

private:
    enum VertexBufferType
    {
        Position,
        UV,
        Indice,
        BufferTypeMax,
    };
    
    enum UniformType
    {
        DisplacementTexture,
        NormalTexture,
        Mvp,
        Mv,
        Mvn,
		SunDir,
        UniformMax,
    };
    
    enum TextureType
    {
        Displacement,
        Normal,
        TextureMax,
    };

private:
	const int mVertexSize = 512;

	GLuint mShaderProgram, mUniform[UniformType::UniformMax];
    GLuint mVao, mVbo[VertexBufferType::BufferTypeMax];
    GLuint mTexture[TextureType::TextureMax];
	GLsizei mIndiceCount;

	SEnvironmentParams mEnvironmentParams;
	CWaveSimulator mWaveSimulator;
	float mUpdateRate = 0.05f;
};
