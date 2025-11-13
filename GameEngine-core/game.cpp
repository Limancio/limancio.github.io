#include "game.h"

Game::Game(const char * title, int width, int height)
	: m_Window(new Window(title, width, height))
{
	glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
	
}

Game::~Game()
{
}

void Game::initGame()
{
	//TODO: load levels in m_Levels
	m_currentLevel = new Level(m_Window);

}

void Game::update()
{
	m_currentLevel->update();
}

void Game::draw()
{
	m_currentLevel->draw();
}

void Game::start()
{

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	double lastTime = glfwGetTime();

	while (!m_Window->closed()) {

		while (GLenum error = glGetError()) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}

		//New Frame
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		m_Window->setElapsedTime(deltaTime);

		glfwPollEvents();

		//update();
		m_currentLevel->update();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw();
		m_currentLevel->draw();

		//Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(m_Window->getWindow());

		lastTime = currentTime;
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();
}
