#include "MousePicker.h"

#include <glm\gtx\transform.hpp>

MousePicker::MousePicker(Window* m_Window, Terrain* terrain, FPSCamera* camera)
	: m_Window(m_Window), m_Terrain(terrain), m_Camera(camera) {}

MousePicker::~MousePicker()
{
}

void MousePicker::update()
{
	m_ViewMatrix = m_Camera->getViewMatrix();
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Camera->getFOV()), (float)m_Window->getWidth() / (float)m_Window->getHeight(), 0.1f, 200.0f);
	m_CurrentRay = glm::vec3(calculateMouseRay());
	if (intersectionInRange(0, RAY_RANGE, m_CurrentRay)) {
		m_CurrentTerrainPoint = glm::vec3(binarySearch(0, 0, RAY_RANGE, m_CurrentRay));
	}
	else {
		m_CurrentTerrainPoint = glm::vec3(-1.0f);
	}
}

glm::vec3 MousePicker::calculateMouseRay()
{
	float mouseX = m_Window->getMousePosition().x;
	float mouseY = m_Window->getMousePosition().y;
	glm::vec2 normalizedCoords(getNormalizedDeviceCoords(mouseX, mouseY));
	glm::vec4 clipCoords(glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f));
	glm::vec4 eyeCoords(toEyeCoords(clipCoords));
	glm::vec3 worldRay(toWorldCoords(eyeCoords));
	std::cout << "Vector3f [" << worldRay.x << ", " << worldRay.y << ", " << worldRay.z << "]" << std::endl;
	return worldRay;
}


glm::vec2 MousePicker::getNormalizedDeviceCoords(float mouseX, float mouseY)
{
	float x = (2.0f * mouseX) / m_Window->getWidth() - 1.0f;
	float y = (2.0f * mouseY) / m_Window->getHeight() - 1.0f;
	return glm::vec2(x, -y);
}

glm::vec4 MousePicker::toEyeCoords(glm::vec4 clipCoords)
{
	glm::mat4 invertedProjection(glm::inverse(m_ProjectionMatrix));
	glm::vec4 eyeCoords(invertedProjection * clipCoords);

	return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec3 MousePicker::toWorldCoords(glm::vec4 eyeCoords)
{
	glm::mat4 invertedView = glm::inverse(m_ViewMatrix);
	glm::vec4 rayWorld(invertedView * eyeCoords);
	glm::vec3 mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay = glm::normalize(mouseRay);
	return mouseRay;
}

glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray, const float distance) {
	glm::vec3 camPos = m_Camera->getPosition();
	glm::vec3 start = glm::vec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 scaledRay = glm::vec3(ray.x * distance, ray.y * distance, ray.z * distance);
	//std::cout << "Vector3f [" << scaledRay.x << ", " << scaledRay.y << ", " << scaledRay.z << "]" << std::endl;

	return start + scaledRay;
}

bool MousePicker::inAABB_test(glm::vec3 vMin, glm::vec3 vMax) {
	glm::vec3 ray = m_CurrentRay;
	glm::vec3 camPos = m_Camera->getPosition();
	glm::vec3 start = glm::vec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 scaledRay = glm::vec3(ray.x * 600.0f , ray.y * 600.0f, ray.z * 600.0f);
	glm::vec3 rayDir = ray;
	float tmp;

	/*X */
	float txMin = (vMin.x - start.x) / rayDir.x;
	float txMax = (vMax.x - start.x) / rayDir.x;
	if (txMax < txMin) { tmp = txMax; txMax = txMin; txMin = tmp; }

	
	float tyMin = (vMin.y - start.y) / rayDir.y;
	float tyMax = (vMax.y - start.y) / rayDir.y;
	if (tyMax < tyMin) { tmp = tyMax; tyMax = tyMin; tyMin = tmp; }

	float tzMin = (vMin.z - start.z) / rayDir.z;
	float tzMax = (vMax.z - start.z) / rayDir.z;
	if (tzMax < tzMin) { tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

	

	
	float tMin = (txMin > tyMin)? txMin : tyMin; //Get Greatest Min
	float tMax = (txMax < tyMax)? txMax : tyMax; //Get Smallest Max

	if (txMin > tyMax || tyMin > txMax) return false;
	if (tMin > tzMax || tzMin > tMax) return false;
	if(tzMin > tMin) tMin = tzMin;
	if(tzMax < tMax) tMax = tzMax;
	//console.log("success");
	return true;
}

void MousePicker::checkSelectedEntity(std::vector<Entity*> entities)
{
	int modelID = -1;
	for (int index = 0; index < entities.size() ; index++) {
		Mesh* mesh = entities[index]->getModel().getMesh();
		glm::vec3 auxPosition(entities[index]->getPosition());
		//entities[index]->getModel().getMesh().getMin();
		if (inAABB_test(mesh->getMin() + auxPosition, mesh->getMax() + auxPosition)) {
			modelID = index;
			break;
		}
	}
	if (modelID == -1) {
		m_SelectedEntity = nullptr;
	}
	else {
		m_SelectedEntity = entities[modelID];
	}
}

glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray) {
	float half = start + ((finish - start) / 2.0f);
	if (count >= RECURSION_COUNT) {
		glm::vec3 endPoint = getPointOnRay(ray, half);
		/*Terrain terrain = getTerrain(endPoint.getX(), endPoint.getZ());
		if (terrain != null) {*/
			return endPoint;
		/*}
		else {
			return null;
		}*/
	}
	if (intersectionInRange(start, half, ray)) {
		return binarySearch(count + 1, start, half, ray);
	}
	else {
		return binarySearch(count + 1, half, finish, ray);
	}
}

bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray) {
	glm::vec3 startPoint = getPointOnRay(ray, start);
	glm::vec3 endPoint = getPointOnRay(ray, finish);
	if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
		return true;
	}
	else {
		return false;
	}
}

bool MousePicker::isUnderGround(glm::vec3 testPoint) {
	//Terrain terrain = getTerrain(testPoint.getX(), testPoint.getZ());
	float height = 0;
	//if (terrain != null) {
	height = m_Terrain->getHeightOfTerrain(testPoint.x, testPoint.z);
	//}
	if (testPoint.y <= height) {
		return true;
	}
	else {
		return false;
	}
}