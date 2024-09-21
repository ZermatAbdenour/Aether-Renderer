#include "Scene.h"

Scene::Scene()
{
	camera = Camera();
}

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	rootEntities.push_back(entity);
}

std::shared_ptr<Entity> Scene::AddEntity(const char* name)
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	rootEntities.push_back(entity);
	entity->Name = name;
	return entity;
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
{
	rootEntities.erase(std::remove(rootEntities.begin(), rootEntities.end(), entity), rootEntities.end());
}

void Scene::StartEffectors()
{
	for (auto effector : m_effectors) {
		effector->Start();
	}
}
void Scene::UpdateEffectors()
{
	for (auto effector : m_effectors) {
		effector->Update();
	}
}

void Scene::ForEachEntity(const std::function<void(std::shared_ptr<Entity>)>& func)
{
	// Lambda to recursively apply the function to each entity
	std::function<void(std::shared_ptr<Entity>)> applyFunc = [&](std::shared_ptr<Entity> entity) {
		func(entity); 
		for (const auto& child : entity->childs) { 
			applyFunc(child);
		}
	};

	// Start with the root entities
	for (const auto& rootEntity : rootEntities) {
		applyFunc(rootEntity);
	}
}

void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : rootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}