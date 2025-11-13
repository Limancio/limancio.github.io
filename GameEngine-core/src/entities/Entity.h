#ifndef ENTITY_H
#define ENTITY_H

#include <glm\glm.hpp>
#include "..\models\Model.h"
#include "..\graphics\Shader.h"

const float GRAVITY = -50.0f;
const float JUMP_POWER = 15.0f;
const float RUN_SPEED = 25.0f;
const float NORMAL_SPEED = 7.5f;
class Entity
{
public:
	Entity() = default;
	Entity(const char* name, glm::vec3 position, const char* objFileName, const char* textFileName,
		Material material = Material(), glm::vec3 scale = glm::vec3(1.0f),
		glm::vec3 rotation = glm::vec3(0.0f), bool useSpecular = false, bool useFakeLight = false);
	Entity(const char* name, glm::vec3 position);
	~Entity();

	inline Model getModel() const { return m_Model;  }

	inline void setPosition(glm::vec3 newPosition) { m_Position = newPosition; }
	inline void increasePosition(glm::vec3 newPosition) { m_Position += newPosition; }
	inline glm::vec3 getPosition() const { return m_Position; }

	virtual void draw();
	virtual void setupShader(Shader& shader);

protected:
	glm::vec3 m_Position;
	std::string m_Name;
	Model m_Model;

};


#endif // !ENTITY_H
