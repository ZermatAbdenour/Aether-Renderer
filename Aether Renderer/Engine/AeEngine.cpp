#include "AeEngine.h"

AeEngine::AeEngine(Renderer* renderer)
{
	m_renderer = renderer;
	//initialize the renderer
	m_window = m_renderer->Init();
	
	//initialize ImGui
	m_renderer->ImGuiInit(m_window);

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

		ImGui::Begin("hello");
		ImGui::SetWindowSize(ImVec2(100, 100));
		ImGui::Text("this is cool");
		ImGui::End();

		//std::cout << FPS << std::endl;


		EndFrame();
	}
}

GLFWwindow* AeEngine::GetWindow()
{
	return m_window;
}
void AeEngine::StartFrame() {
	//ImGui new Frame
	m_renderer->ImGuiNewFrame();

	double currentTime = glfwGetTime();
	deltaTime = currentTime - time;
	time = currentTime;
	FPS = (int)(1.0 / deltaTime);
}
void AeEngine::EndFrame()
{
	m_renderer->ImGuiRender();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

AeEngine::~AeEngine()
{
	m_renderer->Clear();
	glfwDestroyWindow(m_window);
	delete m_renderer;
}
