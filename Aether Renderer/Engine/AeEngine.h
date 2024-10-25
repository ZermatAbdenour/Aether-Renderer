#pragma once
#include "Renderer.h"
#include "Scene.h"
#include "Editor.h"
#include "Time.h"

#include <Imgui/imgui.h>
class AeEngine {
public:
	AeEngine(Renderer* renderer);
	~AeEngine();
	void Load(Scene* scene);
	Time m_time;

private:
	GLFWwindow* m_window;
	Renderer* m_renderer;
	Editor* m_editor ;
};