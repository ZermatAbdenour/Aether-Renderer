#pragma once
#include "Renderer.h"
#include "Scene.h"

class Editor
{
public:
	Editor() = default;
	void AddEditorWindow(Scene* scene,Renderer* renderer);

	void SceneTab();
	void RendererSettingsTab(Renderer* renderer);
};