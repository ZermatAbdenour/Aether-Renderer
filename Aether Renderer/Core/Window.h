#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window {

private:
	int Width = 800, Height = 600;
	const char* Name = "Aether";
public:
	/// <summary>
	/// Create a window with the default settings
	/// </summary>
	Window();
	/// <summary>
	/// create a window with customized width height and name
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="name"></param>
	Window(int width,int height,const char* name);
	/// <summary>
	/// Get the Width and height of the window
	/// </summary>
	/// <returns></returns>
	glm::vec2 GetDimensions();
	/// <summary>
	/// Function that creates a glfw window
	/// </summary>
	bool ShouldClose();
	/// <summary>
	/// Swap the glfw window buffers
	/// </summary>
	void SwapBuffers();
private:
	void CreateWindow();
	GLFWwindow* window;
};