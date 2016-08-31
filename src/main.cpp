#include "GLHeader.h"
#include "Renderer.h"


int main ()
{
	if (!glfwInit ())
		return -1;

	glfwWindowHint (GLFW_SAMPLES, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow (1024, 768, "miura", NULL, NULL);
	if (!window) return -1;

	glfwMakeContextCurrent (window);

	if (glewInit () != GLEW_OK) return -2;

	CRenderer renderer;

	while (!glfwWindowShouldClose (window))
	{
		renderer.Render ();
		glfwSwapBuffers (window);
		glfwPollEvents ();
	}

	glfwTerminate ();
	return 0;
}
