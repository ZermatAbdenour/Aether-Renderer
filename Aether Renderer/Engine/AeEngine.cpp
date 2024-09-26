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
		StartFrame();

		scene->camera.ProcessInputs(m_window,deltaTime);
		m_renderer->SetupFrame();
		m_renderer->RenderScene();
	
		scene->UpdateEffectors(deltaTime);
		m_renderer->EndFrame();

		EndFrame();
	}
}

GLFWwindow* AeEngine::GetWindow()
{
	return m_window;
}
void AeEngine::StartFrame() {
	double currentTime = glfwGetTime();
	deltaTime = currentTime - time;
	time = currentTime;
	FPS = (int)(1.0 / deltaTime);
}
void AeEngine::EndFrame()
{
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

AeEngine::~AeEngine()
{
	m_renderer->Clear();
	glfwDestroyWindow(m_window);
	delete m_renderer;
}
