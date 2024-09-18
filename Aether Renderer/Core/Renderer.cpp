#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


void Renderer::Render(Scene* scene)
{
	for (auto rootEntity : scene->RootEntities)
		RenderEntity(rootEntity);
}
