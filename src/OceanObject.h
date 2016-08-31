#pragma once

#include "GLHeader.h"
#include "WaveSimulator.h"


class COceanObject
{
public:
	COceanObject ();
	~COceanObject ();

	void Render ();

private:
	void InitBuffer ();
	void InitShader ();

private:
	GLuint mVertexbuffer;
	GLuint mIndicebuffer;
	GLsizei mIndiceCount;
	GLuint mShaderProgram;

	CWaveSimulator mWaveSimulator;
};
