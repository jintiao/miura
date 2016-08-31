#include "OceanObject.h"
#include "GLHeader.h"

#include <cassert>
#include <string>
#include <vector>


struct OceanVertex
{
	float x, y, z;
};


COceanObject::COceanObject ()
{
	InitBuffer ();
	InitShader ();
}


void COceanObject::InitBuffer ()
{
	GLsizei sizeX = 10, sizeZ = 10, size = sizeX * sizeZ; // sizeX * sizeZ (meters)
	float scaleX = 1.0f, scaleZ = 1.0f;

	std::vector<OceanVertex> vertexData;
	vertexData.reserve (size);
	for (GLsizei z = 0; z < sizeZ; z++)
	{
		for (GLsizei x = 0; x < sizeX; x++)
		{
			vertexData.emplace_back (OceanVertex{ scaleX * x / sizeX, 0.0f, scaleZ * z / sizeZ });
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

	glGenBuffers (1, &mVertexbuffer);
	glBindBuffer (GL_ARRAY_BUFFER, mVertexbuffer);
	glBufferData (GL_ARRAY_BUFFER, vertexData.size () * sizeof (decltype (vertexData)::value_type), vertexData.data (), GL_STATIC_DRAW);

	glGenBuffers (1, &mIndicebuffer);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mIndicebuffer);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indiceData.size () * sizeof (decltype (indiceData)::value_type), indiceData.data (), GL_STATIC_DRAW);
}


void COceanObject::InitShader ()
{
	GLint result = GL_FALSE;

	char const *vsSrc = "\n"
		"#version 330 core \n"
		"layout(location = 0) in vec3 vertexPosition_modelspace; \n"
		"void main(){ \n"
		"    gl_Position.xyz = vertexPosition_modelspace.xzy; \n"
		"    gl_Position.w = 1.0; \n"
		"} \n";

	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vsSrc, NULL);
	glCompileShader (vs);
	glGetShaderiv (vs, GL_COMPILE_STATUS, &result);
	assert (result != GL_FALSE);

	char const *psSrc = "\n"
		"#version 330 core \n";
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

	glDetachShader (mShaderProgram, vs);
	glDetachShader (mShaderProgram, ps);

	glDeleteShader (vs);
	glDeleteShader (ps);
}


COceanObject::~COceanObject ()
{
	glDeleteBuffers (1, &mVertexbuffer);
	glDeleteBuffers (1, &mIndicebuffer);
	glDeleteProgram (mShaderProgram);
}


void COceanObject::Render ()
{
	//glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram (mShaderProgram);

	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, mVertexbuffer);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mIndicebuffer);
	glDrawElements (GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_SHORT, nullptr);

	glDisableVertexAttribArray (0);
}
