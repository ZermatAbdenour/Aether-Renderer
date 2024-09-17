#include "AeEngine.h"

AeEngine::AeEngine()
{
	m_window = new Window();
	m_renderer = new Renderer();
}

void AeEngine::Render()
{
	while (!m_window->ShouldClose())
	{
		m_renderer->Clear();

		m_window->SwapBuffers();
		glfwPollEvents();
	}
}