#pragma once

#include "DrawEngine.h"
#include "Level.h"
#include "Vector2i.h"
#include "Vector2f.h"

enum SpriteID {
	SPRITE_CLASSID,
	CHARACTER_CLASSID
};

class Sprite {

public:
	Sprite(Level *l, DrawEngine *d_engine, int s_index, int i_lives = 1, Vector2f s_pos = Vector2f(1.0f, 1.0f));
	~Sprite();

	Vector2f getPosition(void);
	float getX(void);
	float getY(void);

	virtual void addLives(int numLives = 1);
	int getLives(void);
	bool isAlive(void);

	virtual bool move(Vector2f pos);


protected:
	Level *level;
	DrawEngine *drawEngine;
	Vector2f position;
	Vector2f facingDirection;
	SpriteID classID;
	int spriteIndex;
	int lives;

	bool isValidLevelMove(Vector2i pos);

private:
	void draw(Vector2f pos);
	void erase(Vector2f pos);

};