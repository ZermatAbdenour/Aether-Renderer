#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Editor.h"
#include <Imgui/imgui.h>
class AeEngine {
public:
	AeEngine(Renderer* renderer);
	~AeEngine();
	void Load(Scene* scene);
	GLFWwindow* GetWindow();
	void StartFrame();
	void EndFrame();
	//Time
	double time;
	double deltaTime;
	int FPS;

private:
	GLFWwindow* m_window;
	Renderer* m_renderer;
	Editor m_editor;
};