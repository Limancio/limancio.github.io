#include "Game.h"

#include <windows.h>
#include <conio.h>
#include <iostream>

#include "Game.h"
#define GAME_SPEED 33.33

bool Game::run(void)
{
	level = new Level(&drawEngine, 30, 20);
	drawEngine.createBackgroundTile(TILE_EMPTY, ' ');
	drawEngine.createBackgroundTile(TILE_WALL, '+');

	drawEngine.createSprite(0, '$');
	player = new Character(level, &drawEngine, 0);

	level->draw();
	level->addPlayer(player);


	char key = ' ';

	startTime = timeGetTime();
	frameCount = 0;
	lastTime = 0;

	player->move(Vector2f(0, 0));
	while (key != 'q') {
		while (!getInput(&key)) {
			timerUpdate();
		}
		level->keyPress(key);
	}

	delete player;
	std::cout << frameCount / ((timeGetTime() - startTime) / 1000) << " FPS" << std::endl;
	return true;
}


bool Game::getInput(char *c) {
	if (_kbhit()) {
		*c = _getch();
		return true;
	}
	return false;
}

void Game::timerUpdate(void)
{
	double currentTime = timeGetTime() - lastTime;

	if (currentTime < GAME_SPEED)
		return;

	level->update();
	frameCount++;
	lastTime = timeGetTime();
}

