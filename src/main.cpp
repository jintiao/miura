#include "GLHeader.h"
#include "Game.h"


int main ()
{
	if (!glfwInit ())
		return -1;

	GLFWwindow *window = glfwCreateWindow (1024, 768, "miura", NULL, NULL);
	if (!window) return -1;

	glfwMakeContextCurrent (window);

    
    GLenum code = glewInit ();
	if (code != GLEW_OK) return -2;

	CGame game;
	game.Init ();

	while (!glfwWindowShouldClose (window))
	{
		game.Update ();
		glfwSwapBuffers (window);
		glfwPollEvents ();
	}

	game.Release ();

	glfwTerminate ();
	return 0;
}
