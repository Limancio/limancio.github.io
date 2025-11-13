#include "Level.h"

#include "..\..\src\vendor\imgui\imgui.h"
#include "..\..\src\vendor\imgui\imgui_impl_glfw.h"
#include "..\..\src\vendor\imgui\imgui_impl_opengl3.h"
#include "..\entities\Entity.h"
#include "..\entities\Car.h"
#include <glm/gtc/matrix_transform.hpp>

Level::Level(Window * window)
{
	std::vector<std::string> skyboxFaces{
		"src/res/textures/skybox/right.png",
		"src/res/textures/skybox/left.png",
		"src/res/textures/skybox/top.png",
		"src/res/textures/skybox/bottom.png",
		"src/res/textures/skybox/back.png",
		"src/res/textures/skybox/front.png"
	};
	m_Window = window;
	m_Skybox = new Skybox(100.0f, "src/shaders/skybox.vert", "src/shaders/skybox.frag", skyboxFaces);

	m_basicShader = new Shader("src/shaders/multi_lighting.vert", "src/shaders/multi_lighting.frag");

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	m_Player = new Player("player", glm::vec3(0.0f, 1.0f, 0.0f), m_Window);

	DirectionalLight* entityDirLight;
	SpotLight* entitySpotLight;
	PointLight* entityPointLight;

	entityDirLight = new DirectionalLight("dir Light", glm::vec3(10.0f, 1.0f, 0.0f), glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.5f, 0.5f, 0.5f));
	entitySpotLight = new SpotLight("spot Light", m_Player->getCamera()->getPosition(), m_Player->getCamera()->getLook(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));
	entityPointLight = new PointLight("point light", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);

	Car* car = new Car("camaro", glm::vec3(1.0f, 1.0f, 1.0f), "src/res/models/car.obj", "src/res/textures/car_diffuse.jpg", Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f), 0, 32.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), true, false);
	//Entity* entityCrate = new Entity("CRATE 2", glm::vec3(1.0f, 1.0f, 1.0f), "src/res/models/crate.obj", "src/res/textures/container2.png", Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f), 0, 32.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), true, false);

	m_Entities.push_back(car);
	//m_Entities.push_back(entityCrate2);

	m_DirLights.push_back(entityDirLight);

	m_SpotLights.push_back(entitySpotLight);

	m_PointLight.push_back(entityPointLight);

	m_Terrain = new Terrain(0, 0, "src/shaders/multi_lighting_texture_terrain.vert", "src/shaders/multi_lighting_texture_terrain.frag");
	
	m_MousePicker = new MousePicker(m_Window, m_Terrain, m_Player->getCamera());

}

Level::~Level()
{
}

void Level::update()
{
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL Error: " << error << std::endl;
	}
	m_Player->update();
	m_Player->playerMovement(*m_Terrain);
	m_MousePicker->update();



	static float force = 0.1f;
	static float radius = 10.0f;

	{

		static int counter = 0;

		ImGui::Begin("Terrain Editor");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Object selected");               // Display some text (you can use a format strings too)
		if (m_MousePicker->getSelectedEntity() != nullptr) {
			glm::vec3 newPosition(m_MousePicker->getSelectedEntity()->getPosition());
			ImGui::SliderFloat3("Transformation", &newPosition.x, -10.0f, 0.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			m_MousePicker->getSelectedEntity()->setPosition(newPosition);
		}
		else {
			ImGui::Text("Null");
		}
		ImGui::SliderFloat("Force", &force, 0.0f, 0.1f);
		ImGui::SliderFloat("Radius", &radius, 0.0f, 25.0f);

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	if (m_Window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {
		m_MousePicker->checkSelectedEntity(m_Entities);
		
		m_Terrain->setHeightOfTerrain(m_MousePicker->getCurrentTerrainPoint().x, m_MousePicker->getCurrentTerrainPoint().z, force, radius);
	}
	else if (m_Window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
		//m_MousePicker->checkSelectedEntity(m_Entities);
		m_Terrain->setHeightOfTerrain(m_MousePicker->getCurrentTerrainPoint().x, m_MousePicker->getCurrentTerrainPoint().z, -force, radius);
		m_Terrain->setHeightOfTerrain(m_MousePicker->getCurrentTerrainPoint().x, m_MousePicker->getCurrentTerrainPoint().z, -force, radius);
	} else if (m_Window->isMouseButtonPressed(GLFW_MOUSE_BUTTON_3)) {
		/*if (m_MousePicker->getSelectedEntity() != nullptr) {
			m_MousePicker->getSelectedEntity()->setPosition(m_MousePicker->getCurrentTerrainPoint());
		}*/
		m_Terrain->smoothHeightOfTerrain(m_MousePicker->getCurrentTerrainPoint().x, m_MousePicker->getCurrentTerrainPoint().z, force, radius);
	}
	
	/*if (m_MousePicker->getSelectedEntity() != nullptr) {
		if (m_Window->isKeyPressed(GLFW_KEY_UP)) {
			m_MousePicker->getSelectedEntity()->increasePosition(glm::vec3(0.0f, 0.0f, -0.1f));
		} else if (m_Window->isKeyPressed(GLFW_KEY_DOWN)) {
			m_MousePicker->getSelectedEntity()->increasePosition(glm::vec3(0.0f, 0.0f, 0.1f));
		}
		if (m_Window->isKeyPressed(GLFW_KEY_UP))
			((Car*)m_MousePicker->getSelectedEntity())->move(((Car*)m_MousePicker->getSelectedEntity())->getCurrentSpeed() * m_Window->getElapsedTime() * ((Car*)m_MousePicker->getSelectedEntity())->getLook());
		else if (m_Window->isKeyPressed(GLFW_KEY_DOWN))
			((Car*)m_MousePicker->getSelectedEntity())->move(((Car*)m_MousePicker->getSelectedEntity())->getCurrentSpeed() * m_Window->getElapsedTime() * -((Car*)m_MousePicker->getSelectedEntity())->getLook());

		// Strafe left/right
		if (m_Window->isKeyPressed(GLFW_KEY_LEFT))
			((Car*)m_MousePicker->getSelectedEntity())->move(((Car*)m_MousePicker->getSelectedEntity())->getCurrentSpeed() * m_Window->getElapsedTime() * -((Car*)m_MousePicker->getSelectedEntity())->getRight());
		else if (m_Window->isKeyPressed(GLFW_KEY_RIGHT))
			((Car*)m_MousePicker->getSelectedEntity())->move(((Car*)m_MousePicker->getSelectedEntity())->getCurrentSpeed() * m_Window->getElapsedTime() * ((Car*)m_MousePicker->getSelectedEntity())->getRight());
	}*/

}

void Level::draw()
{
	glm::mat4 view(1.0), projection(1.0);

	projection = glm::perspective(glm::radians(m_Player->getCamera()->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 600.0f);
	view = m_Player->getCamera()->getViewMatrix();

	glm::vec3 viewPos;
	viewPos.x = m_Player->getCamera()->getPosition().x;
	viewPos.y = m_Player->getCamera()->getPosition().y;
	viewPos.z = m_Player->getCamera()->getPosition().z;

	glm::vec3 SkyColor = glm::vec3(0.544f, 0.62f, 0.69f);

	m_Skybox->drawSkybox(view, projection, SkyColor);

	m_basicShader->enable();

	m_basicShader->setUniformMat4("view", view);
	m_basicShader->setUniformMat4("projection", projection);
	m_basicShader->setUniform3f("viewPos", viewPos);
	m_basicShader->setUniform1i("nrPointsLights", 1);

	m_basicShader->setUniform3f("skyColour", SkyColor);

	for (int i = 0; i < m_DirLights.size(); i++) {
		m_DirLights[i]->setupShader(*m_basicShader);
		m_DirLights[i]->draw();
	}
	for (int i = 0; i < m_SpotLights.size(); i++) {
		m_SpotLights[i]->setupShader(*m_basicShader);
		m_SpotLights[i]->draw();
	}
	for (int i = 0; i < m_PointLight.size(); i++) {
		m_PointLight[i]->setupShader(*m_basicShader, i);
		m_PointLight[i]->draw();
	}
	for (int i = 0; i < m_Entities.size(); i++) {
		m_Entities[i]->setupShader(*m_basicShader);
		m_Entities[i]->draw();
	}

	m_basicShader->disable();

	
	m_Terrain->getShader()->enable();

	m_Terrain->getShader()->setUniformMat4("view", view);
	m_Terrain->getShader()->setUniformMat4("projection", projection);
	m_Terrain->getShader()->setUniform3f("viewPos", viewPos);
	m_Terrain->getShader()->setUniform1i("nrPointsLights", 1);
	m_Terrain->getShader()->setUniform1i("nrPointsLights", 1);

	m_Terrain->getShader()->setUniform3f("skyColour", SkyColor);

	for (int i = 0; i < m_DirLights.size(); i++) {
		m_DirLights[i]->setupShader(*m_Terrain->getShader());
	}
	for (int i = 0; i < m_SpotLights.size(); i++) {
		m_SpotLights[i]->setupShader(*m_Terrain->getShader());
	}
	for (int i = 0; i < m_PointLight.size(); i++) {
		m_PointLight[i]->setupShader(*m_Terrain->getShader(), i);
	}
	for (int i = 0; i < m_Entities.size(); i++) {
		m_Entities[i]->setupShader(*m_Terrain->getShader());
	}
	m_Terrain->getShader()->disable();

	m_Terrain->drawTerrain(view, projection, viewPos);
	/*m_DirLight.draw(m_basicShader);
	m_SpotLight.draw(m_basicShader);
	m_PointLight.draw(m_basicShader, 0);
	m_Entitie.draw(m_basicShader);*/

	//m_basicShader->setUniform1i("multiplyTexture", 40);
	//glm::mat4 model(1.0);
	//model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
	//m_basicShader->setUniformMat4("model", model);

}
