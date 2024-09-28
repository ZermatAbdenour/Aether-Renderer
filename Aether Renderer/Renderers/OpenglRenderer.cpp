#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../Engine/Ressources.h"

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
        OpenglRenderer* renderer = static_cast<OpenglRenderer*>(glfwGetWindowUserPointer(window));
        if (!renderer)
            return;
        renderer->UpdateFrameBuffer(renderer->FBO,width,height);
        });

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

    glViewport(0, 0, windowWidth, windowHeight);

	return window;
}

void OpenglRenderer::Setup()
{

	PBRShader = CreateShader(Ressources::Shaders::Default);

    //Setup FBO and the Full screen quad
    {
        screenShader = CreateShader(Ressources::Shaders::ScreenShader);
        FBO = CreateFramebuffer();
        screenQuad = CreateMesh(Ressources::Primitives::Quad);
    }
    //Setup UBOs
    {
        glGenBuffers(1, &matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
    }
}
void OpenglRenderer::SetupScene(Scene* scene)
{
    //Setup each entity
    Renderer::SetupScene(scene);
    //Setup skybox
    {
        SkyBoxShader = CreateShader(Ressources::Shaders::Skybox);
        SkyboxMesh = CreateMesh(Ressources::Primitives::Cube);

        SkyBoxMap = CreateCubeMap(std::vector<std::string>{
            "skybox/right.jpg",
                "skybox/left.jpg",
                "skybox/top.jpg",
                "skybox/bottom.jpg",
                "skybox/front.jpg",
                "skybox/back.jpg"
        });
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
        GLuint texture = CreateTexture(GL_TEXTURE_2D);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->image);
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

    //Set the "Camera" UBO sub data 
    {
        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_currentScene->camera.Projection(windowWidth, windowHeight)));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_currentScene->camera.View()));
        glBufferSubData(GL_UNIFORM_BUFFER,2* sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(m_currentScene->camera.position));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    //Render SkyBox
    {
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glUseProgram(SkyBoxShader);
        glBindVertexArray(SkyboxMesh->vao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, SkyBoxMap);
	    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }
}

void OpenglRenderer::RenderEntity(MeshRenderer* meshRenderer, glm::mat4 model,Camera camera)
{
    //Get necessary data to render 
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    GLuint textureId = GetTexture(meshRenderer->image);
    
	glUseProgram(PBRShader);

    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "model"),1,false, glm::value_ptr(model));

    if (meshRenderer->image) {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glActiveTexture(GL_TEXTURE0);
    }
    
	glBindVertexArray(mesh->vao);

	glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D,0);
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
    fboImage.Width = windowWidth;
    fboImage.Height = windowHeight;
    fboImage.NRChannels = 3;

    fbo->colorAttachment = CreateTexture(GL_TEXTURE_2D);
    SetTextureData(GL_TEXTURE_2D, &fboImage);
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

void OpenglRenderer::UpdateFrameBuffer(std::shared_ptr<OpenglRenderer::GLFrameBuffer> frameBuffer, int width, int height)
{
    glViewport(0, 0, width, height);
    glBindTexture(GL_TEXTURE_2D, frameBuffer->colorAttachment);
    Image fboImage = Image();
    fboImage.data = NULL;
    fboImage.Width = width;
    fboImage.Height = height;
    fboImage.NRChannels = 3;
    SetTextureData(GL_TEXTURE_2D, &fboImage);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer->depthStencilRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
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

GLuint OpenglRenderer::CreateTexture(GLenum type)
{
    //Texture Already created 
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(type, texture);
    // set the texture wrapping and filtering options
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    

    glGenerateMipmap(type);

    return texture;
}

void OpenglRenderer::SetTextureData(GLenum target,Image* image)
{
    //!this function does not bind the texture
    GLenum format;
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;
    case 2: format = GL_RG;
    case 3: format = GL_RGB;
    case 4: format = GL_RGBA;
    default: format = GL_RGB;
    }

    glTexImage2D(target, 0, format, image->Width, image->Height, 0, format, GL_UNSIGNED_BYTE, image->data);
    //glGenerateMipmap(target);
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return Textures[image];
}

GLuint OpenglRenderer::CreateCubeMap(std::vector<std::string> faces)
{
    GLuint cubeMap = CreateTexture(GL_TEXTURE_CUBE_MAP);
    for (int i = 0;i < 6;i++) {
        Image* faceImage = Ressources::LoadImageFromFile(faces[i],false);
        SetTextureData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, faceImage);

        delete faceImage;
    }
    return cubeMap;
}