#include "Scene.h"
#include <Imgui/imGuIZMO.quat/imGuIZMOquat.h>
#include "Renderer.h"

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

void Scene::Setup()
{
	//Start the effectors
	for (auto effector : m_effectors) {
		effector->Start();
	}
}
void Scene::Update(GLFWwindow* window, Editor* editor, Time* time)
{
	ForEachEntity([this](std::shared_ptr<Entity> entity) {
		entity->CalculateModel();
		});
	//Update effectors
	for (auto effector : m_effectors) {
		effector->Update(time->deltaTime);
	}

	camera.Update(window, editor, time);
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
void Scene::RenderSceneTab(Renderer* renderer)
{
	ImGui::Text("Heirarchy");

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[7]);

	ImVec2 childSize = ImVec2(ImGui::GetWindowWidth(), 150);
	ImVec2 childPos = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddRectFilled(childPos,
		ImVec2(childPos.x + childSize.x, childPos.y + childSize.y),
		ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_ChildBg]),
		10);

	ImGui::BeginChild("Heirarchy", childSize,false);


	for (auto root : rootEntities) {
		RenderSceneHierarchyUI(root);
	}
	ImGui::EndChild();

	ImGui::PopStyleColor();
	ImGui::Separator();

	ImGui::Text("Properties");
	if (m_selectedEntity != nullptr) {
		ImGui::DragFloat3("Postion", &m_selectedEntity->localPosition[0],0.5f);
		ImGui::DragFloat3("EulerEngles", &m_selectedEntity->eulerAngles[0],0.5f);
		ImGui::DragFloat3("Scale", &m_selectedEntity->scale[0], 0.5f);
		MeshRenderer* meshRenderer = m_selectedEntity->meshRenderer;
		if (meshRenderer != nullptr) {
			ImGui::Text("Mesh renderer");
			Mesh* mesh = m_selectedEntity->meshRenderer->mesh;
			ImGui::Text("Vertices : %i Indices : %i",mesh->vertices.size(),mesh->indices.size());
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::Text("base color");
				ImGui::ColorEdit3("##basecolor", &meshRenderer->baseColor[0]);
				if (meshRenderer->diffuse != nullptr) {
					ImGui::Text("diffuse map");
					ImGui::Image((void*)renderer->GetUITexture(meshRenderer->diffuse), ImVec2(100, 100), ImVec2(1, 1),ImVec2(0,0));
				}
				if (meshRenderer->normalMap != nullptr) {
					ImGui::Text("normal map");
					ImGui::Image((void*)renderer->GetUITexture(meshRenderer->normalMap), ImVec2(100, 100), ImVec2(1, 1), ImVec2(0, 0));
				}

				if (meshRenderer->metalicMap != nullptr) {
					ImGui::Text(meshRenderer->packedAoRM?"Packed texture" : "Metalic map");
					if (meshRenderer->packedAoRM) 
						ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Info: Packed AO | Roughness | Metalic");
					ImGui::Image((void*)renderer->GetUITexture(meshRenderer->metalicMap), ImVec2(100, 100), ImVec2(1, 1), ImVec2(0, 0));
				}
				if (!meshRenderer->packedAoRM && meshRenderer->roughnessMap != nullptr) {
					ImGui::Text("roughness map");
					ImGui::Image((void*)renderer->GetUITexture(meshRenderer->roughnessMap), ImVec2(100, 100), ImVec2(1, 1), ImVec2(0, 0));
				}
				if (!meshRenderer->packedAoRM && meshRenderer->aoMap != nullptr) {
					ImGui::Text("AO map");
					ImGui::Image((void*)renderer->GetUITexture(meshRenderer->aoMap), ImVec2(100, 100), ImVec2(1, 1), ImVec2(0, 0));
				}
				ImGui::DragFloat("metallic", &meshRenderer->metallic, 0.05, 0, 1);
				ImGui::DragFloat("roughness", &meshRenderer->roughness, 0.05, 0, 1);
				ImGui::DragFloat("ao", &meshRenderer->ao, 0.05, 0, 1);
			}
		}
	}
}
void Scene::RenderLightingTab(Renderer* renderer)
{
	ImVec2 buttonsize = ImVec2(20,20);
	if (ImGui::CollapsingHeader("Directional Lights"), ImGuiTreeNodeFlags_DefaultOpen) {
		for (int i = 0;i < DirectionalLights.size();i++) {

			if (i == 0) {
				ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Info: Shadow map target");
			}
			ImVec2 firstPos = ImGui::GetCursorPos();
			ImVec2 buttonPos = firstPos;
			buttonPos.x += ImGui::GetWindowWidth() - buttonsize.x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing;
			ImGui::SetCursorPos(buttonPos + ImVec2(-30, 15));
			std::string special = "##d" + std::to_string(i);
			if (ImGui::Button(("-" + special).c_str(), buttonsize)) {
				DirectionalLights.erase(DirectionalLights.begin() + i);
			}
			ImGui::SetCursorPos(firstPos);
			//Get light view Direction
			ImGui::Text("direction");
			vec3 direction = camera.directionToViewSpace(DirectionalLights[i].direction);
			ImGui::gizmo3D((special + "dir").c_str(), direction);
			DirectionalLights[i].direction = camera.viewSpaceToDirection(direction);
			ImGui::Text("color");
			ImGui::ColorEdit3((special + "color").c_str(), &DirectionalLights[i].color[0]);
			ImGui::Text("intensity");
			ImGui::InputFloat((special + "intesity").c_str(), &DirectionalLights[i].intensity);
			DirectionalLights[i].intensity = glm::clamp(DirectionalLights[i].intensity, 0.0f, DirectionalLights[0].intensity + 1);
			ImGui::Separator();
		}
		ImVec2 addButtonSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing, 20);
		if (ImGui::Button("add directional light", addButtonSize)) {
			DirectionalLights.push_back(DirectionalLight());
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Point Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int i = 0;i < PointLights.size();i++) {
			ImVec2 firstPos = ImGui::GetCursorPos();
			ImVec2 buttonPos = firstPos;
			buttonPos.x += ImGui::GetWindowWidth() - buttonsize.x - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing;
			ImGui::SetCursorPos(buttonPos + ImVec2(-30, 15));
			std::string special = "##p" + std::to_string(i);
			if (ImGui::Button(("-"+special).c_str(), buttonsize)) {
				PointLights.erase(PointLights.begin() + i);
			}
			ImGui::SetCursorPos(firstPos);
			ImGui::Text("position");
			ImGui::DragFloat3((special+"position").c_str() , &PointLights[i].position[0]);
			//Get light view Direction
			ImGui::Text("color");
			ImGui::ColorEdit3((special + "color").c_str(), &PointLights[i].color[0]);
			ImGui::Text("intensity");
			ImGui::InputFloat((special + "intensity").c_str(), &PointLights[i].intensity);
			PointLights[i].intensity = glm::clamp(PointLights[i].intensity, 0.0f, PointLights[0].intensity + 1);
			ImGui::Separator();
		}
		ImVec2 addButtonSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::GetStyle().IndentSpacing, 20);
		if (ImGui::Button("add point light", addButtonSize)) {
			PointLights.push_back(PointLight());
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("SkyBox"), ImGuiTreeNodeFlags_DefaultOpen) {
		ImGui::Text("HDR texture");
		float ImageWidth = ImGui::GetWindowWidth() -ImGui::GetStyle().WindowPadding.x -30;
		if (ImGui::ImageButton((void*)renderer->GetSkyBox(), ImVec2(ImageWidth, ImageWidth *environmentMap->Height/environmentMap->Width),ImVec2(1,1),ImVec2(0,0))) {
			std::string selectedFilePath = OpenFilePicker();
			if (!selectedFilePath.empty()) {
				 environmentMap = Ressources::LoadImageFromPath(selectedFilePath,true);
				 renderer->LoadSkyBox(environmentMap);
			}
		}

		const char* enumNames[] = { "EnvirenmentMap", "DiffuseIrradiance","PrefilteringMap"};
		int targetSkyBoxMap = static_cast<int>(renderer->settings.targetSkyBoxMap);
		if (ImGui::Combo("SkyBox map", &targetSkyBoxMap, enumNames, IM_ARRAYSIZE(enumNames))) {
			renderer->settings.targetSkyBoxMap = static_cast<RendererSettings::SkyBoxMap>(targetSkyBoxMap);
		}
	}
}

void Scene::RenderSceneHierarchyUI(std::shared_ptr<Entity> entity)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags();
	if (entity->childs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (entity == m_selectedEntity)
		flags |= ImGuiTreeNodeFlags_Selected;
	bool opened = ImGui::TreeNodeExV(std::to_string(entity->id).c_str(), flags, entity->Name.c_str(), NULL);

	if (ImGui::IsItemClicked()) {
		m_selectedEntity = entity;
	}

	if (opened) {
		for (auto& child : entity->childs) {
			RenderSceneHierarchyUI(child);
		}
		ImGui::TreePop();
	}
}


void Scene::PrintSceneHeirarchy()
{
	for (const auto& rootEntity : rootEntities) {
		rootEntity->PrintEntityHierarchy(0);
	}
}