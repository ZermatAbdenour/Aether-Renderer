#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Time.h"
class Editor
{
public:
	static const int MAX_FRAMES = 2000;
	std::vector<float> frameTimes;
	int frameCount = 0;
	float totalFrameTime = 0.0f;
	float averageFPS = 0;
	Editor() = default;
	void UpdateAverageFPS(float deltaTime);
	void CreateEditorWindow(Scene* scene,Renderer* renderer,Time& time);
	void RenderSceneTab(Scene* scene);
	void RendererSettingsTab(Renderer* renderer);
};