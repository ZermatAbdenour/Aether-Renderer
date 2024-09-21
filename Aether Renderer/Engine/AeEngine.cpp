#include "AeEngine.h"

AeEngine::AeEngine(Renderer* renderer)
{
	m_renderer = renderer;
	m_window = m_renderer->Init();

}

void AeEngine::Load(Scene* scene)
{
	m_renderer->Setup();
	m_renderer->SetupScene(scene);
	scene->StartEffectors();
	while (!glfwWindowShouldClose(m_window))
	{
		m_renderer->SetupFrame();
		m_renderer->RenderScene(scene);

		scene->UpdateEffectors();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

AeEngine::~AeEngine()
{
	m_renderer->Clear();
	glfwDestroyWindow(m_window);
	delete m_renderer;
}
