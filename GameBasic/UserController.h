#pragma once

struct UserController
{
	UserController(char up = 'w', char down = 's', char left = 'a', char right = 'd');

	char up_key;
	char down_key;
	char left_key;
	char right_key;
};