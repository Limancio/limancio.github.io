#include "Level.h"
#include "Character.h"
#include <stdlib.h>

Level::Level(DrawEngine * d_engine, int w, int h)
{
	drawEngine = d_engine;

	width = w;
	height = h;

	player = 0;

	level = new char *[width];

	for (int x = 0; x < width; x++) {
		level[x] = new char[height];
	}

	createLevel();

	drawEngine->setMap(level);


}

Level::~Level()
{

	for (int x = 0; x < width; x++) 
		delete[] level[x];
	
	delete[] level;
}

void Level::addPlayer(Character * p)
{
	player = p;
}

void Level::update(void)
{
}

void Level::draw(void)
{
	drawEngine->drawBackground();
}

bool Level::keyPress(char c)
{
	if (player)
		if (player->keyPress(c))
			return true;
	
	return false;
}

void Level::createLevel(void)
{
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int random = rand() % 100;

			if (y == 0 || y == height - 1 || x == 0 || x == width - 1) {
				level[x][y] = TILE_WALL;
			} else {
				if (random < 90 || (x < 3 && y < 3)) {
					level[x][y] = TILE_EMPTY;
				}
				else {
					level[x][y] = TILE_WALL;
				}
			}
		}
	}
}
