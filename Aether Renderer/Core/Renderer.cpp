#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

Renderer::Renderer()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
}

void Renderer::Clear()
{
	glClearColor(1, 0.2, 0.4, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Render(Scene* scene)
{
	
}