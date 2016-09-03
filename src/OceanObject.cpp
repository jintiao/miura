#include "GlobalHeader.h"

#include "OceanObject.h"
#include "OceanShader.h"
#include "Camera.h"

#include <cassert>
#include <random>
#include <string>
#include <vector>


COceanObject::COceanObject ()
{
	InitBuffer ();
	InitShader ();
}


void COceanObject::InitBuffer ()
{
	int sizeX = 512, sizeZ = 512, size = sizeX * sizeZ; // sizeX * sizeZ (meters)
    

    mHeightMap.resize (mWaveSimulator.GetMapSize () * mWaveSimulator.GetMapSize () * 3);
    glGenTextures(1, &mHeightTexture);
    glBindTexture(GL_TEXTURE_2D, mHeightTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaveSimulator.GetMapSize (), mWaveSimulator.GetMapSize (), 0, GL_RGB, GL_FLOAT, &mHeightMap[0]);
    glBindTexture(GL_TEXTURE_2D, 0);


	std::vector<Math::Vector3> vertexData;
	vertexData.reserve (size);
	std::vector<Math::Vector2> uvData;
	uvData.reserve (size);
    
    float stepu = 1.0f / (float)sizeX;
    float stepv = 1.0f / (float)size;

	for (int z = 0; z < sizeZ; z++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			vertexData.emplace_back (x - sizeX * 0.5f, 0, z - sizeZ * 0.5f);
            uvData.emplace_back(x * stepu, z * stepv);
		}
	}


	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> distribution (0.0f, 1.0f);
	static auto roll = std::bind (distribution, generator);
    
	std::vector<Math::Vector3> colorData;
	colorData.reserve (size);
    for (int z = 0; z < sizeZ; z++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			colorData.emplace_back (roll (), roll (), roll ());
		}
	}

	std::vector<unsigned int> indiceData;
	indiceData.reserve (size * 6);
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
	glGenBuffers (4, mVbo);
    glBindVertexArray(mVao);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[0]);
	glBufferData (GL_ARRAY_BUFFER, vertexData.size () * sizeof (vertexData[0]), vertexData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[3]);
	glBufferData (GL_ARRAY_BUFFER, uvData.size () * sizeof (uvData[0]), uvData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[2]);
	glBufferData (GL_ARRAY_BUFFER, colorData.size () * sizeof (decltype (colorData)::value_type), colorData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (2);
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mVbo[1]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indiceData.size () * sizeof (indiceData[0]), indiceData.data (), GL_STATIC_DRAW);
    

    glBindVertexArray(0);
}


void COceanObject::InitShader ()
{
	GLint result = GL_FALSE;


	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vsSrc, NULL);
	glCompileShader (vs);
    
    GLint bufflen = 0;
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &bufflen);
    if (bufflen > 1)
    {
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(vs, bufflen, 0, log_string);
        delete []log_string;
    }
	glGetShaderiv (vs, GL_COMPILE_STATUS, &result);
	assert (result != GL_FALSE);


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
    
    mUniform[0] = glGetUniformLocation(mShaderProgram, "MVP");
    mUniform[1] = glGetUniformLocation(mShaderProgram, "heightMap");

	glDetachShader (mShaderProgram, vs);
	glDetachShader (mShaderProgram, ps);

	glDeleteShader (vs);
	glDeleteShader (ps);
}


COceanObject::~COceanObject ()
{
	glDeleteBuffers (4, mVbo);
    glDeleteVertexArrays(1, &mVao);
	glDeleteProgram (mShaderProgram);
	glDeleteTextures(1, &mHeightTexture);
}


void COceanObject::Update (float currentTime)
{
    mWaveSimulator.Update (currentTime * 0.1);
    auto &hmap = mWaveSimulator.GetHeightMap ();
    
    for (int i = 0; i < hmap.size (); i++)
    {
        mHeightMap[i * 3] = hmap[i];
        mHeightMap[i * 3 + 1] = hmap[i];
        mHeightMap[i * 3 + 2] = hmap[i];
    }

    glBindTexture(GL_TEXTURE_2D, mHeightTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaveSimulator.GetMapSize (), mWaveSimulator.GetMapSize (), 0, GL_RGB, GL_FLOAT, &mHeightMap[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void COceanObject::Render (const CCamera &camera)
{
	glUseProgram (mShaderProgram);
    
    Math::Matrix4 mvp = camera.GetProjectionMatrix () * camera.GetViewMatrix () * mModelMatrix;
    glUniformMatrix4fv(mUniform[0], 1, GL_FALSE, &mvp[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mHeightTexture);
    glUniform1i(mUniform[1], 0);

    glBindVertexArray(mVao);
    //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements (GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
