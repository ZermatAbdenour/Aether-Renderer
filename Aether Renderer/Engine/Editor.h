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
	Renderer* m_renderer;
	Scene* m_scene;
	Time* m_time;

	Editor() = default;
	void Setup(Scene* scene, Renderer* renderer, Time* time);
	void UpdateAverageFPS(float deltaTime);
	void EditorStyle();
	void Update();
	void RenderSceneTab();
	void RenderLightingTab();
	void RendererSettingsTab();
	bool uiInteracting;
};