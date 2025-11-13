#include <iostream>
#include "src\graphics\window.h"
#include "src\graphics\Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "src\graphics\Camera.h"
#include "src\models\Mesh.h"
#include "src\graphics\Texture2D.h"
#include "src\models\Model.h"
#include "src\enviroment\Skybox.h"
#include "src\graphics\Light.h"
#include "src\input\MousePicker.h"
#include "src\enviroment\Terrain.h"

int main(int argc, char *argv[]) {

	using namespace gameEngine;
	
	Window window("M1 Project", 1024, 720);

	glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

	Shader SkyboxShader("src/shaders/skybox.vert", "src/shaders/skybox.frag");
	Skybox skybox(1000.0f);

	Shader lightingShader("src/shaders/multi_lighting.vert", "src/shaders/multi_lighting.frag");
	
	// Load meshes and textures
	const int numModels = 6;
	Model models[numModels];

	Model dirLightModel;
	Model pointLight0Model;
	Model spotLightModel;

	Model* selectedModel = nullptr;

	dirLightModel.loadModel("src/res/models/light.obj", "src/res/textures/light_diffuse.jpg", false, Material(), glm::vec3(10.0f, 1.0f, 1.0f), glm::vec3(1.0f));
	pointLight0Model.loadModel("src/res/models/light.obj", "src/res/textures/light_diffuse.jpg", false, Material(), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f));
	spotLightModel.loadModel("src/res/models/light.obj", "src/res/textures/light_diffuse.jpg", false, Material(), window.getCamera()->getPosition(), glm::vec3(1.0f));


	DirectionalLight dirLight(dirLightModel, dirLightModel.getPosition(), glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.5f, 0.5f, 0.5f));
	PointLight pointLight(pointLight0Model, pointLight0Model.getPosition(), glm::vec3(0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
	SpotLight spotLight(spotLightModel, spotLightModel.getPosition(), window.getCamera()->getLook(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));

	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	models[0].loadModel("src/res/models/crate.obj", "src/res/textures/container2.png", true, Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f),0, 32.0f),glm::vec3(-2.5f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), false);
	models[1].loadModel("src/res/models/woodcrate.obj", "src/res/textures/woodcrate_diffuse.jpg", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f), 0, 32.0f), glm::vec3(2.5f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), false);
	models[2].loadModel("src/res/models/robot.obj", "src/res/textures/robot_diffuse.jpg", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), lightColor, 0, 32.0f), glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), false);
	//models[3].loadModel("src/res/models/floor.obj", "src/res/textures/tile_floor.jpg", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), lightColor, 0, 32.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	models[3].loadModel("src/res/models/bowling_pin.obj", "src/res/textures/AMF.tga", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), lightColor, 0, 50.0f), glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.1f, 0.1f, 0.1f), false);
	models[4].loadModel("src/res/models/bunny.obj", "src/res/textures/bunny_diffuse.jpg", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), lightColor, 0, 32.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), false);
	models[5].loadModel("src/res/models/grassModel.obj", "src/res/textures/grassTexture.png", false, Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f), 0, 32.0f), glm::vec3(5.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), true);

	Mesh lightMesh;
	lightMesh.loadOBJ("src/res/models/light.obj", false);

	Terrain terrain(0, 0, "src/res/textures/test.png");
	MousePicker mousePicker((&window), (&terrain), glm::perspective(glm::radians(window.getCamera()->getFOV()), window.getWidth() / (float)window.getHeight(), 0.1f, 200.0f));

	double lastTime = glfwGetTime();
	float angle = 0.0f;

	while (!window.closed()) {

			while (GLenum error = glGetError()) {
				std::cout << "OpenGL Error: " << error << std::endl;
			}

			double currentTime = glfwGetTime();
			double deltaTime = currentTime - lastTime;

			// Poll for and process events
			glfwPollEvents();
			window.update(deltaTime);
			mousePicker.update();
			//std::cout << "Vector3f [" << mousePicker.getCurrentTerrainPoint().x << ", " << mousePicker.getCurrentTerrainPoint().y << ", " << mousePicker.getCurrentTerrainPoint().z << "]" << std::endl;

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 model(1.0), view(1.0), projection(1.0);

			// Create the View matrix
			view = window.getCamera()->getViewMatrix();

			// Create the projection matrix
			projection = glm::perspective(glm::radians(window.getCamera()->getFOV()), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 600.0f);

			glm::vec3 viewPos;
			viewPos.x = window.getCamera()->getPosition().x;
			viewPos.y = window.getCamera()->getPosition().y;
			viewPos.z = window.getCamera()->getPosition().z;

			// move the light
			//angle += (float)deltaTime * 50.0f;
			//lightPos.x = 8.0f * sinf(glm::radians(angle));

			glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
			SkyboxShader.enable();
			view[3][0] = 0.0f;
			view[3][1] = 0.0f;
			view[3][2] = 0.0f;
			SkyboxShader.setUniformMat4("view", view);
			SkyboxShader.setUniformMat4("projection", projection);
			skybox.drawSkybox();
			glDepthFunc(GL_LESS); // Set depth function back to default
			SkyboxShader.disable();

			view = window.getCamera()->getViewMatrix();
			
			lightingShader.enable();

			// Pass the matrices to the shader
			lightingShader.setUniformMat4("view", view);
			lightingShader.setUniformMat4("projection", projection);
			lightingShader.setUniform3f("viewPos", viewPos);
			lightingShader.setUniform1i("nrPointsLights", 1);

			
			// draw the scene
			for (int i = 0; i < numModels; i++)
			{
				if (selectedModel != nullptr && selectedModel->getID() == models[i].getID()) 
					models[i].setPosition(glm::vec3(mousePicker.getCurrentTerrainPoint().x, terrain.getHeightOfTerrain(mousePicker.getCurrentTerrainPoint().x, mousePicker.getCurrentTerrainPoint().z), mousePicker.getCurrentTerrainPoint().z));
					
				model = glm::translate(glm::mat4(1.0), models[i].getPosition()) * glm::scale(glm::mat4(1.0), models[i].getScale());
				
				lightingShader.setUniformMat4("model", model);
				lightingShader.setUniform1i("multiplyTexture", 1);

				lightingShader.setUniform3f("material.ambient", models[i].getMaterial().getAmbient());
				lightingShader.setUniformSampler("material.diffuse", 0);
				lightingShader.setUniformSampler("material.specular", 1); 

				if (models[i].useSpecular()) {
					lightingShader.setUniform1i("material.useSpecular", 1);
				}
				else {
					lightingShader.setUniform1i("material.useSpecular", 0);
					lightingShader.setUniform3f("material.specularVec", models[i].getMaterial().getSpecular());

				}
				lightingShader.setUniform1f("material.shininess", models[i].getMaterial().getShininess());

				models[i].draw();
			}

			/*window.getCamera()->setPosition(
				glm::vec3(window.getCamera()->getPosition().x, 
				2.0f+terrain.getHeightOfTerrain(window.getCamera()->getPosition().x, window.getCamera()->getPosition().z), 
				window.getCamera()->getPosition().z));*/
			dirLight.Draw(lightingShader, 0);
			pointLight.Draw(lightingShader, 0);

			spotLight.setPosition(window.getCamera()->getPosition());
			spotLight.setDirection(window.getCamera()->getLook());
			spotLight.Draw(lightingShader, 0);

			if (window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_1) && selectedModel == nullptr) {
				selectedModel = mousePicker.getSelectedModel(models, numModels);
				//terrain.setHeightOfTerrain(mousePicker.getCurrentTerrainPoint().x, mousePicker.getCurrentTerrainPoint().z, 0.05f, 10.0f);
			}
			else if (window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_2) && selectedModel != nullptr) {
				selectedModel = nullptr;
				//terrain.setHeightOfTerrain(mousePicker.getCurrentTerrainPoint().x, mousePicker.getCurrentTerrainPoint().z, -0.05f, 10.0f);
			}

			lightingShader.setUniform1i("multiplyTexture", 40);
			model = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
			lightingShader.setUniformMat4("model", model);
			terrain.drawTerrain();

			lightingShader.disable();

			// Swap front and back buffers
			glfwSwapBuffers(window.getWindow());

			lastTime = currentTime;
	}

	return 0;
}

