#include "Material.h"

Material::Material(glm::vec3 ambient, glm::vec3 specular, int diffuseMap, float shininess)
	: m_Ambient(ambient), m_Specular(specular), m_DiffuseMap(diffuseMap), m_Shininess(shininess)
{}

Material::~Material()
{
}
