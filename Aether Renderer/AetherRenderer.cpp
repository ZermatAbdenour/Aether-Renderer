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
    auto ent = scene->AddEntity("var");
    scene->camera.position = glm::vec3(0, 0, 10);
    scene->camera.eulerAngles = glm::vec3(0, 10, 10);
    //scene->camera.eulerAngles = glm::vec3
    aetherEngine.Load(scene);
    return 0;
}