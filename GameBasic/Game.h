#pragma once

#include "DrawEngine.h"
#include "Character.h"
#include "Level.h"


class Game {
public:
	bool run(void);
protected:
	bool getInput(char *c);
	void timerUpdate(void);
private:
	Level *level;
	Character *player;

	double frameCount;
	double startTime;
	double lastTime;

	DrawEngine drawEngine;
};