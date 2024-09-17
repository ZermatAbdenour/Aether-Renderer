#include "AeEngine.h"

AeEngine::AeEngine()
{
	m_window = new Window();
	m_renderer = new Renderer();
}

void AeEngine::Load(Scene* scene)
{
	while (!m_window->ShouldClose())
	{
		m_renderer->Clear();
		m_renderer->Render(scene);

		m_window->SwapBuffers();
		glfwPollEvents();
	}
}

AeEngine::~AeEngine()
{
	delete m_window;
	delete m_renderer;
}
