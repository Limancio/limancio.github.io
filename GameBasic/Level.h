#pragma once

#include "DrawEngine.h"

class Character;

enum {
	TILE_EMPTY,
	TILE_WALL
};

class Level {
public:
	Level(DrawEngine *d_engine, int width = 30, int height = 20);
	~Level();

	void addPlayer(Character *p);
	void update(void);
	void draw(void);
	bool keyPress(char c);

	friend class Sprite;
protected:
	void createLevel(void);

private:
	int width;
	int height;

	char **level;

	Character *player;
	DrawEngine *drawEngine;


};