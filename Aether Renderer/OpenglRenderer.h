#pragma once
#include "Core/Renderer.h"

class OpenglRenderer:public Renderer
{
public:
	GLFWwindow* Init() override;
	void Setup() override;
	void FrameSetup() override;
	void RenderEntity(std::shared_ptr<Entity> entity) override;
	void Clear() override;
};