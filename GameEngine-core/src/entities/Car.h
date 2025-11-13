#pragma once

#include "Entity.h"

#include <glm\glm.hpp>
#include <glm/gtc/constants.hpp>

class Car : public Entity
{
public:
	Car(const char* name, glm::vec3 position, const char* objFileName, const char* textFileName,
		Material material = Material(), glm::vec3 scale = glm::vec3(1.0f),
		glm::vec3 rotation = glm::vec3(0.0f), bool useSpecular = false, bool useFakeLight = false);
	~Car();
	const float MOVE_SPEED = 5.0f; // units per second

	void updateCameraVectors();
	void rotate(float yaw, float pitch);
	void move(const glm::vec3& offsetPos);

	inline glm::vec3 getLook() const { return mLook; }

	inline glm::vec3 getRight() const { return mRight; }

	inline glm::vec3 getUp() const { return mUp; }
	
	inline glm::vec3 getPosition() const { return mPosition; }

	inline float getCurrentSpeed() const { return m_CurrentSpeed; }
private:
	float m_CurrentSpeed;
	float m_UpwardsSpeed;

	glm::vec3 mPosition;
	glm::vec3 mLook;
	glm::vec3 mUp;
	glm::vec3 mRight;
	const glm::vec3 WORLD_UP;

	// Euler Angles (in radians)
	float mYaw;
	float mPitch;
};
