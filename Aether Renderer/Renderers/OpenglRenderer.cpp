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
        renderer->FrameBufferResizeCallBack(width,height);
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

    //Setup FBO and the Full screen quad
    { 
        screenShader = CreateShader(Ressources::Shaders::ScreenShader);

        screenFBO = CreateFrameBuffer(true,4);//add depth stencil attachement with 4 samples
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

    //Set the "Lights" SSBO
    {
        //Genereate GLLights
        int numDirectionalLights = scene->DirectionalLights.size();
        GLDirectionalLight directionalLights[MAX_DIRECTIONALLIGHTS];
        for (int i = 0;i < numDirectionalLights;i++) {
            directionalLights[i] = GLDirectionalLight(scene->DirectionalLights[i]);
        }

        int numPointLights = scene->PointLights.size();
        GLPointLight pointLights[MAX_POINTLIGHTS];
        for (int i = 0;i < numPointLights;i++) {
            pointLights[i] =  GLPointLight(scene->PointLights[i]);
        }

        glGenBuffers(1, &lightsSSBO);
        glBindBuffer(GL_UNIFORM_BUFFER, lightsSSBO);
        glBufferData(GL_UNIFORM_BUFFER, 16 + sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS + sizeof(GLPointLight) * MAX_POINTLIGHTS, nullptr, GL_DYNAMIC_DRAW);

        int offset = 0;
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &numDirectionalLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &numPointLights);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset,sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS, &directionalLights[0]);
        offset += sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLPointLight) * MAX_POINTLIGHTS, &pointLights[0]);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsSSBO);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

    //Create and add textures
    if (meshRenderer->diffuse && !Textures.contains(meshRenderer->diffuse)) {
        GLuint diffuseTexture = CreateTexture(GL_TEXTURE_2D,GL_LINEAR,GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->diffuse);
        Textures.insert({ meshRenderer->diffuse,diffuseTexture });
    }

    if (meshRenderer->normalMap && !Textures.contains(meshRenderer->normalMap)) {
        GLuint normalTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->normalMap);
        Textures.insert({ meshRenderer->normalMap,normalTexture });
    }
}

void OpenglRenderer::SetupFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER,screenFBO->id);
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
    GLuint diffuseTexture = GetTexture(meshRenderer->diffuse);
    GLuint normalTexture = GetTexture(meshRenderer->normalMap);

	glUseProgram(PBRShader);

    glUniformMatrix4fv(glGetUniformLocation(PBRShader, "model"),1,false, glm::value_ptr(model));

    if (meshRenderer->diffuse) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        glUniform1i(glGetUniformLocation(PBRShader, "diffuseMap"), 0);
    }
    if (meshRenderer->normalMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(PBRShader, "normalMap"), 1);
    }
    
	glBindVertexArray(mesh->vao);
	glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D,0);

}

void OpenglRenderer::EndFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);

    //Render Frame buffer
    glUseProgram(screenShader);

    glBindTexture(GL_TEXTURE_2D, screenFBO->colorAttachment);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(screenShader, "samples"),screenFBO->samples);

    glBindVertexArray(screenQuad->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenglRenderer::Clear()
{

}

void OpenglRenderer::FrameBufferResizeCallBack(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    UpdateFrameBuffer(screenFBO, width, height);
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


std::shared_ptr<OpenglRenderer::GLFrameBuffer> OpenglRenderer::CreateFrameBuffer(bool useDepthStencil,int samples)
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

    fbo->colorAttachment = CreateTexture(GL_TEXTURE_2D_MULTISAMPLE);
    SetMultiSampleTextureData(GL_TEXTURE_2D_MULTISAMPLE, &fboImage,samples);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbo->colorAttachment, 0);
    fbo->width = windowWidth;
    fbo->height = windowHeight;
    fbo->samples = samples;


    //DepthStencil render buffer
    if (useDepthStencil) {
        glGenRenderbuffers(1, &fbo->depthStencilRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, fbo->depthStencilRBO);

        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->depthStencilRBO);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glEnable(GL_DEPTH_TEST);//enable depth testing on the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}

void OpenglRenderer::UpdateFrameBuffer(std::shared_ptr<OpenglRenderer::GLFrameBuffer> frameBuffer, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id);
    Image fboImage = Image();
    fboImage.data = NULL;
    fboImage.Width = width;
    fboImage.Height = height;
    fboImage.NRChannels = 3;

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBuffer->colorAttachment);
    SetMultiSampleTextureData(GL_TEXTURE_2D_MULTISAMPLE, &fboImage,frameBuffer->samples);
    frameBuffer->width = width;
    frameBuffer->height = height;
    if (frameBuffer->depthStencilRBO != 0) {
        glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer->depthStencilRBO);

        glRenderbufferStorageMultisample(GL_RENDERBUFFER, frameBuffer->samples, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    
    //position
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Mesh::Vertex), (void*)0);
	glEnableVertexAttribArray(0);

    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normal));
    glEnableVertexAttribArray(1);

    //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex,uv));
    glEnableVertexAttribArray(2);

    //tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, tangent));
    glEnableVertexAttribArray(3);

    //biTangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, biTangent));
    glEnableVertexAttribArray(4);

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

    glGenerateMipmap(type);
    return texture;
}

GLuint OpenglRenderer::CreateTexture(GLenum type, GLenum minFilter, GLenum magFilter)
{
    //Texture Already created 
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(type, texture);

    // set the texture wrapping and filtering options
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);

    glGenerateMipmap(type);
    return texture;
}

void OpenglRenderer::SetTextureData(GLenum target,Image* image)
{
    //!this function does not bind the texture
    GLenum format;
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;break;
    case 2: format = GL_RG;break;
    case 3: format = image->gammaCorrect ? GL_SRGB : GL_RGB;break;
    case 4: format = image->gammaCorrect ? GL_SRGB_ALPHA : GL_RGBA;break;
    default: format = GL_RGB;break;
    }
    glTexImage2D(target,0, format, image->Width, image->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    //glGenerateMipmap(target);
}

void OpenglRenderer::SetMultiSampleTextureData(GLenum target, Image* image,int samples)
{
    GLenum format;
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;break;
    case 2: format = GL_RG;break;
    case 3: format = GL_RGB;break;
    case 4: format = GL_RGBA;break;
    default: format = GL_RGB;break;
    }
    glTexImage2DMultisample(target, samples, GL_RGB16, image->Width, image->Height, GL_TRUE);
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return Textures[image];
}

GLuint OpenglRenderer::CreateCubeMap(std::vector<std::string> faces)
{
    GLuint cubeMap = CreateTexture(GL_TEXTURE_CUBE_MAP,GL_LINEAR,GL_LINEAR);
    for (int i = 0;i < 6;i++) {
        Image* faceImage = Ressources::LoadImageFromFile(faces[i],false);
        SetTextureData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, faceImage);

        delete faceImage;
    }
    return cubeMap;
}
