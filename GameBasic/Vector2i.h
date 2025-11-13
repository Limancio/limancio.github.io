#pragma once

struct Vector2i {
	int x;
	int y;

	Vector2i(int X, int Y) {
		x = X;
		y = Y;
	}

	Vector2i() {
		x = 0;
		y = 0;
	}
};