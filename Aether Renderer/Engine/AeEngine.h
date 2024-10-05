#pragma once
#include "Renderer.h"
#include "Scene.h"
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
	Renderer* m_renderer;
	GLFWwindow* m_window;
};