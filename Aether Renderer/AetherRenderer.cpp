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
    auto ent = scene->AddEntity(Ressources::LoadModelFromFile("backpack/backpack.obj"));
    ent->localPosition = glm::vec3(5, 0, 0);
    ent->scale = glm::vec3(1, 1, 1);
    //scene->camera.eulerAngles = glm::vec3
    aetherEngine.Load(scene);
    return 0;
}