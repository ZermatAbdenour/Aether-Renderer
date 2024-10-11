#include "Editor.h"
#include <Imgui/imgui.h>
#include <glm/gtc/quaternion.hpp>
void Editor::CreateEditorWindow(Scene* scene, Renderer* renderer,Time& engineTime)
{
	ImGui::Begin("Editor", nullptr );

	ImGui::BeginTabBar("EditorTabBar");
		SceneTab();
		RendererSettingsTab(renderer);
	ImGui::EndTabBar();
	ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetStyle().ItemSpacing.y - 20);
	ImGui::Separator();
	if (averageFPS == 0)//For initialisation
		averageFPS = engineTime.FPS;
	else
		averageFPS = glm::mix(averageFPS,engineTime.FPS,engineTime.deltaTime * 2);
	ImGui::Text("deltatime : %f | FPS : %i | time : %.2f",engineTime.deltaTime, averageFPS,engineTime.time);
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
