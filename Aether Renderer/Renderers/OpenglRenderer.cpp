#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"
#include <glm/gtc/type_ptr.hpp>s

GLFWwindow* OpenglRenderer::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "AeEngine", NULL, NULL);

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
	MeshRenderer entityRenderer = entity->entityRenderer;

	CreateMesh(entityRenderer.mesh);
	PBRShader = CreateShader(Ressources::Shaders::Default);
    if (entityRenderer.image)
        CreateTexture(entityRenderer.image);


}

void OpenglRenderer::SetupFrame()
{
	glClearColor(1, 0.2, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglRenderer::RenderEntity(std::shared_ptr<Entity> entity,Scene::Camera camera)
{
    //? This Does not feel Right I want it to access the data directly
    //Get necessary data to render entity
	MeshRenderer entityRenderer = entity->entityRenderer;
    std::shared_ptr<GLMesh> mesh = GetGLMesh(entityRenderer.mesh);
    GLuint textureId = GetTexture(entityRenderer.image);

	glUseProgram(PBRShader);
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_model"), 1, GL_FALSE, glm::value_ptr(entity->model));
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_view"), 1, GL_FALSE, glm::value_ptr(camera.view()));
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_projection"), 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix));
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(PBRShader, "ourTexture"), 0);

	glBindVertexArray(mesh->vao);

	glDrawElements(GL_TRIANGLES, entityRenderer.mesh->Indices.size(), GL_UNSIGNED_INT, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }
}

void OpenglRenderer::Clear()
{

}
GLuint OpenglRenderer::CreateShader(Shader* shader)
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

    return glShader;
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

    std::vector<float> vertexData = mesh->GetVertexData();
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(unsigned int), mesh->Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the vertex array
	glBindVertexArray(0);

    // Insert the shader into the map
    Meshs.insert({ mesh, glMesh});
}

std::shared_ptr<OpenglRenderer::GLMesh> OpenglRenderer::GetGLMesh(Mesh* mesh)
{
    return Meshs[mesh];
}

void OpenglRenderer::CreateTexture(Image* image)
{
    //Shader Already created 
    if (Textures.contains(image))
        return;
    if (!image) {
        std::cout << "Failed to load texture" << std::endl;
        return;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping and filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format;
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;
    case 2: format = GL_RG;
    case 3: format = GL_RGB;
    case 4: format = GL_RGBA;
    default: format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, image->Width, image->Height, 0, format, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(GL_TEXTURE_2D);
   
    Textures.insert({ image,texture });
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return Textures[image];
}
