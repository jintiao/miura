#include "GlobalHeader.h"

#include "Renderer.h"
#include "Camera.h"


CRenderer::CRenderer (int width, int height, const CCamera &cam) : mCamera (cam)
{
	glClearColor (0.86f, 0.86f, 0.86f, 0.0f);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
}


void CRenderer::Update (float currentTime)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mOcean.Update (currentTime);
	mOcean.Render (mCamera);
}
