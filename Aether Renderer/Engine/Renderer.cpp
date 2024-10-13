#include "Renderer.h"
#include <iostream>

void Renderer::SetupScene(Scene* scene)
{
	m_currentScene = scene;
	scene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
		SetupEntity(entity);
	});
}

void Renderer::RenderScene()
{
	m_currentScene->ForEachEntity([this](std::shared_ptr<Entity> entity) {
		entity->CalculateModel();
	});

	RenderFrame();
}
