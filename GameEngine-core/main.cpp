#include <iostream>

#include "game.h"

int main(int argc, char *argv[]) {

	Game game = Game("M1 Project", 1024, 720);
	game.initGame();
	game.start();
	return 0;

}