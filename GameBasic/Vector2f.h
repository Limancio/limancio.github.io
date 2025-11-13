#pragma once
#include "Vector2i.h"

struct Vector2f {
	float x;
	float y;

	Vector2f(float X, float Y) 
		: x(X), y(Y) {}

	Vector2f() 
		: x(0.0f), y(0.0f) {}

	Vector2f Add(const Vector2f& other) const {
		return Vector2f(x + other.x, y + other.y);
	}

	Vector2f operator+(const Vector2f& other) const {
		return Add(other);
	}

	Vector2f Multiply(const Vector2f& other) const {
		return Vector2f(x * other.x, y * other.y);
	}

	Vector2f operator*(const Vector2f& other) const {
		return Multiply(other);
	}

	Vector2i parseToVecInt() {
		return Vector2i((int)x,(int)y);
	}
};