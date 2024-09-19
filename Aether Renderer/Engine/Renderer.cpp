#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//? This two function are not tested and i feel there is something wrong
void Renderer::RenderScene(Scene* scene)
{
	for (auto rootEntity : scene->RootEntities) {
		RenderEntity(rootEntity);
		RenderChildEntities(rootEntity);
	}
}

void Renderer::RenderChildEntities(std::shared_ptr<Entity> entity) {
	for (auto childEntity : entity->Childs) {
		RenderEntity(childEntity);
		RenderChildEntities(childEntity);
	}
}
