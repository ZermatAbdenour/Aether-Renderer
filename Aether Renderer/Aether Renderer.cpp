#include <iostream>
#include "Core/Window.h"
#include "Core/Renderer.h"
#include "Core/AeEngine.h"
int main()
{
    AeEngine aetherEngine = AeEngine();

    aetherEngine.Render();
    return 0;
}