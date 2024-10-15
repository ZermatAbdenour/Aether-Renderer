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
	ImVec2 buttonsize = ImVec2(20,20);
	if (ImGui::CollapsingHeader("Lights",ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Indent();
		if (ImGui::CollapsingHeader("Directional Lights")) {
			for (int i = 0;i < DirectionalLights.size();i++) {
				ImVec2 firstPos = ImGui::GetCursorPos();
				ImVec2 buttonPos = firstPos;
				buttonPos.x += ImGui::GetWindowWidth() - buttonsize.x- ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing;
				ImGui::SetCursorPos(buttonPos+ImVec2(-30,15));
				std::string special = "-##d" + std::to_string(i);
				if (ImGui::Button(special.c_str(), buttonsize)) {
					DirectionalLights.erase(DirectionalLights.begin() + i);
				}
				ImGui::SetCursorPos(firstPos);
				//Get light view Direction
				ImGui::Text("direction");
				vec3 direction = camera.directionToViewSpace(DirectionalLights[i].direction);
				ImGui::gizmo3D("##ddir" + i, direction);
				DirectionalLights[i].direction = camera.viewSpaceToDirection(direction);
				ImGui::Text("color");
				ImGui::ColorEdit3("##dcolor" + i, &DirectionalLights[0].color[0]);
				ImGui::Text("intensity");
				ImGui::InputFloat("##dintensity" + i, &DirectionalLights[0].intensity);
				DirectionalLights[0].intensity = glm::clamp(DirectionalLights[0].intensity, 0.0f, DirectionalLights[0].intensity + 1);
				ImGui::Separator();
			}
			ImVec2 addButtonSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing, 20);
			if (ImGui::Button("add directional light", addButtonSize)) {
				DirectionalLights.push_back(DirectionalLight());
			}
		}

		ImGui::Spacing();
		ImGui::Unindent();
		ImGui::Separator();
		ImGui::Indent();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Point Lights")) {
			for (int i = 0;i < PointLights.size();i++) {
				ImVec2 firstPos = ImGui::GetCursorPos();
				ImVec2 buttonPos = firstPos;
				buttonPos.x += ImGui::GetWindowWidth() - buttonsize.x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing;
				ImGui::SetCursorPos(buttonPos + ImVec2(-30, 15));
				std::string special = "-##p" + std::to_string(i);
				if (ImGui::Button(special.c_str(), buttonsize)) {
					PointLights.erase(PointLights.begin() + i);
				}
				ImGui::SetCursorPos(firstPos);
				//Get light view Direction
				ImGui::Text("color");
				ImGui::ColorEdit3("##pcolor" + i, &PointLights[0].color[0]);
				ImGui::Text("intensity");
				ImGui::InputFloat("##pintensity" + i, &PointLights[0].intensity);
				PointLights[0].intensity = glm::clamp(PointLights[0].intensity, 0.0f, PointLights[0].intensity + 1);
				ImGui::Separator();
			}
			ImVec2 addButtonSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing, 20);
			if (ImGui::Button("add point light", addButtonSize)) {
				PointLights.push_back(PointLight());
			}
		}
	}
}

void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : rootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}