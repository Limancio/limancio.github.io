#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <GL\glew.h>
#include <stb_image\stb_image.h>
#include "..\graphics\Shader.h"



class Skybox {
private:
	void bindShader(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor);
	void unbindShader();
public:
	Skybox(const float& SIZE, const char* shaderVSFileName, const char* shaderFSFileName, std::vector<std::string> faces);
	~Skybox();
	unsigned int loadCubemap();
	void drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor);
private:

	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;

	GLuint m_CubemapTexture;
	Shader m_SkyboxShader;
	std::vector<std::string> m_TextureFaces/*{
		"src/res/textures/skybox/right.tga",
		"src/res/textures/skybox/left.tga",
		"src/res/textures/skybox/top.tga",
		"src/res/textures/skybox/bottom.tga",
		"src/res/textures/skybox/back.tga",
		"src/res/textures/skybox/front.tga"
	}*/;
	float m_Rotation;

};

#endif // SKYBOX_H