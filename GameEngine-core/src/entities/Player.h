#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "../graphics/Camera.h"
#include "../graphics/Window.h"
#include "../enviroment/Terrain.h"

class Player : public Entity
{
public:
	Player() = default;
	Player(const char* name, glm::vec3 position, Window *window, Model model = Model());
	~Player();

	const double ZOOM_SENSITIVITY = -3.0;
	const float MOVE_SPEED = 5.0f; // units per second
	const float MOUSE_SENSITIVITY = 0.1f;

	inline FPSCamera* getCamera() const { return m_FpsCamera; }

	void update();
	void playerMovement(const Terrain& terrain);

private:
	void cameraMovement();

private:
	float m_CurrentSpeed;
	float m_UpwardsSpeed;

	bool m_IsInAir;
	bool m_IsRunning;
	bool m_Noclip;

	FPSCamera* m_FpsCamera;
	Window* m_Window;
};

#endif // !PLAYER_H
