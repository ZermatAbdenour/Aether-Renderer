#pragma once
#include "Scene.h"
class Renderer {
public:
	Renderer();
	void Clear();
	void Render(Scene* scene);
private:
};