#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"

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

void OpenglRenderer::Setup(Scene* scene)
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

void OpenglRenderer::CreateShader(Shader* shader)
{
	//Create and compile the vertex shader
	unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCode = shader->vertexShaderSource;
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
	const char* fragmentShaderCode = shader->fragmentShaderSource;
	glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentID);


	glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	auto glShader = std::make_shared<GLShader>();
	//Create Shader Program
	glShader->id = glCreateProgram();
	//Attach the vertex shader to the shader program
	glAttachShader(glShader->id, fragmentID);
	//Attach the vertex shader to the shader program
	glAttachShader(glShader->id, vertexID);
	glLinkProgram(glShader->id);

	// check for linking errors
	glGetProgramiv(glShader->id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(glShader->id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	//insert the Shader to the map
	Shaders.insert({shader,glShader});
}

void OpenglRenderer::CreateMesh(Mesh* mesh)
{
	auto glMesh = std::make_shared<GLMesh>();
	//Generate the Vertex Buffer Object and The Index Buffer Object
	glGenBuffers(1, &glMesh->VBO);
	glGenBuffers(1, &glMesh->EBO);

	//Generate the Vertex array object
	glGenVertexArrays(1, &glMesh->VAO);

	glBindVertexArray(glMesh->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, glMesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->Vertices), mesh->Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->Indices), mesh->Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//unbind the vertex array
	glBindVertexArray(0);
}