#include "AeEngine.h"

AeEngine::AeEngine(Renderer* renderer)
{
	m_renderer = renderer;
	m_window = m_renderer->Init();

}

void AeEngine::Load(Scene* scene)
{
	m_renderer->Setup(scene);
	scene->StartEffectors();
	while (!glfwWindowShouldClose(m_window))
	{
		m_renderer->FrameSetup();
		m_renderer->RenderScene(scene);

		scene->UpdateEffectors();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

AeEngine::~AeEngine()
{
	m_renderer->Clear();
	delete m_window;
	delete m_renderer;
}
