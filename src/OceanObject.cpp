#include "GlobalHeader.h"

#include "OceanObject.h"
#include "Camera.h"

#include <cassert>
#include <iostream>
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
    
    glGenTextures(TextureType::TextureMax, mTexture);
    for (int i = 0; i < TextureType::TextureMax; i++)
    {
        glBindTexture(GL_TEXTURE_2D, mTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	std::vector<Math::Vector3> vertexData;
	vertexData.reserve (size);
	std::vector<Math::Vector2> uvData;
	uvData.reserve (size);
    
    float stepu = 1.0f / (float)sizeX;
    float stepv = 1.0f / (float)sizeZ;

	for (int z = 0; z < sizeZ; z++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			vertexData.emplace_back (x - sizeX * 0.5f, 0, z - sizeZ * 0.5f);
            uvData.emplace_back(x * stepu, z * stepv);
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
	glGenBuffers (VertexBufferType::BufferTypeMax, mVbo);
    glBindVertexArray(mVao);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[VertexBufferType::Position]);
	glBufferData (GL_ARRAY_BUFFER, vertexData.size () * sizeof (vertexData[0]), vertexData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (VertexBufferType::Position);
	glVertexAttribPointer (VertexBufferType::Position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    
	glBindBuffer (GL_ARRAY_BUFFER, mVbo[VertexBufferType::UV]);
	glBufferData (GL_ARRAY_BUFFER, uvData.size () * sizeof (uvData[0]), uvData.data (), GL_STATIC_DRAW);
	glEnableVertexAttribArray (VertexBufferType::UV);
	glVertexAttribPointer (VertexBufferType::UV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mVbo[VertexBufferType::Indice]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, indiceData.size () * sizeof (indiceData[0]), indiceData.data (), GL_STATIC_DRAW);
    

    glBindVertexArray(0);
}


extern const char *vsSrc;
extern const char *psSrc;
void COceanObject::InitShader ()
{
	GLint result = GL_FALSE;
    GLint bufflen = 0;
    

	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vsSrc, NULL);
	glCompileShader (vs);
	glGetShaderiv (vs, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &bufflen);
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(vs, bufflen, 0, log_string);
        std::cout << log_string << std::endl;
        delete []log_string;
        assert (false);
    }


	GLuint ps = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (ps, 1, &psSrc, NULL);
	glCompileShader (ps);
	glGetShaderiv (ps, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &bufflen);
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(ps, bufflen, 0, log_string);
        std::cout << log_string << std::endl;
        delete []log_string;
        assert (false);
    }


	mShaderProgram = glCreateProgram ();
	glAttachShader (mShaderProgram, vs);
    if (glGetError () != GL_NO_ERROR)
    {
        glGetShaderiv(mShaderProgram, GL_INFO_LOG_LENGTH, &bufflen);
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(mShaderProgram, bufflen, 0, log_string);
        std::cout << log_string << std::endl;
        delete []log_string;
        assert (false);
    }
	glAttachShader (mShaderProgram, ps);
    if (glGetError () != GL_NO_ERROR)
    {
        glGetShaderiv(mShaderProgram, GL_INFO_LOG_LENGTH, &bufflen);
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(mShaderProgram, bufflen, 0, log_string);
        std::cout << log_string << std::endl;
        delete []log_string;
        assert (false);
    }
	glLinkProgram (mShaderProgram);
	glGetShaderiv (mShaderProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        glGetShaderiv(mShaderProgram, GL_INFO_LOG_LENGTH, &bufflen);
        GLchar* log_string = new char[bufflen + 1];
        glGetShaderInfoLog(mShaderProgram, bufflen, 0, log_string);
        std::cout << log_string << std::endl;
        delete []log_string;
        assert (false);
    }
    
    
    mUniform[UniformType::DisplacementTexture] = glGetUniformLocation(mShaderProgram, "DisplacementTexture");
    mUniform[UniformType::NormalTexture] = glGetUniformLocation(mShaderProgram, "NormalTexture");
    mUniform[UniformType::Mvp] = glGetUniformLocation(mShaderProgram, "mvp");
    mUniform[UniformType::Mv] = glGetUniformLocation(mShaderProgram, "mv");
    mUniform[UniformType::Mvn] = glGetUniformLocation(mShaderProgram, "mvn");


	glDetachShader (mShaderProgram, vs);
	glDetachShader (mShaderProgram, ps);
	glDeleteShader (vs);
	glDeleteShader (ps);
}


COceanObject::~COceanObject ()
{
	glDeleteBuffers (VertexBufferType::BufferTypeMax, mVbo);
    glDeleteVertexArrays(1, &mVao);
	glDeleteProgram (mShaderProgram);
	glDeleteTextures(TextureType::TextureMax, mTexture);
}


void COceanObject::Update (float currentTime)
{
    mWaveSimulator.Update (currentTime * 0.025);
    
    glBindTexture(GL_TEXTURE_2D, mTexture[TextureType::Displacement]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaveSimulator.GetDataSize (), mWaveSimulator.GetDataSize (), 0, GL_RGB, GL_FLOAT, mWaveSimulator.GetDisplacementData ());
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, mTexture[TextureType::Normal]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWaveSimulator.GetDataSize (), mWaveSimulator.GetDataSize (), 0, GL_RGB, GL_FLOAT, mWaveSimulator.GetNormalData ());
    glBindTexture(GL_TEXTURE_2D, 0);
}


void COceanObject::Render (const CCamera &camera)
{
	glUseProgram (mShaderProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture[TextureType::Displacement]);
    glUniform1i(mUniform[UniformType::DisplacementTexture], 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mTexture[TextureType::Normal]);
    glUniform1i(mUniform[UniformType::NormalTexture], 1);
    
    Math::Matrix4 mvp = camera.GetProjectionMatrix () * camera.GetViewMatrix ();
    glUniformMatrix4fv(mUniform[UniformType::Mvp], 1, GL_FALSE, &mvp[0][0]);
    
    glUniformMatrix4fv(mUniform[UniformType::Mv], 1, GL_FALSE, &camera.GetViewMatrix ()[0][0]);
    
    glUniformMatrix4fv(mUniform[UniformType::Mvn], 1, GL_FALSE, &camera.GetViewNormalMatrix ()[0][0]);

    glBindVertexArray(mVao);
	glDrawElements (GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


void COceanObject::DebugSave (const char *path)
{
    mWaveSimulator.DebugSave (path);
}
