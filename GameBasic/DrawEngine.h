#pragma once

#include "Vector2i.h"

class DrawEngine {
public:
	DrawEngine(Vector2i size = Vector2i(20, 30));

	~DrawEngine();

	int createSprite(int index, char c);
	void deleteSprite(int index);

	void eraseSprite(Vector2i pos);
	void drawSprite(int index, Vector2i pos);

	void createBackgroundTile(int index, char c);
	void setMap(char **);
	void drawBackground(void);

protected:
	char **map;
	int screenWidth, screenHeight;
	int spriteCount;
	char* spriteImage;
	char* tileImage;

private:
	void gotoxy(Vector2i newPos);
	void cursorVisibility(bool visibility);

};