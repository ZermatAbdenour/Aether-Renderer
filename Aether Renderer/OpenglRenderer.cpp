#include "OpenglRenderer.h"

GLFWwindow* OpenglRenderer::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "AeEngine", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	return window;
}

void OpenglRenderer::Setup()
{

}

void OpenglRenderer::FrameSetup()
{
	glClearColor(1, 0.2, 0.5, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglRenderer::RenderEntity(std::shared_ptr<Entity> entity)
{
		
}

void OpenglRenderer::Clear()
{
}