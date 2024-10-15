#include "Scene.h"
#include <Imgui/imGuIZMO.quat/imGuIZMOquat.h>

Scene::Scene()
{
	camera = Camera();
}

std::shared_ptr<Entity> Scene::AddEntity(std::shared_ptr<Entity> entity)
{
	rootEntities.push_back(entity);
	return entity;
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
void Scene::UpdateEffectors(float deltaTime)
{
	for (auto effector : m_effectors) {
		effector->Update(deltaTime);
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
void Scene::RenderSceneTab()
{
	if (ImGui::CollapsingHeader("Lights")) {
		ImGui::BeginListBox("Directional Lights");
		for (int i = 0;i < DirectionalLights.size();i++ ) {

			//Get light view Direction
			glm::vec4 dir = glm::vec4(DirectionalLights[i].direction.x, DirectionalLights[i].direction.y, DirectionalLights[i].direction.z,1);
			dir =  dir * glm::inverse(camera.View());
			vec3 direction = vec3(dir.x, dir.y, dir.z);

			ImGui::gizmo3D("##Dir1", direction);
			
			//Set light direction
			dir = glm::vec4(direction.x, direction.y, direction.z, 1);
			dir =  dir * camera.View();
			DirectionalLights[i].direction = glm::vec3(dir.x, dir.y, dir.z);


			ImGui::ColorPicker3("color", &DirectionalLights[0].color[0]);
		}
		ImGui::EndListBox();

		ImGui::BeginListBox("Point Lights");
		for (int i = 0;i < DirectionalLights.size();i++) {
			ImGui::DragFloat3("position", &PointLights[0].direction[0]);
			ImGui::ColorPicker3("color", &PointLights[0].color[0]);
		}
		ImGui::EndListBox();
	}
}

void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : rootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}