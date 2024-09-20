#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


//? Like this aproach but it not tested at the moment
void Renderer::SetupScene(Scene* scene)
{
	scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
		SetupEntity(entity);
	});
}

void Renderer::RenderScene(Scene* scene)
{
	scene->ForEachEntity([this,scene](std::shared_ptr<Entity> entity) {
		entity->CalculateModel();
		RenderEntity(entity,scene->camera);
	});
}
