#ifndef MOUSE_PICKER_H
#define MOUSE_PICKER_H

#include <glm/glm.hpp>
#include "..\graphics\Window.h"
#include "..\entities\Entity.h"
#include "..\enviroment\Terrain.h"

class MousePicker
{
public:
	MousePicker() = default;
	MousePicker(Window* m_Window, Terrain* terrain, FPSCamera* camera);
	~MousePicker();

	inline glm::vec3 MousePicker::getCurrentRay() const { return  m_CurrentRay; };
	inline glm::vec3 MousePicker::getCurrentTerrainPoint() const { return m_CurrentTerrainPoint; };

	inline void setRadius(float radius) { m_Radius = radius; }
	inline void setPower(float power) { m_Power = power; }

	inline float getRadius() { return m_Radius; }
	inline float getPower() { return m_Power; }

	inline Entity* getSelectedEntity() const { return m_SelectedEntity; }
	inline void setSelectedEntity(Entity* entity) { m_SelectedEntity = entity; }

	void update();
	glm::vec3 calculateMouseRay();
	glm::vec2 getNormalizedDeviceCoords(float mouseX, float mouseY);
	glm::vec4 toEyeCoords(glm::vec4 clipCoords);
	glm::vec3 toWorldCoords(glm::vec4 eyeCoords);

	void MousePicker::checkSelectedEntity(std::vector<Entity*> entities);
	bool MousePicker::inAABB_test(glm::vec3 vMin, glm::vec3 max);

	glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray,const float distance);
	glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray);
	bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray);
	bool MousePicker::isUnderGround(glm::vec3 testPoint);

private:
	int RECURSION_COUNT = 200;
	float RAY_RANGE = 600;

	float m_Radius;
	float m_Power;
	Entity* m_SelectedEntity;

	Window* m_Window;
	Terrain* m_Terrain;
	FPSCamera* m_Camera;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;

	glm::vec3 m_CurrentRay;
	glm::vec3 m_CurrentTerrainPoint;

};

#endif // !MOUSE_PICKER_H
