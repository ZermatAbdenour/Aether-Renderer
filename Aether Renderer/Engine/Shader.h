#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Utilities/FileUtil.hpp"

/// <summary>
/// This Shader class Contain only the source code for the vertex and the fragment shader
/// Each renderer has his one shader
/// </summary>
class Shader {
public:
	const char* vertexShaderSource;
	const char* fragmentShaderSource;
public: 
	Shader() = delete;
	~Shader() = default;
	Shader(const char * vertexShader,const char*fragmentShader);
};