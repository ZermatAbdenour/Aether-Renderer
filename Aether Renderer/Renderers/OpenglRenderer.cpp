#include <glad/glad.h>
#include "OpenglRenderer.h"
#include "../Utilities/FileUtil.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "../Engine/Ressources.h"
#include <Imgui/imgui_internal.h>
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include "../Engine/Editor.h"

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
    if (settings.VSync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        OpenglRenderer* renderer = static_cast<OpenglRenderer*>(glfwGetWindowUserPointer(window));
        if (!renderer)
            return;
        renderer->FrameBufferResizeCallBack(width, height);
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


void OpenglRenderer::Setup(Scene* scene)
{
    //Setup each entity
    scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
        SetupEntity(entity);
        });

    m_PBRShader = CreateShader(Ressources::Shaders::PBR);

    m_autoExposureFBO = CreateFrameBuffer();
    SetFrameBufferAttachements(m_autoExposureFBO, windowWidth, windowHeight, 1, settings.HDR, None, 0);
    //Setup FBO and the Full screen quad
    {
        m_screenShader = CreateShader(Ressources::Shaders::ScreenShader);

        //m_screenFBO = CreateScreenFrameBuffer(true,4);//add depth stencil attachement with 4 samples
        m_screenFBO = CreateFrameBuffer();
        //add depth stencil attachement with 4 samples
        DepthStencilType defaultDepthStencilType = static_cast<DepthStencilType>(settings.screenFBODepthStencilType);
        SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, settings.HDR, defaultDepthStencilType, settings.multiSampling ? settings.samples : 0);

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
        m_EquiRecToCubeMapShader = CreateShader(Ressources::Shaders::EquiRecToCubeMap);
        m_diffuseIrradianceShader = CreateShader(Ressources::Shaders::DiffuseIrradiance);
        m_hdrPreFilteringShader = CreateShader(Ressources::Shaders::HDRPrefiltering);
        m_convoluteBRDFShader = CreateShader(Ressources::Shaders::ConvoluteBRDF);

        m_cubeMesh = CreateMesh(Ressources::Primitives::Cube);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        LoadSkyBox(scene->environmentMap);
    }

    //Setup bloom and ping pong framebuffers
    {
        m_gaussianBlurShader = CreateShader(Ressources::Shaders::Gaussianblur);
        m_kernelBlurShader = CreateShader(Ressources::Shaders::Kernel);
        m_boomPingpongFBOs[0] = CreateFrameBuffer();
        SetFrameBufferAttachements(m_boomPingpongFBOs[0], windowWidth, windowHeight, 1, settings.HDR, None, 0);
        m_boomPingpongFBOs[1] = CreateFrameBuffer();
        SetFrameBufferAttachements(m_boomPingpongFBOs[1], windowWidth, windowHeight, 1, settings.HDR, None, 0);
    }

    //Setup depth testing
    {
        m_earlyDepthTestingShader = CreateShader(Ressources::Shaders::EarlyDepthTesting);
    }
    //Setup shadowmaping
    {
        m_shadowDepthFBO = CreateFrameBuffer();
        SetFrameBufferAttachements(m_shadowDepthFBO, settings.shadowResolution.x, settings.shadowResolution.y, 1, settings.HDR, TextureDepthStencil, 0);
        //Since the default is GL_Repeat
        glBindTexture(GL_TEXTURE_2D, m_shadowDepthFBO->depthStencilBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        m_shadowMapShader = CreateShader(Ressources::Shaders::ShadowMap);
    }
    //setup SSAO
    {
        m_ssaoFBO = CreateFrameBuffer();
        SetFrameBufferAttachements(m_ssaoFBO, windowWidth, windowHeight, 1, settings.HDR, None, 0);
        m_resolveDepthFBO = CreateFrameBuffer();
        SetFrameBufferAttachements(m_resolveDepthFBO, windowWidth, windowHeight, 1, settings.HDR, TextureDepthStencil, 0);
        m_ssaoBlurFBO = CreateFrameBuffer();
        SetFrameBufferAttachements(m_ssaoBlurFBO, windowWidth, windowHeight, 1, settings.HDR, None, 0);

        m_ssaoShader = CreateShader(Ressources::Shaders::SSAO);
        m_ssaoBlurShader = CreateShader(Ressources::Shaders::SSAOBlur);
        m_ssaoNoiseTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glGenBuffers(1, &ssaoKernelSSBO);

        //Generating Kernel
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                0.0f);
            m_ssaoNoise.push_back(noise);
        }
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
    if (meshRenderer->diffuse != nullptr && !m_textures.contains(meshRenderer->diffuse)) {
        GLuint diffuseTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->diffuse);
        glGenerateMipmap(GL_TEXTURE_2D);
        m_textures.insert({ meshRenderer->diffuse,diffuseTexture });
    }

    if (meshRenderer->normalMap != nullptr && !m_textures.contains(meshRenderer->normalMap)) {
        GLuint normalTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->normalMap);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_textures.insert({ meshRenderer->normalMap,normalTexture });
    }

    if (meshRenderer->metalicMap != nullptr && !m_textures.contains(meshRenderer->metalicMap)) {
        GLuint metalicTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->metalicMap);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_textures.insert({ meshRenderer->metalicMap,metalicTexture });
    }
    if (!meshRenderer->packedAoRM && meshRenderer->roughnessMap != nullptr && !m_textures.contains(meshRenderer->roughnessMap)) {
        GLuint roughnessTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->roughnessMap);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_textures.insert({ meshRenderer->roughnessMap,roughnessTexture });
    }
    if (!meshRenderer->packedAoRM && meshRenderer->aoMap != nullptr && !m_textures.contains(meshRenderer->aoMap)) {
        GLuint aoTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->aoMap);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_textures.insert({ meshRenderer->aoMap,aoTexture });
    }

    if (meshRenderer->emissiveMap != nullptr && !m_textures.contains(meshRenderer->emissiveMap)) {
        GLuint emissiveTexture = CreateTexture(GL_TEXTURE_2D, GL_LINEAR, GL_LINEAR);
        SetTextureData(GL_TEXTURE_2D, meshRenderer->emissiveMap);
        glGenerateMipmap(GL_TEXTURE_2D);

        m_textures.insert({ meshRenderer->emissiveMap,emissiveTexture });
    }
}
GLuint cubeVAO, cubeVBO;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void OpenglRenderer::RenderScene(Scene* scene)
{

    glBindFramebuffer(GL_FRAMEBUFFER, m_screenFBO->id);

    glEnable(GL_DEPTH_TEST);

    glClearColor(1, 0.2, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    //Set the "Camera" UBO sub data 
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(scene->camera.projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(scene->camera.view));
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(scene->camera.position));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

    }

    //Render SkyBox
    {
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glUseProgram(m_skyBoxShader);
        glBindVertexArray(m_cubeMesh->vao);
        glActiveTexture(GL_TEXTURE0);

        switch (settings.targetSkyBoxMap)
        {
        case RendererSettings::envirenmentMap:
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->envirenmentMap);
            break;
        case RendererSettings::diffuseIrradiance:
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->diffuseIrradianceMap);
            break;
        case RendererSettings::prefilteredMap:
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->prefilteredMap);
            break;
        default:
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->envirenmentMap);
            break;
        }
        glUniform1i(glGetUniformLocation(m_skyBoxShader, "cubeMap"), 0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);

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
            pointLights[i] = GLPointLight(scene->PointLights[i]);
        }

        glGenBuffers(1, &m_lightsUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUBO);
        glBufferData(GL_UNIFORM_BUFFER, 16 + sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS + sizeof(GLPointLight) * MAX_POINTLIGHTS, nullptr, GL_DYNAMIC_DRAW);

        int offset = 0;
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &numDirectionalLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &numPointLights);
        offset += 16;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS, &directionalLights[0]);
        offset += sizeof(GLDirectionalLight) * MAX_DIRECTIONALLIGHTS;
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(GLPointLight) * MAX_POINTLIGHTS, &pointLights[0]);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightsUBO);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    //Shadow map
    {
        //LightSpace matrix
        glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 5.0f);
        float lightDistance = 2;
        glm::mat4 lightView = glm::lookAt(glm::vec3(scene->DirectionalLights[0].direction * -lightDistance),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        m_lightSpaceMatrix = lightProjection * lightView;

    }

    //Depth Pre-pass
    if (settings.zPrePass) {
        glColorMask(0, 0, 0, 0);
        glDepthFunc(GL_LESS);
        glUseProgram(m_earlyDepthTestingShader);

        scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
            if (!entity->meshRenderer)
                return;
            EarlyDepthTestEntity(entity->meshRenderer, entity->model);
            });
    }
    //SSAO Pass
    if (settings.SSAO) {

        glBindTexture(GL_TEXTURE_2D, m_ssaoNoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);

        //resolve depth in an intermediat fbo
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_screenFBO->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_resolveDepthFBO->id);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO->id);


        glDepthFunc(GL_LESS);
        glColorMask(1, 1, 1, 1);

        glUseProgram(m_ssaoShader);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_resolveDepthFBO->depthStencilBuffer);

        glUniform1i(glGetUniformLocation(m_ssaoShader, "depthTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_ssaoNoiseTexture);
        glUniform1i(glGetUniformLocation(m_ssaoShader, "noiseTexture"), 1);

        glUniform1f(glGetUniformLocation(m_ssaoShader, "sampleRad"), settings.sampleRad);
        
        m_ssaokernel.clear();
        std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
        std::default_random_engine generator;
        for (unsigned int i = 0; i < settings.kernelSize; ++i)
        {
            glm::vec3 sample(
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator) * 2.0 - 1.0,
                randomFloats(generator)
            );
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = (float)i / settings.kernelSize;
            scale = glm::mix(0.1f, 1.0f, scale * scale);
            sample *= scale;
            m_ssaokernel.push_back(sample);
        }
        //set kernel buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssaoKernelSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_ssaokernel.size() * sizeof(glm::vec3), m_ssaokernel.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssaoKernelSSBO);
        glUniform2f(glGetUniformLocation(m_ssaoShader, "noiseScale"), windowWidth / 4, windowHeight / 4);
        glm::mat4 projection = scene->camera.projection;
        glUniformMatrix4fv(glGetUniformLocation(m_ssaoShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 projectionInv = glm::inverse(scene->camera.projection);
        glUniformMatrix4fv(glGetUniformLocation(m_ssaoShader, "projectionInv"), 1, GL_FALSE, glm::value_ptr(projectionInv));
        glUniform1f(glGetUniformLocation(m_ssaoShader, "power"), settings.power);
        glUniform1f(glGetUniformLocation(m_ssaoShader, "bias"), settings.bias);

        glBindVertexArray(m_screenQuad->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Blur ssao
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO->id);

        glUseProgram(m_ssaoBlurShader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_ssaoFBO->colorAttachments[0]);

        glUniform1i(glGetUniformLocation(m_ssaoBlurShader, "ssaoTexture"), 0);

        glBindVertexArray(m_screenQuad->vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO->id);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //Shadowmap pass
    glDepthFunc(GL_LESS);
    glColorMask(1, 1, 1, 1);
    if (settings.shadowMapping) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowDepthFBO->id);
        glViewport(0, 0, settings.shadowResolution.x, settings.shadowResolution.y);
        glClearColor(1, 1, 1, 1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glUseProgram(m_shadowMapShader);
        glUniformMatrix4fv(glGetUniformLocation(m_shadowMapShader, "lightSpaceMatrix"), 1, false, glm::value_ptr(m_lightSpaceMatrix));
        scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
            if (!entity->meshRenderer)
                return;
            ShadowMapEntity(entity->meshRenderer, entity->model);
            });
    }
    glViewport(0, 0, windowWidth, windowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, m_screenFBO->id);
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
    glUniformMatrix4fv(glGetUniformLocation(m_PBRShader, "lightSpace"), 1, false, glm::value_ptr(m_lightSpaceMatrix));
    //Settings
    glUniform1i(glGetUniformLocation(m_PBRShader, "SSAO"), settings.SSAO);
    glUniform1i(glGetUniformLocation(m_PBRShader, "SSAOOnly"), settings.SSAOOnly);
    glUniform1i(glGetUniformLocation(m_PBRShader, "shadowMapping"), settings.shadowMapping);
    glUniform1i(glGetUniformLocation(m_PBRShader, "softShadow"), settings.softShadow);
    glUniform1f(glGetUniformLocation(m_PBRShader, "bias"), settings.shadowbias);
    glUniform1f(glGetUniformLocation(m_PBRShader, "minBias"), settings.minBias);


    scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
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

void OpenglRenderer::ShadowMapEntity(MeshRenderer* meshRenderer, glm::mat4 model)
{
    //Get necessary data to render 
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    glUniformMatrix4fv(glGetUniformLocation(m_shadowMapShader, "model"), 1, false, glm::value_ptr(model));
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenglRenderer::RenderEntity(MeshRenderer* meshRenderer, glm::mat4 model)
{
    //Get necessary data to render 
    std::shared_ptr<GLMesh> mesh = GetGLMesh(meshRenderer->mesh);
    GLuint diffuseTexture = GetTexture(meshRenderer->diffuse);
    GLuint normalTexture = GetTexture(meshRenderer->normalMap);
    GLuint metalicTexture = GetTexture(meshRenderer->metalicMap);
    GLuint roughnessTexture = GetTexture(meshRenderer->roughnessMap);
    GLuint aoTexture = GetTexture(meshRenderer->aoMap);
    GLuint emissiveTexture = GetTexture(meshRenderer->emissiveMap);

    glUniformMatrix4fv(glGetUniformLocation(m_PBRShader, "model"), 1, false, glm::value_ptr(model));

    glUniform1i(glGetUniformLocation(m_PBRShader, "baseColorOnly"), meshRenderer->diffuse == nullptr);
    glUniform4fv(glGetUniformLocation(m_PBRShader, "baseColor"), 1, glm::value_ptr(meshRenderer->baseColor));
    glUniform1f(glGetUniformLocation(m_PBRShader, "metallic"), meshRenderer->metallic);
    glUniform1f(glGetUniformLocation(m_PBRShader, "roughness"), meshRenderer->roughness);
    glUniform1f(glGetUniformLocation(m_PBRShader, "ao"), meshRenderer->ao);
    glUniform1i(glGetUniformLocation(m_PBRShader, "packedAoRM"), meshRenderer->packedAoRM);

    glActiveTexture(GL_TEXTURE0);
    if (meshRenderer->diffuse)
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "diffuseMap"), 0);

    glActiveTexture(GL_TEXTURE1);
    if (meshRenderer->normalMap)
        glBindTexture(GL_TEXTURE_2D, normalTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "normalMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    if (meshRenderer->metalicMap)
        glBindTexture(GL_TEXTURE_2D, metalicTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "metalicMap"), 2);

    glActiveTexture(GL_TEXTURE3);
    if (!meshRenderer->packedAoRM && meshRenderer->roughnessMap)
        glBindTexture(GL_TEXTURE_2D, roughnessTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "roughnessMap"), 3);


    glActiveTexture(GL_TEXTURE4);
    if (!meshRenderer->packedAoRM&&meshRenderer->aoMap)
        glBindTexture(GL_TEXTURE_2D, aoTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "aoMap"), 4);

    glActiveTexture(GL_TEXTURE5);
    if (meshRenderer->emissiveMap)
        glBindTexture(GL_TEXTURE_2D, emissiveTexture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "emissiveMap"), 5);

    glActiveTexture(GL_TEXTURE6);
    if (settings.SSAO)
        glBindTexture(GL_TEXTURE_2D, m_ssaoBlurFBO->colorAttachments[0]);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "ssaoTexture"), 6);

    glActiveTexture(GL_TEXTURE7);
    if (settings.shadowMapping)
        glBindTexture(GL_TEXTURE_2D, m_shadowDepthFBO->depthStencilBuffer);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "shadowMap"), 7);

    glActiveTexture(GL_TEXTURE8);
    if(settings.enableDiffuseIrradiance)
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->diffuseIrradianceMap);
    else
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "irradianceMap"), 8);

    glActiveTexture(GL_TEXTURE9);
    if(settings.enableSpecularIBL)
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyBox->prefilteredMap);
    else
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUniform1i(glGetUniformLocation(m_PBRShader, "prefilterMap"), 9);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, m_skyBox->brdfLutTexture);
    glUniform1i(glGetUniformLocation(m_PBRShader, "brdfLUT"), 10);

    //Draw the mesh
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, meshRenderer->mesh->indices.size(), GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

}

void OpenglRenderer::LoadSkyBox(Image* envirementMap)
{
    if (m_skyBox != nullptr) {
        std::cout << m_skyBox->diffuseIrradianceMap << std::endl;
        glDeleteTextures(1, &m_skyBox->hdrTexture);
        glDeleteTextures(1, &m_skyBox->envirenmentMap);
        glDeleteTextures(1, &m_skyBox->diffuseIrradianceMap);
        delete m_skyBox;
    }
    glDisable(GL_CULL_FACE);
    m_skyBox = CreateHDRCubeMap(envirementMap, envirementMap->Width, envirementMap->Width);
    glEnable(GL_CULL_FACE);
}

void OpenglRenderer::PostProcess()
{
    glDepthFunc(GL_LESS);
    //auto Exposure
    if (settings.HDR && settings.toneMapping && settings.autoExposure) {
        glBindFramebuffer(GL_READ_BUFFER, m_screenFBO->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_autoExposureFBO->id);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, m_autoExposureFBO->colorAttachments[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glm::vec3 luminescence;
        //Max
        int maxDim = windowWidth>windowHeight?windowWidth:windowHeight;
        int mipmapLevels = static_cast<int>(glm::floor(glm::log2(static_cast<float>(maxDim))));
        glGetTexImage(GL_TEXTURE_2D, mipmapLevels, GL_RGB, GL_FLOAT, &luminescence);
        const float lum = 0.2126f * luminescence.r + 0.7152f * luminescence.g + 0.0722f * luminescence.b;
        if (lum > 0.0001) {
            float sceneExposureMultiplier = 0.6;
            float sceneExposureRangeMin = 0.1;
            float sceneExposureRangeMax = 5;
            settings.exposure = glm::mix(settings.exposure, 0.5f / lum * settings.exposureMultiplier, settings.adjustmentSpeed);
            settings.exposure = glm::clamp(settings.exposure, sceneExposureRangeMin, sceneExposureRangeMax);
        }
    }

    bool horizontal = true, first_iteration = true;

    if (settings.bloom) {

        glBindFramebuffer(GL_READ_BUFFER, m_screenFBO->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_boomPingpongFBOs[0]->id);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        if (settings.bloomType == RendererSettings::gaussianBlur) {
            glUseProgram(m_gaussianBlurShader);
            for (unsigned int i = 0; i < settings.amount; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_boomPingpongFBOs[horizontal]->id);

                if (settings.bloomType == RendererSettings::gaussianBlur)
                    glUniform1i(glGetUniformLocation(m_gaussianBlurShader, "horizontal"), horizontal);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(
                    GL_TEXTURE_2D, m_boomPingpongFBOs[!horizontal]->colorAttachments[0]
                );
                glUniform1i(glGetUniformLocation(m_gaussianBlurShader, "image"), 0);

                glBindVertexArray(m_screenQuad->vao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                horizontal = !horizontal;
                if (first_iteration)
                    first_iteration = false;
            }
        }
        else
            glUseProgram(m_kernelBlurShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    //Render Frame buffer
    glUseProgram(m_screenShader);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_screenFBO->colorAttachments[0]);
    glUniform1i(glGetUniformLocation(m_screenShader, "MSScreenTexture"), 0);


    glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, m_screenFBO->colorAttachments[0]);
    glBindTexture(GL_TEXTURE_2D, m_screenFBO->colorAttachments[0]);
    glUniform1i(glGetUniformLocation(m_screenShader, "screenTexture"), 1);

    glActiveTexture(GL_TEXTURE2);
    if (settings.bloom) {
        glBindTexture(GL_TEXTURE_2D, m_boomPingpongFBOs[!horizontal]->colorAttachments[0]);
    }
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(m_screenShader, "bloomTexture"), 2);
    //Settings
    glUniform1i(glGetUniformLocation(m_screenShader, "ssao"), settings.SSAO);
    glUniform1i(glGetUniformLocation(m_screenShader, "multiSampling"), settings.multiSampling);
    glUniform1i(glGetUniformLocation(m_screenShader, "samples"), m_screenFBO->samples);
    glUniform1i(glGetUniformLocation(m_screenShader, "gammaCorrection"), settings.gammaCorrection);
    glUniform1f(glGetUniformLocation(m_screenShader, "gamma"), settings.gamma);
    glUniform1i(glGetUniformLocation(m_screenShader, "bloom"), settings.bloom);
    glUniform1i(glGetUniformLocation(m_screenShader, "toneMapping"), settings.toneMapping && settings.HDR);
    glUniform1f(glGetUniformLocation(m_screenShader, "exposure"), settings.exposure);

    glBindVertexArray(m_screenQuad->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void OpenglRenderer::RenderEditor(Editor* editor)
{
    //ImGui new frame
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    editor->Update();

    //render ImGui
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
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
    SetFrameBufferAttachements(m_screenFBO, width, height, 2, settings.HDR, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
    SetFrameBufferAttachements(m_autoExposureFBO, width, height, 1, settings.HDR, m_autoExposureFBO->depthStencilType, 0);

    SetFrameBufferAttachements(m_boomPingpongFBOs[0], width, height, 1, settings.HDR, m_boomPingpongFBOs[0]->depthStencilType, 0);
    SetFrameBufferAttachements(m_boomPingpongFBOs[1], width, height, 1, settings.HDR, m_boomPingpongFBOs[1]->depthStencilType, 0);

    SetFrameBufferAttachements(m_ssaoFBO, width, height, 1, settings.HDR, None, 0);
    SetFrameBufferAttachements(m_resolveDepthFBO, width, height, 1, settings.HDR, TextureDepthStencil, 0);
    SetFrameBufferAttachements(m_ssaoBlurFBO, windowWidth, windowHeight, 1, settings.HDR, None, 0);

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
    bool hdr,
    DepthStencilType depthStencilType,
    int samples)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    // Check for samples change
    bool changeSampling = (samples == 0 && framebuffer->samples > 0) || (samples > 0 && framebuffer->samples == 0);
    if (changeSampling) {
        GLuint* textures = framebuffer->colorAttachments.data();
        glDeleteTextures(framebuffer->colorAttachments.size(), textures);
        framebuffer->colorAttachments.clear();
    }
    if (framebuffer->depthStencilType != None && framebuffer->depthStencilType != depthStencilType || changeSampling) {
        if (framebuffer->depthStencilType == TextureDepthStencil || framebuffer->depthStencilType == TextureDepth)
            glDeleteTextures(1, &framebuffer->depthStencilBuffer);
        if (framebuffer->depthStencilType == RBODepth || framebuffer->depthStencilType == RBODepthStencil)
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
    framebuffer->image->NRChannels = 3;

    // Create color attachments
    if (colorAttachementsCount == 0)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
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
        GLenum internalFormat = settings.HDR ? GL_RGB16F : GL_RGB;
        if (samples > 0) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->colorAttachments[i]);

            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, framebuffer->image->Width, framebuffer->image->Height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->colorAttachments[i], 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, framebuffer->colorAttachments[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_FLOAT, NULL);
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

    GLuint depthStencilAttachement = depthStencilType == RBODepth ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
    GLuint depthStencilInternalFormat = depthStencilType == RBODepth ? GL_DEPTH_COMPONENT32 : GL_DEPTH32F_STENCIL8;
    // Setup DepthStencil render buffer
    if (depthStencilType == RBODepth || depthStencilType == RBODepthStencil) {
        if (framebuffer->depthStencilBuffer == 0) {
            glGenRenderbuffers(1, &framebuffer->depthStencilBuffer);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->depthStencilBuffer);

        if (samples > 0) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, depthStencilInternalFormat, width, height);
        }
        else {
            glRenderbufferStorage(GL_RENDERBUFFER, depthStencilInternalFormat, width, height);
        }

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthStencilAttachement, GL_RENDERBUFFER, framebuffer->depthStencilBuffer);
    }

    if (depthStencilType == TextureDepth || depthStencilType == TextureDepthStencil) {
        if (framebuffer->depthStencilBuffer == 0) {
            if (samples > 0)
                framebuffer->depthStencilBuffer = CreateTexture(GL_TEXTURE_2D_MULTISAMPLE);
            else
                framebuffer->depthStencilBuffer = CreateTexture(GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST);

        }

        if (samples > 0) {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebuffer->depthStencilBuffer);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, depthStencilInternalFormat, width, height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, depthStencilAttachement, GL_TEXTURE_2D_MULTISAMPLE, framebuffer->depthStencilBuffer, 0);
        }
        else {
            glBindTexture(GL_TEXTURE_2D, framebuffer->depthStencilBuffer);
            GLuint depthStencilFormat = depthStencilType == RBODepth ? GL_DEPTH : GL_DEPTH_STENCIL;
            glTexImage2D(GL_TEXTURE_2D, 0, depthStencilInternalFormat, width, height, 0, depthStencilFormat, GL_UNSIGNED_INT_24_8, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, depthStencilAttachement, GL_TEXTURE_2D, framebuffer->depthStencilBuffer, 0);
        }

    }

    framebuffer->depthStencilType = depthStencilType;

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    //glViewport(0, 0, width, height);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    //normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, normal));
    glEnableVertexAttribArray(1);

    //uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)offsetof(Mesh::Vertex, uv));
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
    if (minFilter != NULL)
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);
    if (magFilter != NULL)
        glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
    if (wrapS != NULL)
        glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
    if (wrapT != NULL)
        glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);

    return texture;
}

void OpenglRenderer::SetTextureData(GLenum target, Image* image)
{
    //!this function does not bind the texture
    GLenum format;
    GLenum internalFormat;
    //image->gammaCorrect && image->imageType != Image::ImageType::normal ? GL_SRGB :
    switch (image->NRChannels)
    {
    case 1: format = GL_RED;internalFormat = GL_RED;break;
    case 2: format = GL_RG, internalFormat = GL_RG;break;
    case 3: format = image->gammaCorrect && image->imageType != Image::ImageType::map ? GL_SRGB : GL_RGB;internalFormat = GL_RGB;break;
    case 4: format = image->gammaCorrect && image->imageType != Image::ImageType::map ? GL_SRGB_ALPHA : GL_RGBA;internalFormat = GL_RGBA;break;
    default: format = GL_RGB;internalFormat = GL_RGB;break;
    }
    //lTexImage2D(GL_TEXTURE_2D, 0, format, image->Width, image->Height, 0, format, GL_UNSIGNED_BYTE, image->data);
    glTexImage2D(target, 0, format, image->Width, image->Height, 0, internalFormat, GL_UNSIGNED_BYTE, image->data);
}

GLuint OpenglRenderer::GetTexture(Image* image)
{
    return m_textures[image];
}
OpenglRenderer::GLHDRCubeMap* OpenglRenderer::CreateHDRCubeMap(Image* image, int width, int height)
{
    GLHDRCubeMap* hdrCubeMap = new GLHDRCubeMap();

    auto captureFBO = CreateFrameBuffer();
    SetFrameBufferAttachements(captureFBO, width, height, 1, true, RBODepth, 0);
    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, image->Width, image->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    hdrCubeMap->hdrTexture = hdrTexture;
    //
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8,
            width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    glUseProgram(m_EquiRecToCubeMapShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glUniform1i(glGetUniformLocation(m_EquiRecToCubeMapShader, "equirectangularMap"), 0);
    glUniformMatrix4fv(glGetUniformLocation(m_EquiRecToCubeMapShader, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO->id);
    //glDepthFunc(GL_LESS);
    //glDisable(GL_DEPTH_TEST);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_EquiRecToCubeMapShader, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }


    hdrCubeMap->envirenmentMap = envCubemap;

    //Generate the diffuse Irradiance
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SetFrameBufferAttachements(captureFBO, 32, 32, 1, true, RBODepth, 0);


    glUseProgram(m_diffuseIrradianceShader);
    glUniformMatrix4fv(glGetUniformLocation(m_diffuseIrradianceShader, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glUniform1i(glGetUniformLocation(m_diffuseIrradianceShader, "environmentMap"), 0);

    glViewport(0, 0, 32, 32); 
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO->id);

    for (unsigned int i = 0; i < 6; ++i)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_diffuseIrradianceShader, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }

    hdrCubeMap->diffuseIrradianceMap = irradianceMap;

    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glUseProgram(m_hdrPreFilteringShader);
    glUniformMatrix4fv(glGetUniformLocation(m_hdrPreFilteringShader, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glUniform1i(glGetUniformLocation(m_hdrPreFilteringShader, "environmentMap"), 0);


    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO->id);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureFBO->depthStencilBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        glUniform1f(glGetUniformLocation(m_hdrPreFilteringShader, "roughness"), roughness);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glUniformMatrix4fv(glGetUniformLocation(m_hdrPreFilteringShader, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }

    hdrCubeMap->prefilteredMap = prefilterMap;

    unsigned int brdfLUTTexture;
    glGenTextures(1, &brdfLUTTexture);

    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO->id);
    glBindRenderbuffer(GL_RENDERBUFFER, captureFBO->depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, 512, 512);
    glUseProgram(m_convoluteBRDFShader);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    hdrCubeMap->brdfLutTexture = brdfLUTTexture;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glDeleteFramebuffers(1, &captureFBO->id);
    glViewport(0, 0, windowWidth, windowHeight);
    return hdrCubeMap;
}

GLuint OpenglRenderer::CreateCubeMap(std::vector<std::string> faces)
{
    GLuint cubeMap;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    for (int i = 0;i < 6;i++) {
        Image* faceImage = Ressources::LoadImageFromFile(faces[i], false);
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

void OpenglRenderer::ReloadTextures(bool gammaCorrection)
{
    for (const auto& pair : m_textures) {
        if (pair.first == nullptr ||pair.first->imageType == Image::ImageType::map)
            continue;
        glBindTexture(GL_TEXTURE_2D, pair.second);
        pair.first->gammaCorrect = gammaCorrection;
        SetTextureData(GL_TEXTURE_2D, pair.first);
    }
}

intptr_t OpenglRenderer::GetUITexture(Image* image)
{
    return GetTexture(image);
}

intptr_t OpenglRenderer::GetShadowMapTexture()
{
    return m_shadowDepthFBO->colorAttachments[0];
}

intptr_t OpenglRenderer::GetSkyBox()
{
    return (intptr_t)m_skyBox->hdrTexture;
}

void OpenglRenderer::RendererSettingsTab()
{

    if (ImGui::CollapsingHeader("Pipline", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("V-Sync", &settings.VSync)) {
            if (settings.VSync)
                glfwSwapInterval(1);
            else
                glfwSwapInterval(0);
        }

        ImGui::Checkbox("Z pre-pass", &settings.zPrePass);
        const char* enumNames[] = { "None", "DepthRBO","DepthStencilRBO" ,"DepthTexture","DepthStencilTexture" };
        if (ImGui::Combo("DepthStencil type", &settings.screenFBODepthStencilType, enumNames, IM_ARRAYSIZE(enumNames))) {
            DepthStencilType type = static_cast<DepthStencilType>(settings.screenFBODepthStencilType);
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, settings.HDR, type, settings.multiSampling ? settings.samples : 0);
        }
    }

    if (ImGui::CollapsingHeader("Anti-Aliasing", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("enable multisampling", &settings.multiSampling)) {
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, settings.HDR, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
        }
        if (settings.multiSampling && ImGui::DragInt("samples", &settings.samples, 1, 1, 8)) {
            SetFrameBufferAttachements(m_screenFBO, windowWidth, windowHeight, 2, settings.HDR, m_screenFBO->depthStencilType, settings.multiSampling ? settings.samples : 0);
        }
    }

    if (ImGui::CollapsingHeader("Gamma correction", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("enable gamma correction", &settings.gammaCorrection)) {
            ReloadTextures(settings.gammaCorrection);
        }
        if (settings.gammaCorrection)
            ImGui::DragFloat("Gamma", &settings.gamma, 0.1f, 0.0f, 5.0f);
    }

    if (ImGui::CollapsingHeader("HDR", ImGuiTreeNodeFlags_DefaultOpen)) {
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

    if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("enable bloom", &settings.bloom);

        const char* enumNames[] = { "Kernel Blur", "Gaussian Blur" };
        int currentIndex = static_cast<int>(settings.bloomType);
        if (ImGui::Combo("blur type", &currentIndex, enumNames, IM_ARRAYSIZE(enumNames)))
            settings.bloomType = static_cast<RendererSettings::BloomTypes>(currentIndex);

        if (settings.bloom && settings.bloomType == RendererSettings::gaussianBlur) {
            ImGui::InputInt("amount", &settings.amount);
            settings.amount = glm::clamp(settings.amount, 1, 100);
        }
    }

    if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("enable ssao", &settings.SSAO)) {
            if (!settings.SSAO) settings.SSAOOnly = false;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Enabling Screen Space Ambient Occlusion requires the depth values of each fragment.");
        }
        if (settings.SSAO)
        {
            ImGui::Checkbox("SSAO only", &settings.SSAOOnly);
            ImGui::InputInt("kernel size", &settings.kernelSize);
            ImGui::DragFloat("sample radius", &settings.sampleRad, 0.1f, 0, 2);
            ImGui::InputFloat("power", &settings.power);
            ImGui::InputFloat("bias", &settings.bias);
        }
    }

    if (ImGui::CollapsingHeader("Shadow mapping", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("enable shadow", &settings.shadowMapping);
        if (settings.shadowMapping) {
            if (ImGui::DragInt2("shadowmap resolution", &settings.shadowResolution[0])) {
                SetFrameBufferAttachements(m_shadowDepthFBO, settings.shadowResolution.x, settings.shadowResolution.y, 1, settings.HDR, TextureDepth, 0);
            }
            ImGui::InputFloat("shadow bias", &settings.shadowbias, 0.01, 0.1, "%8f");
            ImGui::InputFloat("min bias", &settings.minBias, 0.01, 0.1, "%8f");
            settings.shadowbias = glm::clamp(settings.shadowbias, settings.minBias, settings.shadowbias + 1);
            settings.minBias = glm::clamp(settings.minBias, 0.0f, settings.minBias + 1);

            const char* enumNames[] = { "Hard shadow", "Soft shadow" };
            int currentIndex = settings.softShadow;
            if (ImGui::Combo("shadow type", &currentIndex, enumNames, IM_ARRAYSIZE(enumNames)))
                settings.softShadow = currentIndex;
            float imageWidth = ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - 30;
            ImGui::Image(((void*)GetShadowMapTexture()), ImVec2(imageWidth , imageWidth * settings.shadowResolution.y / settings.shadowResolution.x), ImVec2(1, 1), ImVec2(0, 0));
        }
    }

    if (ImGui::CollapsingHeader("PBR", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("diffuse irradiance",&settings.enableDiffuseIrradiance);
        ImGui::Checkbox("specular IBL", &settings.enableSpecularIBL);
    }
}
