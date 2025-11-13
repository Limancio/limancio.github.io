#pragma once

#include "Sprite.h"
#include "UserController.h"

class Character : public Sprite {
public:
	Character(Level *l, DrawEngine *d_engine, int s_index, int i_lives = 3, Vector2f s_pos = Vector2f(1.0f, 1.0f), UserController u_controller = UserController('w', 's', 'a', 'd'));

	virtual bool keyPress(char c);

protected:
	UserController controller;
};