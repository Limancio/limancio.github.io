#include <Windows.h>
#include <iostream>

#include "DrawEngine.h"


DrawEngine::DrawEngine(Vector2i size)
{
	screenWidth = size.x;
	screenHeight = size.y;

	spriteImage = new char[spriteCount];
	tileImage = new char[spriteCount];
	cursorVisibility(false);

	map = 0;

	//set cursor visability to false
}

DrawEngine::~DrawEngine()
{
	cursorVisibility(true);
	//delete[] spriteImage;
	//delete[] tileImage;
	gotoxy(Vector2i(0, screenHeight));
	//set cursos visability to true
}



int DrawEngine::createSprite(int index, char c)
{
	if (index >= 0 || index < spriteCount) {
		(spriteImage[index]) = c;
		return index;
	}

	return -1;
}

void DrawEngine::deleteSprite(int index)
{
}

void DrawEngine::eraseSprite(Vector2i pos)
{
	gotoxy(pos);
	std::cout << ' ';
}

void DrawEngine::drawSprite(int index, Vector2i pos)
{
	gotoxy(pos);
	std::cout << spriteImage[index];
}

void DrawEngine::createBackgroundTile(int index, char c)
{
	if (index >= 0 || index < spriteCount) {
		tileImage[index] = c;
	}
}

void DrawEngine::setMap(char ** data)
{
	map = data;
}

void DrawEngine::drawBackground(void)
{
	if (map) {
		for (int y = 0; y < screenWidth; y++) {
			gotoxy(Vector2i(0, y));
			for (int x = 0; x < screenHeight; x++) {
				std::cout << tileImage[map[x][y]];
			}
		}
	}
}

void DrawEngine::gotoxy(Vector2i newPos)
{
	HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = newPos.x;
	pos.Y = newPos.y;
	SetConsoleCursorPosition(output_handle, pos);

}

void DrawEngine::cursorVisibility(bool visibility)
{
	HANDLE output_handle;
	CONSOLE_CURSOR_INFO cciInfo;

	cciInfo.bVisible = visibility;
	cciInfo.dwSize = 1;
	output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorInfo(output_handle, &cciInfo);
}




