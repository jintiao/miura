#pragma once

#include "GLHeader.h"
#include "WaveSimulator.h"

class CCamera;

class COceanObject
{
public:
	COceanObject ();
	~COceanObject ();

	void Update (float frameTime);
	void Render (const CCamera &camera);
    
    const glm::mat4 &GetModelMatrix () const { return mModelMatrix; }

private:
	void InitBuffer ();
	void InitShader ();

private:
	GLuint mShaderProgram, mUniform, mVao, mVbo[2];
	GLsizei mIndiceCount;

    glm::mat4 mModelMatrix = glm::mat4 (1.0);

	CWaveSimulator mWaveSimulator;
};
