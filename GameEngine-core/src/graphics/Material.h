#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm\glm.hpp>

class Material
{
public:
	Material() = default;
	Material(glm::vec3 ambient, glm::vec3 specularMap, int diffuseMap, float shininess);
	~Material();

	inline glm::vec3 getAmbient() const { return m_Ambient; }
	inline glm::vec3 getSpecular() const { return m_Specular; }
	inline int getDiffuseMap() const { return m_DiffuseMap; }
	inline float getShininess() const { return m_Shininess; }

	inline void setAmbient(glm::vec3 ambient) { m_Ambient = ambient; }
	inline void setSpecular(glm::vec3 specular) { m_Specular = specular; }
	inline void setDiffuseMap(int diffuseMap) { m_DiffuseMap = diffuseMap; }
	inline void setShininess(float shininess) { m_Shininess = shininess; }

private:
	glm::vec3 m_Ambient;
	glm::vec3 m_Specular;
	int m_DiffuseMap;
	float m_Shininess;
};

#endif // !MATERIAL_H
