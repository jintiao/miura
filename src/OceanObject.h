#pragma once

#include "GLHeader.h"
#include "WaveSimulator.h"

class CCamera;

class COceanObject
{
public:
	COceanObject ();
	~COceanObject ();

	void Update (float currentTime);
	void Render (const CCamera &camera);
    
    const glm::mat4 &GetModelMatrix () const { return mModelMatrix; }

private:
	void InitBuffer ();
	void InitShader ();

private:
	GLuint mShaderProgram, mUniform[2], mVao, mVbo[4], mHeightTexture;
	GLsizei mIndiceCount;

    glm::mat4 mModelMatrix = glm::mat4 (1.0);

	CWaveSimulator mWaveSimulator;
    
    std::vector<float> mHeightMap;
};
