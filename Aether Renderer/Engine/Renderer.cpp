#include "Renderer.h"
#include <iostream>

void Renderer::Render(Scene* scene)
{
	RenderScene(scene);
	PostProcess();
}