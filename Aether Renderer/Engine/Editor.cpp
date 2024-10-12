#include "Editor.h"
#include <Imgui/imgui.h>
#include <glm/gtc/quaternion.hpp>
void Editor::UpdateAverageFPS(float deltaTime)
{
	float currentFPS = 1.0f / deltaTime;
	if (averageFPS == 0) {
		averageFPS = currentFPS;;
	}
	else
	averageFPS = glm::mix(averageFPS, currentFPS, deltaTime );
}
void Editor::CreateEditorWindow(Scene* scene, Renderer* renderer,Time& engineTime)
{
	ImGui::Begin("Editor", nullptr );

	UpdateAverageFPS(engineTime.deltaTime );
	ImGui::BeginTabBar("EditorTabBar");
		SceneTab();
		RendererSettingsTab(renderer);
	ImGui::EndTabBar();
	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetStyle().ItemSpacing.y - 20);
	ImGui::Separator();
	ImGui::Text("deltatime : %f | FPS : %i | time : %.2f",engineTime.deltaTime, (int)averageFPS,engineTime.time);
	ImGui::End();
}

void Editor::SceneTab()
{
	if (!ImGui::BeginTabItem("Scene"))
		return;
	ImGui::Text("Here you can update the scene");
	ImGui::EndTabItem();
}

void Editor::RendererSettingsTab(Renderer* renderer)
{
	if (!ImGui::BeginTabItem("Settings"))
		return;
	renderer->RendererSettingsTab();
	ImGui::EndTabItem();
}
