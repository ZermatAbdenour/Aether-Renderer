#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../Engine/Ressources.h"
#include <Imgui/imgui_internal.h>
#include <random>

GLFWwindow* OpenglRenderer::Init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

    //init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();(void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

	return window;
}


void OpenglRenderer::SetupScene(Scene* scene)
{
    //Setup each entity
    Renderer::SetupScene(scene);

    m_PBRShader = CreateShader(Ressources::Shaders::Default);


    m_autoExposureFBO = CreateFrameBuffer();
    SetFrameBufferAttachements(m_autoExposureFBO, windowWidth, windowHeight, 1, 3, None, 0);
    //Setup FBO and the Full screen quad
    {
        m_screenShader = CreateShader(Ressources::Shaders::ScreenShader);



        //m_screenFBO = CreateScreenFrameBuffer(true,4);//add depth stencil attachement with 4 samples
        m_screenFBO = CreateFrameBuffer();
        //add depth stencil attachement with 4 samples
        DepthStencilType defaultDepthStencilType = static_cast<DepthStencilType>(settings.screenFBODepthStencilType);
        SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, 3, defaultDepthStencilType, settings.multiSampling ? settings.samples : 0);

        //SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 1, 3, true, settings.multiSampling ?settings.samples:0);
        m_screenQuad = CreateMesh(Ressources::Primitives::Quad);

    }
    //Setup UBOs
    {
        glGenBuffers(1, &m_matricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matricesUBO);
    }


    //Setup skybox
    {
        m_skyBoxShader = CreateShader(Ressources::Shaders::Skybox);
        m_skyboxMesh = CreateMesh(Ressources::Primitives::Cube);

        m_skyBoxMap = CreateCubeMap(std::vector<std::string>{
            "skybox/right.jpg",
                "skybox/left.jpg",
                "skybox/top.jpg",
                "skybox/bottom.jpg",
                "skybox/front.jpg",
                "skybox/back.jpg"
        });
    }
    //Set the "Lights" UBO
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

        glGenBuffers(1, &m_lightsUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUBO);
        glBufferData(GL_UNIFORM_BUFFER, 16 + sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS + sizeof(GLPointLight) * MAX_POINTLIGHTS, nullptr, GL_DYNAMIC_DRAW);

        int offset = 0;
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &numDirectionalLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &numPointLights);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset,sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS, &directionalLights[0]);
        offset += sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLPointLight) * MAX_POINTLIGHTS, &pointLights[0]);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightsUBO);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    //Setup bloom and ping pong framebuffers
    {
        m_gaussianBlurShader = CreateShader(Ressources::Shaders::Gaussianblur);
        m_kernelBlurShader = CreateShader(Ressources::Shaders::Kernel);
        m_pingpongFBOs[0] = CreateFrameBuffer();
        SetFrameBufferAttachements(m_pingpongFBOs[0], windowWidth, windowHeight, 1, 3, None, 0);
        m_pingpongFBOs[1] = CreateFrameBuffer();
        SetFrameBufferAttachements(m_pingpongFBOs[1], windowWidth, windowHeight, 1, 3, None, 0);
    }

    //Settup depth testing
    {
        m_earlyDepthTestingShader = CreateShader(Ressources::Shaders::EarlyDepthTesting);
    }

    //SSAO
    {
        m_ssaoNoiseTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR,GL_REPEAT,GL_REPEAT);
    }
}
void OpenglRenderer::SetupEntity(std::shared_ptr<Entity> entity)
{
	MeshRenderer* meshRenderer = entity->meshRenderer;
    if (!meshRenderer)
        return;

    //Create and add Glmesh
    if (meshRenderer->mesh && !m_meshs.contains(meshRenderer->mesh)) {
        auto glmesh = CreateMesh(meshRenderer->mesh);
        m_meshs.insert({ meshRenderer->mesh, glmesh });
    }

    //Create and add textures
    if (meshRenderer->diffuse && !m_textures.contains(meshRenderer->diffuse)) {
        GLuint diffuseTexture = CreateTexture(GL_TEXTURE_2D,GL_LINEAR,GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->diffuse);
        m_textures.insert({ meshRenderer->diffuse,diffuseTexture });
    }

    if (meshRenderer->normalMap && !m_textures.contains(meshRenderer->normalMap)) {
        GLuint normalTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->normalMap);
        m_textures.insert({ meshRenderer->normalMap,normalTexture });
    }
}

void OpenglRenderer::SetupFrame()
{
    //ImGui new frame
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    glBindFramebuffer(GL_FRAMEBUFFER,m_screenFBO->id);
    glEnable(GL_DEPTH_TEST);
	glClearColor(1, 0.2, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    //Set the "Camera" UBO sub data 
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_currentScene->camera.Projection(windowWidth, windowHeight)));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_currentScene->camera.View()));
        glBufferSubData(GL_UNIFORM_BUFFER,2* sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(m_currentScene->camera.position));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

    }
    //Render SkyBox
    {
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glUseProgram(m_skyBoxShader);
        glBindVertexArray(m_skyboxMesh->vao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBoxMap);
	    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }
}

void OpenglRenderer::RenderFrame()
{
    //Depth Pre-pass
    if (settings.zPrePass) {
        glColorMask(0, 0, 0, 0);
        glDepthFunc(GL_LESS);
        glUseProgram(m_earlyDepthTestingShader);

        m_currentScene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
            if (!entity->meshRenderer)
                return;
            EarlyDepthTestEntity(entity->meshRenderer, entity->model);
            });
    }
    //SSAO Pass
    if (settings.SSAO) {
        //Generating Kernek
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        std::vector<glm::vec3> ssaokernel;
        for (unsigned int i = 0; i < settings.kernelSize; ++i)
        {
            glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator)
            );
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float)i / 64.0;
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaokernel.push_back(sample);
        }

        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
            ssaoNoise.push_back(noise);
        }
        glBindTexture(GL_TEXTURE_2D,m_ssaoNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);

        m_currentScene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
            if (!entity->meshRenderer)
                return;
            SSAOPass(entity->meshRenderer, entity->model);
        });
    }

    //Lighting Pass
    if (settings.zPrePass) {
        glDepthFunc(GL_EQUAL);
        glColorMask(1, 1, 1, 1);
    }
    else {
        glDepthFunc(GL_LESS);
        glColorMask(1, 1, 1, 1);
    }
    glUseProgram(m_PBRShader);
    m_currentScene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
        if (!entity->meshRenderer)
            return;
        RenderEntity(entity->meshRenderer, entity->model);
    });
}

void OpenglRenderer::EarlyDepthTestEntity(MeshRenderer* meshRenderer, glm::mat4 model) {
    //Get necessary data to render 
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    glUniformMatrix4fv(glGetUniformLocation(m_earlyDepthTestingShader, "model"), 1, false, glm::value_ptr(model));
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void OpenglRenderer::SSAOPass(MeshRenderer* meshRenderer, glm::mat4 model) {

}
void OpenglRenderer::RenderEntity(MeshRenderer* meshRenderer, glm::mat4 model)
{
    //Get necessary data to render 
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    GLuint diffuseTexture = GetTexture(meshRenderer->diffuse);
    GLuint normalTexture = GetTexture(meshRenderer->normalMap);


    glUniformMatrix4fv(glGetUniformLocation(m_PBRShader, "model"), 1, false, glm::value_ptr(model));

    if (meshRenderer->diffuse) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        glUniform1i(glGetUniformLocation(m_PBRShader, "diffuseMap"), 0);
    }
    if (meshRenderer->normalMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(m_PBRShader, "normalMap"), 1);
    }
    if (meshRenderer->specularMap) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(m_PBRShader, "specularMap"), 2);
    }

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

}

void OpenglRenderer::EndFrame()
{
    glDepthFunc(GL_LESS);
    //auto Exposure
    if (settings.HDR&&settings.toneMapping && settings.autoExposure) {
        glBindFramebuffer(GL_READ_BUFFER, m_screenFBO->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_autoExposureFBO->id);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, m_autoExposureFBO->colorAttachments[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glm::vec3 luminescence;
        int maxDim = glm::max(windowWidth, windowHeight);
        int mipmapLevels = static_cast<int>(glm::floor(glm::log2(static_cast<float>(maxDim))));
        glGetTexImage(GL_TEXTURE_2D, mipmapLevels, GL_RGB, GL_FLOAT, &luminescence);
        const float lum = 0.2126f * luminescence.r + 0.7152f * luminescence.g + 0.0722f * luminescence.b;

        float sceneExposureMultiplier = 0.6;
        float sceneExposureRangeMin = 0.1;
        float sceneExposureRangeMax = 10;
        settings.exposure = glm::mix(settings.exposure, 0.5f / lum * settings.exposureMultiplier, settings.adjustmentSpeed);
        settings.exposure = glm::clamp(settings.exposure, sceneExposureRangeMin, sceneExposureRangeMax);
    }

    
    bool horizontal = true, first_iteration = true;



    if (settings.bloom) {

        glBindFramebuffer(GL_READ_BUFFER, m_screenFBO->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pingpongFBOs[0]->id);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        if (settings.bloomType == RendererSettings::gaussianBlur)
            glUseProgram(m_gaussianBlurShader);
        else
            glUseProgram(m_kernelBlurShader);
        for (unsigned int i = 0; i < settings.amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBOs[horizontal]->id);

            if(settings.bloomType == RendererSettings::gaussianBlur)
            glUniform1i(glGetUniformLocation(m_gaussianBlurShader, "horizontal"), horizontal);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(
                GL_TEXTURE_2D, m_pingpongFBOs[!horizontal]->colorAttachments[0]
            );
            glUniform1i(glGetUniformLocation(m_gaussianBlurShader, "image"), 0);

            glBindVertexArray(m_screenQuad->vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDisable(GL_DEPTH_TEST);

    //Render Frame buffer
    glUseProgram(m_screenShader);
    
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_screenFBO->colorAttachments[0]);
    glUniform1i(glGetUniformLocation(m_screenShader, "MSScreenTexture"), 0);
   

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_screenFBO->colorAttachments[0]);
    glUniform1i(glGetUniformLocation(m_screenShader, "screenTexture"), 1);

    if (settings.bloom) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_pingpongFBOs[!horizontal]->colorAttachments[0]);
        glUniform1i(glGetUniformLocation(m_screenShader, "bloomTexture"), 2);
    }

    //Settings
    glUniform1i(glGetUniformLocation(m_screenShader, "multiSampling"), settings.multiSampling);
    glUniform1i(glGetUniformLocation(m_screenShader, "samples"),m_screenFBO->samples);
    glUniform1i(glGetUniformLocation(m_screenShader, "gammaCorrection"), settings.gammaCorrection);
    glUniform1f(glGetUniformLocation(m_screenShader, "gamma"), settings.gamma);
    glUniform1i(glGetUniformLocation(m_screenShader, "bloom"), settings.bloom);
    glUniform1i(glGetUniformLocation(m_screenShader, "toneMapping"), settings.toneMapping && settings.HDR);
    glUniform1f(glGetUniformLocation(m_screenShader, "exposure"), settings.exposure);

    glBindVertexArray(m_screenQuad->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenglRenderer::Clear()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OpenglRenderer::FrameBufferResizeCallBack(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    SetFrameBufferAttachements(m_screenFBO, width, height, 2, 3, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
    SetFrameBufferAttachements(m_autoExposureFBO, width, height, 1, 3, m_autoExposureFBO->depthStencilType, 0);

    SetFrameBufferAttachements(m_pingpongFBOs[0], width, height, 1, 3, m_pingpongFBOs[0]->depthStencilType, 0);
    SetFrameBufferAttachements(m_pingpongFBOs[1], width, height, 1, 3, m_pingpongFBOs[1]->depthStencilType, 0);

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

GLuint OpenglRenderer::CreateComputeShader(ComputeShader* shader)
{
    GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
    const char* source = shader->shaderSource.c_str();
    glShaderSource(compute, 1, &source, NULL);
    glCompileShader(compute);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(compute, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, compute);
    glLinkProgram(program);

    // Check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(compute); // We no longer need the shader object after linking
    return program; 
}


std::shared_ptr<OpenglRenderer::GLFrameBuffer> OpenglRenderer::CreateFrameBuffer()
{
    auto fbo = std::make_shared<OpenglRenderer::GLFrameBuffer>();
    fbo->samples = settings.samples;
    fbo->depthStencilType = None;
    glGenFramebuffers(1, &fbo->id);
    return fbo;
}

void OpenglRenderer::DeleteFrameBuffer(std::shared_ptr<GLFrameBuffer> framebuffer)
{
    glDeleteFramebuffers(1, &framebuffer->id);
    for (int i = 0;i < framebuffer->colorAttachments.size();i++)
            glDeleteTextures(1, &framebuffer->colorAttachments[i]);
        //Delete render buffers
        glDeleteTextures(1, &framebuffer->depthStencilBuffer);

        framebuffer.reset();
}

void OpenglRenderer::SetFrameBufferAttachements(std::shared_ptr<OpenglRenderer::GLFrameBuffer> framebuffer,
    int width,
    int height,
    int colorAttachementsCount,
    int NRChannels,
    DepthStencilType depthStencilType,
    int samples)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    // Check for samples change
    if ((samples == 0 && framebuffer->samples > 0) || (samples > 0 && framebuffer->samples == 0)) {
        GLuint* textures = framebuffer->colorAttachments.data();
        glDeleteTextures(framebuffer->colorAttachments.size(), textures);
        framebuffer->colorAttachments.clear();
    }
    if (framebuffer->depthStencilType != None && framebuffer->depthStencilType != depthStencilType) {
        if (framebuffer->depthStencilType == Texture)
            glDeleteTextures(1, &framebuffer->depthStencilBuffer);
        if (framebuffer->depthStencilType == RBO)
            glDeleteRenderbuffers(1, &framebuffer->depthStencilBuffer);
        framebuffer->depthStencilBuffer = 0;
    }

    // Initialize image data
    if (framebuffer->image == nullptr) {
        framebuffer->image = new Image();
    }
    framebuffer->image->data = NULL;
    framebuffer->image->Width = width;
    framebuffer->image->Height = height;
    framebuffer->image->NRChannels = NRChannels;

    // Create color attachments
    for (int i = 0; i < colorAttachementsCount; i++) {
        if (framebuffer->colorAttachments.size() < colorAttachementsCount) {
            GLuint textureColorbuffer;

            if (samples > 0) {
                textureColorbuffer = CreateTexture(GL_TEXTURE_2D_MULTISAMPLE);
            }
            else {
                textureColorbuffer = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
            }

            framebuffer->colorAttachments.push_back(textureColorbuffer);
        }

        // Bind and attach the texture
        if (samples > 0) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->colorAttachments[i]);

            //GLenum internalFormat = settings.HDR ? GL_RGB16F : GL_RGB;
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB16F, framebuffer->image->Width, framebuffer->image->Height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->colorAttachments[i], 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, framebuffer->colorAttachments[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer->colorAttachments[i], 0);
        }
    }

    // Setup draw buffers
    std::vector<GLuint> colorAttachments(colorAttachementsCount);
    for (int i = 0; i < colorAttachementsCount; ++i) {
        colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(colorAttachementsCount, colorAttachments.data());

    framebuffer->samples = samples;
    // Setup DepthStencil render buffer
    if (depthStencilType == DepthStencilType::RBO) {
        if (framebuffer->depthStencilBuffer == 0) {
            glGenRenderbuffers(1, &framebuffer->depthStencilBuffer);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthStencilBuffer);
        if (samples > 0) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
        }
        else {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthStencilBuffer);
    }

    if (depthStencilType == DepthStencilType::Texture) {
        if (framebuffer->depthStencilBuffer == 0) {
            if(samples>0)
                framebuffer->depthStencilBuffer = CreateTexture(GL_MULTISAMPLE);
            else
                framebuffer->depthStencilBuffer = CreateTexture(GL_TEXTURE_2D,GL_NEAREST,GL_NEAREST);

        }

        if (samples > 0) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->depthStencilBuffer);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->depthStencilBuffer, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->depthStencilBuffer, 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, framebuffer->depthStencilBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthStencilBuffer, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depthStencilBuffer, 0);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->depthStencilBuffer);
    }

    framebuffer->depthStencilType = depthStencilType;

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer

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
    return m_meshs[mesh];
}


GLuint OpenglRenderer::CreateTexture(GLenum type, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT)
{
    //Texture Already created 
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(type, texture);

    // set the texture wrapping and filtering options
    if(minFilter != NULL)
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
    if(magFilter != NULL)
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
    if(wrapS != NULL)
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
    if (wrapT != NULL)
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);

    return texture;
}

void OpenglRenderer::SetTextureData(GLenum target,Image* image)
{
    //!this function does not bind the texture
    GLenum format;
    GLenum internalFormat;
    //image->gammaCorrect && image->imageType != Image::ImageType::normal ? GL_SRGB :
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;internalFormat = GL_RED;break;
    case 2: format = GL_RG,internalFormat = GL_RG;break;
    case 3: format = image->gammaCorrect && image->imageType != Image::ImageType::normal ? GL_SRGB : GL_RGB;internalFormat = GL_RGB;break;
    case 4: format = image->gammaCorrect && image->imageType != Image::ImageType::normal ? GL_SRGB_ALPHA : GL_RGBA;internalFormat = GL_RGBA;break;
    default: format = GL_RGB;internalFormat = GL_RGB;break;
    }
    //lTexImage2D(GL_TEXTURE_2D, 0, format, image->Width, image->Height, 0, format, GL_UNSIGNED_BYTE, image->data);
    glTexImage2D(target,0, format, image->Width, image->Height, 0, internalFormat, GL_UNSIGNED_BYTE, image->data);
    glGenerateMipmap(target);
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return m_textures[image];
}

GLuint OpenglRenderer::CreateCubeMap(std::vector<std::string> faces)
{
    GLuint cubeMap;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    for (int i = 0;i < 6;i++) {
        Image* faceImage = Ressources::LoadImageFromFile(faces[i],false);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_SRGB, faceImage->Width, faceImage->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, faceImage->data
        );

        delete faceImage;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return cubeMap;
}

void OpenglRenderer::RendererSettingsTab()
{
    if (ImGui::CollapsingHeader("Pipline")) {
        ImGui::Checkbox("Z pre-pass", &settings.zPrePass);
        const char* enumNames[] = { "None", "RBO" ,"Texture"};
        if (ImGui::Combo("DepthStencil type", &settings.screenFBODepthStencilType, enumNames, IM_ARRAYSIZE(enumNames))) {
            DepthStencilType type = static_cast<DepthStencilType>(settings.screenFBODepthStencilType);
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, m_screenFBO->image->NRChannels, type, settings.multiSampling ? settings.samples : 0);

        }
    }

    if (ImGui::CollapsingHeader("Anti-Aliasing")) {
        if (ImGui::Checkbox("enable multisampling", &settings.multiSampling)) {
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, m_screenFBO->image->NRChannels, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
        }
        if (settings.multiSampling && ImGui::DragInt("samples", &settings.samples, 1, 1, 8)) {
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, m_screenFBO->image->NRChannels, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
        }
    }

    if (ImGui::CollapsingHeader("Gamma correction")) {
        if (ImGui::Checkbox("enable gamma correction", &settings.gammaCorrection)) {
            ReloadTextures(settings.gammaCorrection);
        }
        if (settings.gammaCorrection)
            ImGui::DragFloat("Gamma", &settings.gamma, 0.1f, 0.0f, 5.0f);
    }

    if (ImGui::CollapsingHeader("HDR")) {
        if (ImGui::Checkbox("enable HDR", &settings.HDR)) {
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, m_screenFBO->image->NRChannels, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
        }
        if (settings.HDR)
        {
            ImGui::Checkbox("tonemapping", &settings.toneMapping);
            
            if (settings.toneMapping) {
                ImGui::BeginDisabled(settings.autoExposure);
                ImGui::DragFloat("exposure", &settings.exposure, 0.1f, 0.0f, 5.0f);
                ImGui::EndDisabled();
                ImGui::Checkbox("auto exposure", &settings.autoExposure);
            }
            if (settings.toneMapping && settings.autoExposure) {
                ImGui::InputFloat("exposure multiplier", &settings.exposureMultiplier);
                ImGui::InputFloat("adjustment speed", &settings.adjustmentSpeed);
            }
        }
    }

    if (ImGui::CollapsingHeader("Bloom")) {
        ImGui::Checkbox("enable", &settings.bloom);

        const char* enumNames[] = { "Kernel Blur", "Gaussian Blur"};
        int currentIndex = static_cast<int>(settings.bloomType);
        if (ImGui::Combo("blur type", &currentIndex, enumNames, IM_ARRAYSIZE(enumNames))) 
            settings.bloomType = static_cast<RendererSettings::BloomTypes>(currentIndex);

        if (settings.bloom && settings.bloomType == RendererSettings::gaussianBlur) {
            ImGui::InputInt("amount", &settings.amount);
            settings.amount = glm::clamp(settings.amount, 1, 100);
        }
        else
            settings.amount = 1;
    }
}

void OpenglRenderer::ReloadTextures(bool gammaCorrection)
{
    for (const auto& pair : m_textures) {
        if (pair.first->imageType == Image::ImageType::normal)
            continue;
        glBindTexture(GL_TEXTURE_2D, pair.second);
        pair.first->gammaCorrect = gammaCorrection;
        SetTextureData(GL_TEXTURE_2D, pair.first);
    }
}