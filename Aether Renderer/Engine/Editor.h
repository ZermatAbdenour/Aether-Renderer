#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Time.h"
class Editor
{
public:
	Editor() = default;
	void CreateEditorWindow(Scene* scene,Renderer* renderer,Time& time);
	int averageFPS = 0;
	void SceneTab();
	void RendererSettingsTab(Renderer* renderer);
};