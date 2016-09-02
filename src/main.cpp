#include "Camera.h"
#include "GLHeader.h"
#include "Renderer.h"


int width = 1024, height = 768;
int FIXED_CURSOR_X = width / 2, FIXED_CURSOR_Y = height / 2;


void UpdateCamera (GLFWwindow *window, CCamera &camera, float deltaTime)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, FIXED_CURSOR_X, FIXED_CURSOR_Y);
    
    float offsetH = 0, offsetV = 0;
    
    if (glfwGetKey (window, GLFW_KEY_D) == GLFW_PRESS){
		offsetH = 1;
	}
	else if (glfwGetKey (window, GLFW_KEY_A) == GLFW_PRESS){
		offsetH = -1;
	}
    
    if (glfwGetKey (window, GLFW_KEY_W) == GLFW_PRESS){
		offsetV = 1;
	}
	else if (glfwGetKey (window, GLFW_KEY_S) == GLFW_PRESS){
		offsetV = -1;
	}
    float rotateH = (float)(FIXED_CURSOR_X - xpos);
    float rotateV = (float)(FIXED_CURSOR_Y - ypos);
    
	camera.Update (deltaTime, CCamera::SUpdateParams (offsetH, offsetV, rotateH, rotateV, glfwGetKey (window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS));
}


int main ()
{
	if (!glfwInit ()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow *window = glfwCreateWindow (width, height, "miura", nullptr, nullptr);
	if (!window) return -2;
    
    // with retina display, width/height might change
    glfwGetFramebufferSize (window, &width, &height);
    FIXED_CURSOR_X = width / 2;
    FIXED_CURSOR_Y = height / 2;
    
	glfwMakeContextCurrent (window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, FIXED_CURSOR_X, FIXED_CURSOR_Y);


    glewExperimental = true;
	if (glewInit () != GLEW_OK) return -3;
    

    CCamera camera (glm::radians(60.0f), float (width) / float (height), 0.1f, 1000.0f);
	CRenderer renderer (width, height, camera);
	return 0;

    auto lastTime = glfwGetTime();
	while (!glfwWindowShouldClose (window) && glfwGetKey (window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
        
        UpdateCamera (window, camera, deltaTime);
		renderer.Update ((float)currentTime);

		glfwSwapBuffers (window);
		glfwPollEvents ();
	}

	glfwTerminate ();
	return 0;
}
