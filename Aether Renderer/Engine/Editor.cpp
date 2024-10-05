#include "Editor.h"
#include <Imgui/imgui.h>

void Editor::AddEditorWindow(Scene* scene, Renderer* renderer)
{
	ImGui::Begin("Editor", nullptr );

	ImGui::BeginTabBar("EditorTabBar");
		SceneTab();
		RendererSettingsTab(renderer);
	ImGui::EndTabBar();

	ImGui::Dummy(ImVec2(0, 20));
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
