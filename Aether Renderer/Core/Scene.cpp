#include "Scene.h"

void Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	m_rootEntities.push_back(entity);
}

std::shared_ptr<Entity> Scene::AddEntity(const char* name)
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	m_rootEntities.push_back(entity);
	entity->Name = name;
	return entity;
}

void Scene::RemoveEntity(std::shared_ptr<Entity> entity)
{
	m_rootEntities.erase(std::remove(m_rootEntities.begin(), m_rootEntities.end(), entity), m_rootEntities.end());
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

void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : m_rootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}