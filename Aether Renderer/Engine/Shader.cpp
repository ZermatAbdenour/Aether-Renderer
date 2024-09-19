#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(std::string vertexShader, std::string fragmentShader)
{
	//fragment shader source
	std::string fragmentShaderContent = ReadShaderFromFile(GetShaderPath(fragmentShader));
	fragmentShaderSource = fragmentShaderContent;
	//vertex shader source
	std::string vertexShaderContent = ReadShaderFromFile(GetShaderPath(vertexShader));
	vertexShaderSource = vertexShaderContent;
}