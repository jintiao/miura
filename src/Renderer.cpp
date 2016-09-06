#include "GlobalHeader.h"

#include "Renderer.h"
#include "Camera.h"


CRenderer::CRenderer (int width, int height, const CCamera &cam) : mCamera (cam)
{
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}


void CRenderer::Draw (float currentTime, bool noUpdate)
{
    if (!noUpdate)
    {
        mOcean.Update (currentTime);
    }
    
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mOcean.Render (mCamera);
}


void CRenderer::DebugSave (const char *path)
{
    mOcean.DebugSave (path);
}
