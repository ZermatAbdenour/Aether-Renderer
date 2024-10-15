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
		RenderSceneTab(scene);
		RenderLightingTab(scene);
		RendererSettingsTab(renderer);
	ImGui::EndTabBar();
	float windowHeight = ImGui::GetWindowHeight();
	float textHeight = ImGui::GetTextLineHeight();
	ImGui::SetCursorPosY(windowHeight - textHeight - ImGui::GetStyle().WindowPadding.y + ImGui::GetScrollY());
	ImGui::Separator();
	ImGui::Text("deltatime : %f | FPS : %i | time : %.2f",engineTime.deltaTime, (int)averageFPS,engineTime.time);
	ImGui::End();
}

void Editor::RenderSceneTab(Scene* scene)
{
	if (!ImGui::BeginTabItem("Scene"))
		return;
	scene->RenderSceneTab();
	ImGui::EndTabItem();
}

void Editor::RenderLightingTab(Scene* scene)
{
	if (!ImGui::BeginTabItem("Lighting"))
		return;
	scene->RenderLightingTab();
	ImGui::EndTabItem();
}

void Editor::RendererSettingsTab(Renderer* renderer)
{
	if (!ImGui::BeginTabItem("Settings"))
		return;
	renderer->RendererSettingsTab();
	ImGui::EndTabItem();
}
