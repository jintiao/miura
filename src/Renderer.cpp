#include "Renderer.h"
#include "GLHeader.h"


CRenderer::CRenderer ()
{
	GLuint VertexArrayID;
	glGenVertexArrays (1, &VertexArrayID);
	glBindVertexArray (VertexArrayID);

	glClearColor (0.86f, 0.86f, 0.86f, 0.0f);
}


void CRenderer::Render ()
{
	glClear (GL_COLOR_BUFFER_BIT);
	mOcean.Render ();
}
