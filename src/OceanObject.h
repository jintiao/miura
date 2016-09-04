#pragma once

#include "Environment.h"
#include "WaveSimulator.h"

class CCamera;


class COceanObject
{
public:
	COceanObject ();
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
        Mvp,
        Mv,
        Mvn,
        WaveTexture,
        UniformMax,
    };

private:
	GLuint mShaderProgram, mUniform[UniformType::UniformMax];
    GLuint mVao, mVbo[VertexBufferType::BufferTypeMax];
    GLuint mWaveTexture;
	GLsizei mIndiceCount;

	CWaveSimulator mWaveSimulator;
};
