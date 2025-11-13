#include "Sprite.h"

Sprite::Sprite(Level *l, DrawEngine * d_engine, int s_index, int i_lives, Vector2f s_pos)
{
	drawEngine = d_engine;
	position.x = s_pos.x;
	position.y = s_pos.y;

	spriteIndex = s_index;
	lives = i_lives;

	facingDirection.x = 1;
	facingDirection.y = 0;
	classID = SPRITE_CLASSID;

	level = l;

}

Sprite::~Sprite()
{
	erase(position);
	//erase();
}

Vector2f Sprite::getPosition(void)
{
	return position;
}

float Sprite::getX(void)
{
	return position.x;
}

float Sprite::getY(void)
{
	return position.y;
}

void Sprite::addLives(int numLives)
{
	lives += numLives;
}

int Sprite::getLives(void)
{
	return lives;
}

bool Sprite::isAlive(void)
{
	return (lives > 0);
}

bool Sprite::move(Vector2f pos)
{
	if (isValidLevelMove(
		Vector2i((int)(position.x+pos.x), (int)(position.y + pos.y)))) {
		erase(position);

		position.x += pos.x;
		position.y += pos.y;

		facingDirection.x = pos.x;
		facingDirection.y = pos.y;

		draw(position);

		return true;
	}
	else
		return false;
}

bool Sprite::isValidLevelMove(Vector2i pos)
{
	if (level->level[pos.x][pos.y] != TILE_WALL)
		return true;
	else 
		return false;
}

void Sprite::draw(Vector2f pos)
{
	drawEngine->drawSprite(spriteIndex, pos.parseToVecInt());
}

void Sprite::erase(Vector2f pos)
{
	drawEngine->eraseSprite(pos.parseToVecInt());
}
