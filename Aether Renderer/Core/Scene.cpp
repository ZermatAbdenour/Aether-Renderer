#include "Scene.h"

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	RootEntities.push_back(entity);
}

std::shared_ptr<Entity> Scene::AddEntity(const char* name)
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	RootEntities.push_back(entity);
	entity->Name = name;
	return entity;
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
{
	RootEntities.erase(std::remove(RootEntities.begin(), RootEntities.end(), entity), RootEntities.end());
}

void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : RootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}