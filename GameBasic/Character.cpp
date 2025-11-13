#include "Character.h"

Character::Character(Level *l, DrawEngine * d_engine, int s_index, int i_lives, Vector2f s_pos, UserController u_controller) 
	: Sprite(l, d_engine, s_index, i_lives, s_pos)
{
	controller = u_controller;
	classID = CHARACTER_CLASSID;
}

bool Character::keyPress(char c)
{
	if(c == controller.up_key) {
		return move(Vector2f(0, -1));
	} else if (c == controller.down_key) {
		return move(Vector2f(0, 1));
	} else if (c == controller.left_key) {
		return move(Vector2f(-1, 0));
	} else if (c == controller.right_key) {
		return move(Vector2f(1, 0));
	}

	return false;
}

