#include "Game.h"
#include "GLHelper.h"


void CGame::Init ()
{
	glClearColor (0.0f, 0.0f, 0.4f, 0.0f);
}


void CGame::Release ()
{
}


void CGame::Update ()
{
	glClear (GL_COLOR_BUFFER_BIT);
}
