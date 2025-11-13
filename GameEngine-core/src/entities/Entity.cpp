#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>

Entity::Entity(const char * name, glm::vec3 position, const char* objFileName, const char* textFileName,
	Material material, glm::vec3 scale, glm::vec3 rotation, bool useSpecular, bool useFakeLight)
	: m_Name(name), m_Position(position), m_Model()
{
	m_Model.loadModel(objFileName, textFileName, material, scale, rotation, useSpecular, useFakeLight);

}

Entity::Entity(const char * name, glm::vec3 position)
	: m_Name(name), m_Position(position)
{
}

Entity::~Entity()
{
}

void Entity::setupShader(Shader & shader)
{
	glm::mat4 modelMatrix(1.0);
	glm::vec3 scale = m_Model.getScale();
	modelMatrix = glm::translate(glm::mat4(1.0), m_Position) * glm::scale(glm::mat4(1.0), glm::vec3(1.0));

	shader.setUniformMat4("model", modelMatrix);
	shader.setUniform1i("multiplyTexture", 1);

	shader.setUniform3f("material.ambient", m_Model.getMaterial().getAmbient());
	shader.setUniformSampler("material.diffuse", 0);
	shader.setUniformSampler("material.specular", 1);
	shader.setUniform1f("material.shininess", m_Model.getMaterial().getShininess());

	if (m_Model.useSpecular()) {
		shader.setUniform1i("material.useSpecular", 1);
	}
	else {
		shader.setUniform1i("material.useSpecular", 0);
		shader.setUniform3f("material.specularVec", m_Model.getMaterial().getSpecular());
	}

}

void Entity::draw()
{
	
	m_Model.draw();
}
