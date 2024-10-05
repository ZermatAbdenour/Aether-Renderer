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
    auto ent = scene->AddEntity(Ressources::LoadModelFromFile("glTF-Sample-Models/2.0/FlightHelmet/glTF/FlightHelmet.gltf"));
    ent->scale = glm::vec3(1, 1, 1);
    DirectionalLight dirlight = DirectionalLight{
        glm::vec3(0,-0.5,-1),
        glm::vec4(10,10,10,0)
    };
    scene->DirectionalLights.push_back(dirlight);
    PointLight light = PointLight{
        glm::vec3(1),
        glm::vec3(1),
        glm::vec4(1,0,0,0)
    };
    scene->PointLights.push_back(light);
    aetherEngine.Load(scene);
    return 0;
}