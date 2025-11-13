#ifndef GAME_H
#define GAME_H

#include <vector>

#include "src\vendor\imgui\imgui.h"
#include "src\vendor\imgui\imgui_impl_glfw.h"
#include "src\vendor\imgui\imgui_impl_opengl3.h"

#include "src\enviroment\Level.h"
#include "src\enviroment\Skybox.h"
#include "src\entities\Player.h"
#include "src\renderEngine\Renderer.h"
#include "src\graphics\Camera.h"
#include "src\graphics\Window.h"
#include "src\input\MousePicker.h"

class Game
{
public:

	Game(const char *title, int width, int height);
	~Game();

	void initGame();

	void update();
	void draw();
	void start();

private:
	std::vector<Level> m_Levels;
	Level* m_currentLevel;
	Window* m_Window;
	//Renderer renderer;


};

#endif // !GAME_H
