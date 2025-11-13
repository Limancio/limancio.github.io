#include "Player.h"


Player::Player(const char* name, glm::vec3 position, Window *window, Model model)
	: Entity(name, position), m_Window(window), m_FpsCamera(new FPSCamera(glm::vec3(0.0f, 0.0f, 0.0f)))
{
	m_CurrentSpeed = MOVE_SPEED;
	m_IsInAir = false;
	m_IsRunning = false;
	m_Noclip = false;
	m_UpwardsSpeed = 0.0f;
}

Player::~Player()
{
}

void Player::update()
{
	cameraMovement();
}

void Player::playerMovement(const Terrain& terrain)
{

	/*PLAYER JUMP*/
	if (true)
		return;

	/*PLAYER TERRAIN COLLISION*/
	m_UpwardsSpeed += -50.0f * Window::getElapsedTime();
	float terrainHeight = terrain.getHeightOfTerrain(m_FpsCamera->getPosition().x, m_FpsCamera->getPosition().z);
	if ((m_FpsCamera->getPosition().y ) < terrainHeight + 2.0f) {
		m_UpwardsSpeed = 0;
		m_FpsCamera->setPosition(glm::vec3(m_FpsCamera->getPosition().x, terrainHeight + 2.0f, m_FpsCamera->getPosition().z));
		if (m_IsInAir)
			m_IsInAir = false;
	}
	m_FpsCamera->increasePosition(glm::vec3(0, m_UpwardsSpeed * Window::getElapsedTime(), 0));

	//setPosition(m_FpsCamera->getPosition());
}

void Player::cameraMovement()
{
	float elapsedTime = m_Window->getElapsedTime();

	if (m_Window->isKeyPressed(GLFW_KEY_SPACE) && !m_IsInAir) {
		m_UpwardsSpeed = JUMP_POWER;
		m_IsInAir = true;
	}
	if (m_Window->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		m_CurrentSpeed = RUN_SPEED;
		m_IsRunning = true;
	}
	if (!m_Window->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && m_IsRunning) {
		m_CurrentSpeed = NORMAL_SPEED;
		m_IsRunning = false;
	}

	//glm::vec2 mousePos(m_Window->getMousePosition()); TODO: use this method

	double mouseX, mouseY;
	glfwGetCursorPos(m_Window->getWindow(), &mouseX, &mouseY);

	if (!m_Window->isMouseLocked()) {
		glfwSetCursorPos(m_Window->getWindow(), m_Window->getWidth() / 2.0, m_Window->getHeight() / 2.0);
		m_FpsCamera->rotate((float)(m_Window->getWidth() / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(m_Window->getHeight() / 2.0 - mouseY) * MOUSE_SENSITIVITY);
	}

	// Forward/backward
	if (m_Window->isKeyPressed(GLFW_KEY_W))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * m_FpsCamera->getLook(), false);
	else if (m_Window->isKeyPressed(GLFW_KEY_S))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * -m_FpsCamera->getLook(), false);

	// Strafe left/right
	if (m_Window->isKeyPressed(GLFW_KEY_A))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * -m_FpsCamera->getRight(), false);
	else if (m_Window->isKeyPressed(GLFW_KEY_D))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * m_FpsCamera->getRight(), false);

	// Up/down
	if (m_Window->isKeyPressed(GLFW_KEY_Z))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * m_FpsCamera->getUp(), false);
	else if (m_Window->isKeyPressed(GLFW_KEY_X))
		m_FpsCamera->move(m_CurrentSpeed * elapsedTime * -m_FpsCamera->getUp(), false);



}
