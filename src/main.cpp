#include "GLHelper.h"
#include "Game.h"


int main ()
{
	if (!glfwInit ())
		return -1;

	GLFWwindow *window = glfwCreateWindow (1024, 768, "miura", NULL, NULL);
	if (!window)
	{
		glfwTerminate ();
		return -1;
	}

	glfwMakeContextCurrent (window);

	GLenum err = glewInit ();
	if (GLEW_OK != err)
	{
		glfwTerminate ();
		return -1;
	}

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
