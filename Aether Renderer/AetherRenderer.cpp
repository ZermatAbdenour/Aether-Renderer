#include <iostream>
#include "Core/Renderer.h"
#include "Core/AeEngine.h"
#include "Core/Scene.h"
#include "Utilities/FileUtil.hpp"
#include "Core/Effectors/Move.h"
#include "OpenglRenderer.h"

int main()
{
    OpenglRenderer* renderer = new OpenglRenderer();
    AeEngine aetherEngine = AeEngine(renderer);
    
    Scene* scene = new Scene();
    auto ent = scene->AddEntity("var");
    aetherEngine.Load(scene);
    return 0;
}