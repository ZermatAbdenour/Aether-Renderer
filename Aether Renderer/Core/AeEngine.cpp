#include "AeEngine.h"

AeEngine::AeEngine()
{
	m_window = new Window();
	m_renderer = new Renderer();
}

void AeEngine::Load(Scene* scene)
{
	scene->StartEffectors();
	while (!m_window->ShouldClose())
	{
		m_renderer->Clear();
		m_renderer->Render(scene);

		scene->UpdateEffectors();

		m_window->SwapBuffers();
		glfwPollEvents();
	}
}

AeEngine::~AeEngine()
{
	delete m_window;
	delete m_renderer;
}
