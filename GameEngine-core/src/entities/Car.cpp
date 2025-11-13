#include "Car.h"

void Car::move(const glm::vec3& offsetPos)
{

	mPosition += offsetPos;

	updateCameraVectors();
}

void Car::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	// Constrain the pitch
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	updateCameraVectors();
}

Car::Car(const char * name, glm::vec3 position, const char * objFileName, const char * textFileName, 
	Material material, glm::vec3 scale, glm::vec3 rotation, bool useSpecular, bool useFakeLight) 
	: Entity(name, position, objFileName, textFileName, material, scale, rotation, useSpecular, useFakeLight),
	mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	mRight(0.0f, 0.0f, 0.0f),
	WORLD_UP(0.0f, 1.0f, 0.0f),
	mYaw(glm::pi<float>()),
	mPitch(0.0f)
{
	m_CurrentSpeed = MOVE_SPEED;
	m_UpwardsSpeed = 0.0f;
	
}

	void Car::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system (NOTE: Our coordinate sys has Y up not Z)

	// Calculate the view direction vector based on yaw and pitch angles (roll not considered)
	// radius is 1 for normalized length
	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	// Re-calculate the Right and Up vector.  For simplicity the Right vector will
	// be assumed horizontal w.r.t. the world's Up vector.
	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));

	mPosition = mPosition + mLook;
}