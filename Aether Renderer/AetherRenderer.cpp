#include <iostream>
#include "Core/Window.h"
#include "Core/Renderer.h"
#include "Core/AeEngine.h"
#include "Core/Scene.h"
#include "Utilities/FileUtil.hpp"
#include "Core/Effectors/Move.h"

int main()
{
    AeEngine aetherRenderer = AeEngine();
    
    Scene* scene = new Scene();
    auto ent = scene->AddEntity("var");
    scene->AddEffector<Move>(ent);

    aetherRenderer.Load(scene);
    return 0;
}