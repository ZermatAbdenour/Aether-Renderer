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
    glfwSetWindowUserPointer(window, this);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
    glfwSwapInterval(0);//Disable VSync
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        OpenglRenderer* renderer = static_cast<OpenglRenderer*>(glfwGetWindowUserPointer(window));
        if (renderer) {
            renderer->windowWidth = width;
            renderer->windowHeight = height;
        }
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

	PBRShader = CreateShader(Ressources::Shaders::Default);

    //setup FBO
    {
        screenShader = CreateShader(Ressources::Shaders::ScreenShader);
        FBO = CreateFramebuffer();
        screenQuad = CreateMesh(Ressources::Primitives::Quad);
    }
}
void OpenglRenderer::SetupEntity(std::shared_ptr<Entity> entity)
{
	MeshRenderer* meshRenderer = entity->meshRenderer;
    if (!meshRenderer)
        return;

    //Create and add Glmesh
    if (meshRenderer->mesh && !Meshs.contains(meshRenderer->mesh)) {
        auto glmesh = CreateMesh(meshRenderer->mesh);
        Meshs.insert({ meshRenderer->mesh, glmesh });
    }

    //Create and add texture
    if (meshRenderer->image && !Textures.contains(meshRenderer->image)) {
        GLuint texture = CreateTexture(meshRenderer->image);
        Textures.insert({ meshRenderer->image,texture });
    }


}

void OpenglRenderer::SetupFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER,FBO->id);
    glEnable(GL_DEPTH_TEST);
	glClearColor(1, 0.2, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
  
}

void OpenglRenderer::RenderEntity(std::shared_ptr<Entity> entity,Camera camera)
{
    //? This Does not feel Right I want it to access the data directly
    //Get necessary data to render entity
	MeshRenderer* meshRenderer = entity->meshRenderer;
    if (!meshRenderer)
        return;
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    GLuint textureId = GetTexture(meshRenderer->image);

	glUseProgram(PBRShader);
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_model"), 1, GL_FALSE, glm::value_ptr(entity->model));
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_view"), 1, GL_FALSE, glm::value_ptr(camera.View()));
    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "u_projection"), 1, GL_FALSE, glm::value_ptr(camera.Projection(windowWidth,windowHeight)));
    glUniform1f(glGetUniformLocation(PBRShader, "far"), camera.farPlane);
    glUniform1f(glGetUniformLocation(PBRShader, "near"), camera.nearPlane);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glActiveTexture(GL_TEXTURE0);
    
    glUniform1i(glGetUniformLocation(PBRShader, "ourTexture"), 0);

	glBindVertexArray(mesh->vao);

	glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << error << std::endl;
    }
}

void OpenglRenderer::EndFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);

    //Render Frame buffer
    glUseProgram(screenShader);

    glBindTexture(GL_TEXTURE_2D, FBO->colorAttachment);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PBRShader, "colorTexture"), 0);

    glBindVertexArray(screenQuad->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    glGetProgramiv(glShader, GL_LINK_STATUS, &success);

    // Check for linking errors
    if (!success) {
        glGetProgramInfoLog(glShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Cleanup shaders 
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    return glShader;
}

std::shared_ptr<OpenglRenderer::GLFrameBuffer> OpenglRenderer::CreateFramebuffer()
{
    auto fbo = std::make_shared<OpenglRenderer::GLFrameBuffer>();
    glGenFramebuffers(1, &fbo->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
    
    //Color attachement
    Image fboImage = Image();
    fboImage.data = NULL;
    fboImage.Width = windowWidth+1;
    fboImage.Height = windowHeight;
    fboImage.NRChannels = 3;

    fbo->colorAttachment = CreateTexture(&fboImage);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->colorAttachment, 0);

    //DepthStencil render buffer
    glGenRenderbuffers(1, &fbo->depthStencilRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo->depthStencilRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->depthStencilRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
    glEnable(GL_DEPTH_TEST);//enable depth testing on the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}


std::shared_ptr<OpenglRenderer::GLMesh> OpenglRenderer::CreateMesh(Mesh* mesh)
{
    //Mesh Already created 
    if (!mesh)
        return nullptr;
	auto glMesh = std::make_shared<GLMesh>();
	//Generate the Vertex Buffer Object and The Index Buffer Object
	glGenBuffers(1, &glMesh->vbo);
	glGenBuffers(1, &glMesh->ebo);

	//Generate the Vertex array object
	glGenVertexArrays(1, &glMesh->vao);

	glBindVertexArray(glMesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, glMesh->vbo);


	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(Mesh::Vertex), mesh->vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glMesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Mesh::Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Mesh::Vertex,uv));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind the vertex array
	glBindVertexArray(0);

    return glMesh;
}

std::shared_ptr<OpenglRenderer::GLMesh> OpenglRenderer::GetGLMesh(Mesh* mesh)
{
    return Meshs[mesh];
}

GLuint OpenglRenderer::CreateTexture(Image* image)
{
    //Texture Already created 
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping and filtering options
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
   
    return texture;
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return Textures[image];
}