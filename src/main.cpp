#include "GlobalHeader.h"

#include "Camera.h"
#include "Renderer.h"


int width = 1024, height = 768;
int FIXED_CURSOR_X = width / 2, FIXED_CURSOR_Y = height / 2;

GLFWwindow *gWindow;
CCamera *gCamera;
CRenderer *gRenderer;
bool gPaused = false;


void UpdateCamera (float deltaTime)
{
    double xpos, ypos;
    glfwGetCursorPos(gWindow, &xpos, &ypos);
    glfwSetCursorPos(gWindow, FIXED_CURSOR_X, FIXED_CURSOR_Y);
    
    float offsetH = 0, offsetV = 0;
    
    if (glfwGetKey (gWindow, GLFW_KEY_D) == GLFW_PRESS)
    {
		offsetH = 1;
	}
	else if (glfwGetKey (gWindow, GLFW_KEY_A) == GLFW_PRESS)
    {
		offsetH = -1;
	}
    
    if (glfwGetKey (gWindow, GLFW_KEY_W) == GLFW_PRESS)
    {
		offsetV = 1;
	}
	else if (glfwGetKey (gWindow, GLFW_KEY_S) == GLFW_PRESS)
    {
		offsetV = -1;
	}
    float rotateH = (float)(FIXED_CURSOR_X - xpos);
    float rotateV = (float)(FIXED_CURSOR_Y - ypos);
    
	gCamera->Update (deltaTime, CCamera::SUpdateParams (offsetH, offsetV, rotateH, rotateV, glfwGetKey (gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS));
}


void HandleRenderInput (GLFWwindow *, int key, int , int state, int)
{
	if (state != GLFW_PRESS)
		return;

	switch (key)
	{
	case (GLFW_KEY_P):
	{
		gPaused = !gPaused;
		break;
	}
	case (GLFW_KEY_L):
	{
		gRenderer->DebugSave ("ocean");
		break;
	}
	}
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
	gWindow = window;
    
    // with retina display, width/height might change
    glfwGetFramebufferSize (window, &width, &height);
    FIXED_CURSOR_X = width / 2;
    FIXED_CURSOR_Y = height / 2;
    
	glfwMakeContextCurrent (window);
    glfwSetKeyCallback (window, HandleRenderInput);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, FIXED_CURSOR_X, FIXED_CURSOR_Y);


    glewExperimental = true;
	if (glewInit () != GLEW_OK) return -3;
    

    CCamera camera (Math::Degree2Radians(60.0f), float (width) / float (height), 0.1f, 10000.0f);
	gCamera = &camera;
	CRenderer renderer (width, height, camera);
	gRenderer = &renderer;


    double pausedTime = 0;
    double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose (window) && glfwGetKey (window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;
        
        if (gPaused)
        {
            pausedTime += deltaTime;
        }
       
        UpdateCamera (deltaTime);
        renderer.Draw ((float)(currentTime - pausedTime), gPaused);

		glfwSwapBuffers (window);
		glfwPollEvents ();
	}

	glfwTerminate ();
	return 0;
}
