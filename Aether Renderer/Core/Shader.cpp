#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexShader, const char* fragmentShader)
{
	//Create and compile the vertex shader
	unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderContent = ReadShaderFromFile(GetShaderPath(vertexShader));
	const char* vertexShaderCode = vertexShaderContent.c_str();
	glShaderSource(vertexID, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexID);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Create and Compile fragment shader
	unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentShaderContent = ReadShaderFromFile(GetShaderPath(fragmentShader));
	const char* fragmentShaderCode = fragmentShaderContent.c_str();
	glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentID);


	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Create Shader Program
	ID = glCreateProgram();
	//Attach the vertex shader to the shader program
	glAttachShader(ID, fragmentID);
	//Attach the vertex shader to the shader program
	glAttachShader(ID, vertexID);
	glLinkProgram(ID);

	// check for linking errors
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::SetInt(const char* name, int value)
{
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetUnsignedInt(const char* name,unsigned int value)
{
	glUniform1ui(glGetUniformLocation(ID, name), value);
}
void Shader::SetFloat(const char* name, float value)
{
	glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::SetVec2(const char* name, glm::vec2 value)
{
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void Shader::SetMat4(const char* name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name),1,GL_FALSE,glm::value_ptr(value));
}


void Shader::Use()
{
	glUseProgram(ID);
}