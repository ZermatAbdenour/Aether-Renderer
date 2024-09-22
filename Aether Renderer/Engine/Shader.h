#pragma once
#include <glm/glm.hpp>
#include "../Utilities/FileUtil.hpp"

/// <summary>
/// This Shader class Contain only the source code for the vertex and the fragment shader
/// Each renderer has his one shader
/// </summary>
class Shader {
public:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
public: 
	Shader() = delete;
	~Shader() = default;
	Shader(std::string vertexShader,std::string fragmentShader);
};