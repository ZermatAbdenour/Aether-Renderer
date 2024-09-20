#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"
#include <glm/gtc/type_ptr.hpp>

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

void OpenglRenderer::RenderEntity(std::shared_ptr<Entity> entity,Scene::Camera camera)
{
    //? This Does not feel Right I want it to access the data directly
    //Get the shaderId and the GLMesh from the unorderedmaps
	MeshRenderer entityRenderer = entity->EntityRenderer;
    GLuint shaderId = Shaders[entityRenderer.Shader];
    std::shared_ptr<GLMesh> mesh = Meshs[entityRenderer.Mesh];

	glUseProgram(shaderId);
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "u_model"), 1, GL_FALSE, glm::value_ptr(entity->Model));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "u_view"), 1, GL_FALSE, glm::value_ptr(camera.view()));
    glUniformMatrix4fv(glGetUniformLocation(shaderId, "u_projection"), 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix));

	glBindVertexArray(mesh->vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenglRenderer::Clear()
{

}
void OpenglRenderer::CreateShader(Shader* shader)
{
    //Shader Already created 
    if (Shaders.contains(shader))
        return;
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
    GLuint glShader = glCreateProgram();

    // Attach shaders to the program
    glAttachShader(glShader, vertexID); 
    glAttachShader(glShader, fragmentID); 
    glLinkProgram(glShader);

    // Check for linking errors
    glGetProgramiv(glShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(glShader, 512, NULL, infoLog);
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
    //Mesh Already created 
    if (Meshs.contains(mesh))
        return;
	auto glMesh = std::make_shared<GLMesh>();
	//Generate the Vertex Buffer Object and The Index Buffer Object
	glGenBuffers(1, &glMesh->vbo);
	glGenBuffers(1, &glMesh->ebo);

	//Generate the Vertex array object
	glGenVertexArrays(1, &glMesh->vao);

	glBindVertexArray(glMesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, glMesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->Vertices.size() * sizeof(float), mesh->Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(unsigned int), mesh->Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the vertex array
	glBindVertexArray(0);

    // Insert the shader into the map
    Meshs.insert({ mesh, glMesh});
}