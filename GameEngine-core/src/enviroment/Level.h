#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

#include "Terrain.h"
#include "Skybox.h"
#include "../entities/Player.h"
#include "../graphics/Light.h"
#include "../graphics/Shader.h"
#include "../graphics/Window.h"
#include "../input/MousePicker.h"

class Level
{
public:
	Level(const char* levelName);
	Level(Window* window);
	//Level(Terrain* terrain, Skybox* skybox, std::vector<Entity*> entities, std::vector<DirectionalLight*> dirLights, std::vector<SpotLight*> spotLights, std::vector<PointLight*> pointLights, Player* player, Window* window, Shader* basicShader);
	//Level(Terrain* terrain, Skybox* skybox, Entity* entities, DirectionalLight* dirLights, SpotLight* spotLights, PointLight* pointLights, Player* player, Window* window, Shader* basicShader);
	~Level();

	void update();
	void draw();

	inline Player* getPlayer() const { return m_Player; }
	inline Terrain* getTerrain() const { return m_Terrain; }

private:
	Terrain* m_Terrain;
	Skybox* m_Skybox;
	Player* m_Player;
	Window* m_Window;
	Shader* m_basicShader;
	MousePicker* m_MousePicker;

	std::vector<Entity*> m_Entities;
	std::vector<DirectionalLight*> m_DirLights;
	std::vector<SpotLight*> m_SpotLights;
	std::vector<PointLight*> m_PointLight;
	/*Entity m_Entitie;
	DirectionalLight m_DirLight;
	SpotLight m_SpotLight;
	PointLight m_PointLight;*/
};

#endif // !LEVEL_H
