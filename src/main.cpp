#include <GLFW/glfw3.h>

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

	glGetString (GL_VERSION);

	while (!glfwWindowShouldClose (window))
	{
		glClear (GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers (window);
		glfwPollEvents ();
	}

	glfwTerminate ();
	return 0;
}
