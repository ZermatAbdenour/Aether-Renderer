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
    //scene->camera.eulerAngles = glm::vec3
    aetherEngine.Load(scene);
    return 0;
}