#pragma once
#include "Renderer.h"
#include "Window.h"

class AeEngine {
public:
	AeEngine();
	void Render();
	~AeEngine();
private:
	Renderer* m_renderer;
	Window* m_window;
};