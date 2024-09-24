#include "Renderer.h"
#include <iostream>

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
		if (!entity->meshRenderer)
			return;
		RenderEntity(entity->meshRenderer,entity->model,scene->camera);
	});
}
