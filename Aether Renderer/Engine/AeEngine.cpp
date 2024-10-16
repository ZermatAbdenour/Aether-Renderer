#include "AeEngine.h"

AeEngine::AeEngine(Renderer* renderer)
{
	m_renderer = renderer;
	//initialize the renderer
	m_window = m_renderer->Init();
}

void AeEngine::Load(Scene* scene)
{
	m_renderer->SetupScene(scene);
	scene->StartEffectors();
	while (!glfwWindowShouldClose(m_window))
	{
		m_time.UpdateTime();

		scene->camera.Update(m_window, m_time.deltaTime, !m_editor.uiInteracting);
		m_renderer->SetupFrame();
		m_renderer->RenderScene();
	
		scene->UpdateEffectors(m_time.deltaTime);
		m_editor.CreateEditorWindow(scene,m_renderer,m_time);

		m_renderer->EndFrame();

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

GLFWwindow* AeEngine::GetWindow()
{
	return m_window;
}

AeEngine::~AeEngine()
{
	m_renderer->Clear();
	glfwDestroyWindow(m_window);
	delete m_renderer;
}
