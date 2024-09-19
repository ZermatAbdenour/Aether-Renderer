#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
	//vertex shader source
	std::string vertexShaderContent = ReadShaderFromFile(GetShaderPath(vertexShader));
	vertexShaderSource = vertexShaderContent.c_str();

	//fragment shader source
	unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentShaderContent = ReadShaderFromFile(GetShaderPath(fragmentShader));
	fragmentShaderSource = fragmentShaderContent.c_str();
}