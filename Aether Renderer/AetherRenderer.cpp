#include <iostream>
#include "Engine/Renderer.h"
#include "Engine/AeEngine.h"
#include "Engine/Scene.h"
#include "Utilities/FileUtil.hpp"
#include "Engine/Effectors/Move.h"
#include "Renderers/OpenglRenderer.h"

int main()
{
   OpenglRenderer* renderer = new OpenglRenderer();
   AeEngine aetherEngine = AeEngine(renderer);

    Scene* scene = new Scene();
    //auto ent = scene->AddEntity(Ressources::LoadModelFromFile("glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf"));
    //auto ent = scene->AddEntity(Ressources::LoadModelFromFile("sphere/sphere.glb"));
    auto ent = scene->AddEntity(Ressources::LoadModelFromFile("glTF-Sample-Models/2.0/ABeautifulGame/glTF/ABeautifulGame.gltf"));
    //auto ent = scene->AddEntity(Ressources::LoadModelFromFile("glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf"));
    //auto ent = scene->AddEntity(Ressources::LoadModelFromFile("sponza/sponza.obj"));
    ent->scale = glm::vec3(1, 1, 1);

    scene->environmentMap = Ressources::LoadImageFromFile("HDR/church_interior_4k.hdr",true);

    DirectionalLight dirlight = DirectionalLight{
        glm::vec3(0,-0.5,-1),
        glm::vec4(1,1,1,0),
        1
    };
    PointLight pointLight = PointLight{
        glm::vec3(-2,2,0),
        glm::vec4(1,1,1,0),
        1
    };
    scene->DirectionalLights.push_back(dirlight);
    scene->PointLights.push_back(pointLight);
    aetherEngine.Load(scene);
    return 0;
}