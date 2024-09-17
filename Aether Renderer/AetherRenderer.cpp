#include <iostream>
#include "Core/Window.h"
#include "Core/Renderer.h"
#include "Core/AeEngine.h"
#include "Core/Scene.h"
#include "Utilities/FileUtil.hpp"
int main()
{
    AeEngine aetherRenderer = AeEngine();
    
    Scene* scene = new Scene();
    auto ent = scene->AddEntity("var");
    aetherRenderer.Load(scene);

    return 0;
}