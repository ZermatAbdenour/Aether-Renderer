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
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window,int width,int height) {
        glViewport(0, 0, width, height);
    });
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}


	return window;
}

void OpenglRenderer::Setup()
{

}

void OpenglRenderer::SetupEntity(std::shared_ptr<Entity> entity)
{
	MeshRenderer entityRenderer = entity->EntityRenderer;

	CreateMesh(entityRenderer.Mesh);
	CreateShader(entityRenderer.Shader);
}

void OpenglRenderer::SetupFrame()
{
	glClearColor(1, 0.2, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglRenderer::RenderEntity(std::shared_ptr<Entity> entity)
{
    //? This Does not feel Right I want it to access the data directly
	MeshRenderer entityRenderer = entity->EntityRenderer;
	glUseProgram(Shaders[entityRenderer.Shader]->id);

	glBindVertexArray(Meshs[entityRenderer.Mesh]->VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenglRenderer::Clear()
{

}
void OpenglRenderer::CreateShader(Shader* shader)
{
    // Create and compile the vertex shader
    unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderCode = shader->vertexShaderSource.c_str(); 
   
    glShaderSource(vertexID, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexID);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderCode = shader->fragmentShaderSource.c_str(); 
    glShaderSource(fragmentID, 1, &fragmentShaderCode, NULL); 
    glCompileShader(fragmentID);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Create Shader Program
    auto glShader = std::make_shared<GLShader>();
    glShader->id = glCreateProgram();

    // Attach shaders to the program
    glAttachShader(glShader->id, vertexID); 
    glAttachShader(glShader->id, fragmentID); 
    glLinkProgram(glShader->id);

    // Check for linking errors
    glGetProgramiv(glShader->id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(glShader->id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Cleanup shaders 
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    // Insert the shader into the map
    Shaders.insert({ shader, glShader });
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
	glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.size() * sizeof(float), mesh->Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(unsigned int), mesh->Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the vertex array
	glBindVertexArray(0);

    // Insert the shader into the map
    Meshs.insert({ mesh, glMesh});
}