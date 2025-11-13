#ifndef LIGHT_H
#define LIGHT_H

#include <glm\glm.hpp>
#include "..\models\Model.h"
#include "..\entities\Entity.h"
#include "Shader.h";

class Light : public Entity {
public:

	Light() = default;
	inline glm::vec3 getDirection() const { return m_Direction; }
	inline glm::vec3 getAmbient() const { return m_Ambient; }
	inline glm::vec3 getDiffuse() const { return m_Diffuse; }
	inline glm::vec3 getSpecular() const { return m_Specular; }

	inline void setDirection(glm::vec3 direction) { m_Direction = direction; }
	inline void setAmbient(glm::vec3 ambient) { m_Ambient = ambient; }
	inline void setDiffuse(glm::vec3 diffuse) { m_Diffuse = diffuse; }
	inline void setSpecular(glm::vec3 specular) { m_Specular = specular; }

	virtual void setupShader(Shader& shader) override;
	virtual void draw() override;

protected:
	Light(const char* name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,glm::vec3 specular);

	glm::vec3 m_Direction;
	glm::vec3 m_Ambient;
	glm::vec3 m_Diffuse;
	glm::vec3 m_Specular;

};

class DirectionalLight : public Light {

public:
	DirectionalLight(const char* name, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

	virtual void setupShader(Shader& shader) override;
	virtual void draw() override;
};

class PointLight : public Light {

public:
	PointLight(const char* name, glm::vec3 position, glm::vec3 direction,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		float constant, float linear, float quadratic);

	inline float getConstant() const { return m_Constant; }
	inline float getLinear() const { return m_Linear; }
	inline float getQuadratic() const { return m_Quadratic; }

	inline void setConstant(float constant) { m_Constant = constant; }
	inline void setLinear(float linear) { m_Linear = linear; }
	inline void setQuadratic(float quadratic) { m_Quadratic = quadratic; }
	
	virtual void setupShader(Shader& shader, int numLight);
	virtual void draw();
protected:
	float m_Constant;
	float m_Linear;
	float m_Quadratic;

};

class SpotLight : public PointLight {

public:
	SpotLight(const char* name, glm::vec3 position, glm::vec3 direction,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant,
		float linear, float quadratic, float cutOff, float outerCutOff);

	inline float getCutOff() const { return m_CutOff; }
	inline float getOuterCutOff() const { return m_OuterCutOff; }

	inline void setCutOff(float cutOff) { m_CutOff = cutOff; }
	inline void setOuterCutOff(float outerCutOff) { m_OuterCutOff = outerCutOff; }

	virtual void setupShader(Shader& shader) override;
	virtual void draw() override;
private:
	float m_CutOff;
	float m_OuterCutOff;

};

#endif // !LIGHT_H