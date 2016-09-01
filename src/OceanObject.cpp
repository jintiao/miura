#include "OceanObject.h"
#include "Camera.h"
#include "GLHeader.h"

#include <cassert>
#include <string>
#include <vector>


struct OceanVertex
{
	float x, y, z;
};


COceanObject::COceanObject () : mWaveSimulator (0, 15.0f, 200.0f)
{
	InitBuffer ();
	InitShader ();
}


void COceanObject::InitBuffer ()
{
	int sizeX = 100, sizeZ = 100, size = sizeX * sizeZ; // sizeX * sizeZ (meters)

	std::vector<OceanVertex> vertexData;
	vertexData.reserve (size);
	for (int z = 0; z < sizeZ; z++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			vertexData.emplace_back (OceanVertex { 2.0f * x - sizeX, 0.0f, 2.0f * z - sizeZ });
		}
	}

	std::vector<unsigned short> indiceData;
	indiceData.reserve (size * 3);
	for (GLsizei z = 0; z < sizeZ - 1; z++)
	{
		for (GLsizei x = 0; x < sizeX - 1; x++)
		{
			indiceData.emplace_back (sizeX * z + x);
			indiceData.emplace_back (sizeX * z + x + 1);
			indiceData.emplace_back (sizeX * (z + 1) + x);

			indiceData.emplace_back (sizeX * (z + 1) + x);
			indiceData.emplace_back (sizeX * z + x + 1);
			indiceData.emplace_back (sizeX * (z + 1) + x + 1);
		}
	}
	mIndiceCount = static_cast<decltype (mIndiceCount)> (indiceData.size ());
    
    
	glGenVertexArrays (1, &mVao);
	glGenBuffers (2, mVbo);
    glBindVertexArray(mVao);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[0]);
	glBufferData (GL_ARRAY_BUFFER, vertexData.size () * sizeof (decltype (vertexData)::value_type), vertexData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mVbo[1]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indiceData.size () * sizeof (decltype (indiceData)::value_type), indiceData.data (), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}


void COceanObject::InitShader ()
{
	GLint result = GL_FALSE;

	char const *vsSrc = "\n"
		"#version 330 core \n"
		"layout(location = 0) in vec3 vertexPosition_modelspace; \n"
        "uniform mat4 MVP;"
		"void main(){ \n"
		"    gl_Position = MVP * vec4(vertexPosition_modelspace,1); \n"
		"} \n";

	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vsSrc, NULL);
	glCompileShader (vs);
	glGetShaderiv (vs, GL_COMPILE_STATUS, &result);
	assert (result != GL_FALSE);

	char const *psSrc = "\n"
		"#version 330 core \n"
		"out vec3 color; \n"
		"void main(){ \n"
		"    color = vec3(1,0,1); \n"
		"} \n";

	GLuint ps = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (ps, 1, &psSrc, NULL);
	glCompileShader (ps);
	glGetShaderiv (ps, GL_COMPILE_STATUS, &result);
	assert (result != GL_FALSE);

	mShaderProgram = glCreateProgram ();
	glAttachShader (mShaderProgram, vs);
	glAttachShader (mShaderProgram, ps);
	glLinkProgram (mShaderProgram);
	glGetShaderiv (mShaderProgram, GL_LINK_STATUS, &result);
	assert (result != GL_FALSE);
    
    mUniform = glGetUniformLocation(mShaderProgram, "MVP");

	glDetachShader (mShaderProgram, vs);
	glDetachShader (mShaderProgram, ps);

	glDeleteShader (vs);
	glDeleteShader (ps);
}


COceanObject::~COceanObject ()
{
	glDeleteBuffers (2, mVbo);
    glDeleteVertexArrays(1, &mVao);
	glDeleteProgram (mShaderProgram);
}


void COceanObject::Update (float currentTime)
{
    mWaveSimulator.Update (currentTime);
}


void COceanObject::Render (const CCamera &camera)
{
	glUseProgram (mShaderProgram);
    
    glm::mat4 mvp = camera.GetProjectionMatrix () * camera.GetViewMatrix () * mModelMatrix;
    glUniformMatrix4fv(mUniform, 1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(mVao);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements (GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);
}
