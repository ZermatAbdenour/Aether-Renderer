#include "AeEngine.h"

AeEngine::AeEngine(Renderer* renderer)
{
	m_renderer = renderer;
	//initialize the renderer
	m_window = m_renderer->Init();
	m_editor = new Editor();
}

void AeEngine::Load(Scene* scene)
{
	//m_renderer->SetupScene(scene);
	scene->Setup();
	m_renderer->Setup(scene);
	m_editor->Setup(scene, m_renderer, &m_time);
	while (!glfwWindowShouldClose(m_window))
	{
		m_time.Update();
		scene->Update(m_window,m_editor,&m_time);
		m_renderer->Render(scene);
		m_renderer->RenderEditor(m_editor);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}


AeEngine::~AeEngine()
{
	m_renderer->Clear();
	glfwDestroyWindow(m_window);
	delete m_renderer;
	delete m_editor;
}
