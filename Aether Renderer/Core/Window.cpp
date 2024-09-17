#include "Window.h"
#include <iostream>

Window::Window()
{
	CreateWindow();
}

Window::Window(int width, int height, const char* name)
{
	Width = width;
	Height = height;
	Name = name;
	CreateWindow();
}

glm::vec2 Window::GetDimensions()
{
	return glm::vec2(Width,Height);
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}



void Window::CreateWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(Width, Height, Name, NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
}