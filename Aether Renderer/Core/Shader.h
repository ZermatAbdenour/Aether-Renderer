#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Utilities/FileUtil.hpp"

class Shader {
public:
	unsigned int ID;
public: 
	Shader() = delete;
	Shader(const char * vertexShader,const char*fragmentShader);
	~Shader();
	/// <summary>
	/// Set an "int" Uniform in the shader
	/// </summary>
	/// <param name="name">: Name of the uniform</param>
	/// <param name="value">: Value you want to set the uniform to</param>
	void SetInt(const char* name, int value);
	/// <summary>
	/// Set an "unsigned int" Uniform in the shader
	/// </summary>
	/// <param name="name">: Name of the uniform</param>
	/// <param name="value">: Value you want to set the uniform to</param>
	void SetUnsignedInt(const char* name, unsigned int value);
	void SetFloat(const char* name, float value);
	/// <summary>
	/// Set a "vec2" Uniform in the shader
	/// </summary>
	/// <param name="name">: Name of the unform</param>
	/// <param name="value">: Value you want to set the uniform to</param>
	void SetVec2(const char* name, glm::vec2 value);
	void SetMat4(const char* name, glm::mat4 value);
	void Use();
};