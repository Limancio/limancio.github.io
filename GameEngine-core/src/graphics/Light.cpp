#include "Light.h"
#include <string>

DirectionalLight::DirectionalLight(const char* name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	: Light(name, position, direction, ambient, diffuse, specular)
{
}

void DirectionalLight::setupShader(Shader & shader)
{
	shader.setUniform3f("dirLight.direction", m_Direction);
	shader.setUniform3f("dirLight.ambient", m_Ambient);
	shader.setUniform3f("dirLight.diffuse", m_Diffuse);
	shader.setUniform3f("dirLight.specular", m_Specular);
}

void DirectionalLight::draw()
{

}


void Light::draw()
{
}

void Light::setupShader(Shader & shader)
{
}

Light::Light(const char* name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	: Entity(name, position), m_Direction(direction), m_Ambient(ambient), m_Diffuse(diffuse), m_Specular(specular) {}

PointLight::PointLight(const char* name, glm::vec3 position, glm::vec3 direction,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	float constant, float linear, float quadratic)
	: Light(name, position, direction, ambient, diffuse, specular),
	m_Constant(constant), m_Linear(linear), m_Quadratic(quadratic)
{
}

void PointLight::setupShader(Shader & shader, int numLight)
{
	std::string number = std::to_string(numLight);
	shader.setUniform3f(("pointLights[" + number + "].position").c_str(), m_Position);
	shader.setUniform3f(("pointLights[" + number + "].ambient").c_str(), m_Ambient);
	shader.setUniform3f(("pointLights[" + number + "].diffuse").c_str(), m_Diffuse);
	shader.setUniform3f(("pointLights[" + number + "].specular").c_str(), m_Specular);
	shader.setUniform1f(("pointLights[" + number + "].constant").c_str(), m_Constant);
	shader.setUniform1f(("pointLights[" + number + "].linear").c_str(), m_Linear);
	shader.setUniform1f(("pointLights[" + number + "].quadratic").c_str(), m_Quadratic);

}

void PointLight::draw()
{
	//m_Model.draw();
}

SpotLight::SpotLight(const char* name, glm::vec3 position, glm::vec3 direction,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant,
	float linear, float quadratic, float cutOff, float outerCutOff)
	: PointLight(name, position, direction, ambient, diffuse, specular, constant, linear, quadratic),
	m_CutOff(cutOff), m_OuterCutOff(outerCutOff) {}

void SpotLight::setupShader(Shader & shader)
{
	shader.setUniform3f("spotLight.position", m_Position);
	shader.setUniform3f("spotLight.direction", m_Direction);
	shader.setUniform3f("spotLight.ambient", m_Ambient);
	shader.setUniform3f("spotLight.diffuse", m_Diffuse);
	shader.setUniform3f("spotLight.specular", m_Specular);
	shader.setUniform1f("spotLight.constant", m_Constant);
	shader.setUniform1f("spotLight.linear", m_Linear);
	shader.setUniform1f("spotLight.quadratic", m_Quadratic);
	shader.setUniform1f("spotLight.cutOff", m_CutOff);
	shader.setUniform1f("spotLight.outerCutOff", m_OuterCutOff);
}

void SpotLight::draw()
{

	//m_Model.draw();
}
