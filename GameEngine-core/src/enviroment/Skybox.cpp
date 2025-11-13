#include "Skybox.h"
#include "../graphics/Window.h"
#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox(const float& SIZE, const char* shaderVSFileName, const char* shaderFSFileName, std::vector<std::string> faces)
	: m_TextureFaces(faces), m_SkyboxShader(shaderVSFileName, shaderFSFileName), m_Rotation(0.0f)
{
	m_CubemapTexture = loadCubemap();

	float cubeMapVertices[108]{
		-SIZE,  SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		-SIZE,  SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE
	};



	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeMapVertices), &cubeMapVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glBindVertexArray(0);

}

Skybox::~Skybox()
{
	glDeleteTextures(1, &m_CubemapTexture);
}

unsigned int Skybox::loadCubemap()
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int imageWidth, imageHeight, nrChannels;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (GLuint i = 0; i < m_TextureFaces.size(); i++)
	{
		unsigned char *data = stbi_load(m_TextureFaces[i].c_str(), &imageWidth, &imageHeight, &nrChannels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void Skybox::bindShader(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor)
{
	viewMatrix[3][0] = 0.0f;
	viewMatrix[3][1] = 0.0f;
	viewMatrix[3][2] = 0.0f;
	m_SkyboxShader.enable();
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	m_Rotation += 1.0f * Window::getElapsedTime();
	viewMatrix = glm::rotate(viewMatrix, glm::radians(m_Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	m_SkyboxShader.setUniformMat4("view", viewMatrix);
	m_SkyboxShader.setUniformMat4("projection", projectionMatrix);
	m_SkyboxShader.setUniform3f("fogColour", skyColor);
}

void Skybox::unbindShader()
{
	glDepthFunc(GL_LESS); // Set depth function back to default
	m_SkyboxShader.disable();
}

void Skybox::drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 skyColor)
{
	bindShader(viewMatrix, projectionMatrix, skyColor);

	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	unbindShader();
}
