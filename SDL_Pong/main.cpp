#include <iostream>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)

int pt1 = 0, pt2 = 0;

typedef struct{
	float x_pos;
	float y_pos;
	float x_vel;
	float y_vel;
	float speed;
	SDL_Texture* texture;
	SDL_Rect destination;
}Ball;

typedef struct {
	float x_pos;
	float y_pos;
	float x_vel;
	float y_vel;
	float speed;
	SDL_Texture* texture;
	SDL_Rect destination;
}Paddle;

SDL_Texture* createTexture(SDL_Renderer* render, SDL_Window* window, const char* file) {
	SDL_Surface* surface = IMG_Load(file);
	if (!surface) {
		printf("error creating surface\n");
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return NULL;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);

	if (!texture) {
		printf("error creating texture: %s \n", SDL_GetError());
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return NULL;
	}

	return texture;
}

void player_collision_detection(Paddle* player, Ball ball) {

	if (player->y_pos <= 0 && player->y_vel > 0.0f) {
		player->y_vel = 0.0f;
		player->x_vel = 0.0f;
	}
	if (player->y_pos >= WINDOW_HEIGHT - (player->destination).h  && player->y_vel < 0.0f) {
		player->y_vel = 0.0f;
		player->x_vel = 0.0f;
	}
	player->x_pos -= player->x_vel / 60;
	player->y_pos -= player->y_vel / 60;

	//(player->destination).y = (int)ball.y_pos;
	(player->destination).y = (int)player->y_pos;
	(player->destination).x = (int)player->x_pos;

	player->y_vel = 0.0f;
	player->x_vel = 0.0f;
}

/*BALL COLLISION DETECTION*/
void ball_collision_detection(Ball* ball, Paddle p1, Paddle p2) {

	if (ball->x_pos <= 0) {
		ball->x_pos = 0;
		ball->x_vel = -ball->x_vel;
		pt2++;
		printf("%i - %i\n", pt1,pt2);
		ball->x_pos = (WINDOW_WIDTH - ball->destination.w) / 2;
		ball->y_pos = (WINDOW_HEIGHT - ball->destination.h) / 2;
		ball->x_vel = 400.0f;
		ball->y_vel = 350.0f;
	}
	if (ball->y_pos <= 0) {
		ball->y_pos = 0;
		ball->y_vel = -ball->y_vel;
	}
	if (ball->x_pos >= WINDOW_WIDTH) {
		ball->x_pos = WINDOW_WIDTH - (p2.destination).w;
		ball->x_vel = -ball->x_vel;
		pt1++;
		printf("%i - %i\n", pt1, pt2);
		ball->x_pos = (WINDOW_WIDTH - ball->destination.w) / 2;
		ball->y_pos = (WINDOW_HEIGHT - ball->destination.h) / 2;
		ball->x_vel = 400.0f;
		ball->y_vel = 350.0f;
	}
	if (ball->y_pos >= WINDOW_HEIGHT - (ball->destination).h) {
		ball->y_pos = WINDOW_HEIGHT - (ball->destination).h;
		ball->y_vel = -ball->y_vel;
	}

	if (p1.x_pos + p1.destination.w >= ball->x_pos && (p1.y_pos + p1.destination.h >= ball->y_pos && p1.y_pos <= ball->y_pos)) {
		ball->x_vel = -ball->x_vel;
	}
	if (p2.x_pos - p2.destination.w/2 <= ball->x_pos && (p2.y_pos + p2.destination.h >= ball->y_pos && p1.y_pos <= ball->y_pos)) {
		ball->x_vel = -ball->x_vel;
	}

	ball->x_pos -= ball->x_vel / 60;
	ball->y_pos -= ball->y_vel / 60;

	(ball->destination).y = (int)ball->y_pos;
	(ball->destination).x = (int)ball->x_pos;
}


int main(int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL could not initialise! SDL Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Window *window;                    // Declare a pointer

										   // Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_CENTERED,           // initial x position
		SDL_WINDOWPOS_CENTERED,           // initial y position
		WINDOW_WIDTH,                               // width, in pixels
		WINDOW_HEIGHT,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* render = SDL_CreateRenderer(window, -1, render_flags);

	if (!render) {
		printf("error creating renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	
	Ball ball;
	Paddle player;
	Paddle enemy;
	ball.texture = createTexture(render, window, "resources/ball.png");
	player.texture = createTexture(render, window, "resources/paddlePlayer.png");
	enemy.texture = createTexture(render, window, "resources/paddleBot.png");


	SDL_QueryTexture(ball.texture, NULL, NULL, &ball.destination.w, &ball.destination.h);
	SDL_QueryTexture(player.texture, NULL, NULL, &player.destination.w, &player.destination.h);
	SDL_QueryTexture(enemy.texture, NULL, NULL, &enemy.destination.w, &enemy.destination.h);

	ball.destination.w /= 4;
	ball.destination.h /= 4;

	ball.x_pos = (WINDOW_WIDTH - ball.destination.w) / 2;
	ball.y_pos = (WINDOW_HEIGHT - ball.destination.h) / 2;
	ball.x_vel = 250.0f;
	ball.y_vel = -350.0f;

	player.destination.w /= 2;
	player.destination.h /= 2;

	player.x_pos = (player.destination.w / 2);
	player.y_pos = (WINDOW_HEIGHT - player.destination.h) / 2;
	player.x_vel = 0.0f;
	player.y_vel = 0.0f;
	player.speed = 300.0f;

	player.destination.y = player.y_pos;
	player.destination.x = player.x_pos;

	enemy.destination.w /= 2;
	enemy.destination.h /= 2;

	enemy.x_pos = (WINDOW_WIDTH - enemy.destination.w);
	enemy.y_pos = (WINDOW_HEIGHT - enemy.destination.h) / 2;
	enemy.x_vel = 0.0f;
	enemy.y_vel = 0.0f;
	enemy.speed = 300.0f;

	enemy.destination.y = enemy.y_pos;
	enemy.destination.x = enemy.x_pos;

	int close_requested = 0;

	int up    = 0, 
		down  = 0, 
		left  = 0, 
		right = 0;

	while (!close_requested) {

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					close_requested = !close_requested;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_UP:
						up = 1;
						break;
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_DOWN:
						down = 1;
						break;
					case SDL_SCANCODE_A:
					case SDL_SCANCODE_LEFT:
						left = 1;
						break;
					case SDL_SCANCODE_D:
					case SDL_SCANCODE_RIGHT:
						right = 1;
						break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_UP:
						up = 0;
						break;
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_DOWN:
						down = 0;
						break;
					case SDL_SCANCODE_A:
					case SDL_SCANCODE_LEFT:
						left = 0;
						break;
					case SDL_SCANCODE_D:
					case SDL_SCANCODE_RIGHT:
						right = 0;
						break;
					}
					break;
			}
		}

		if (up && !down) player.y_vel = player.speed;
		if (down && !up) player.y_vel = -player.speed;

		player_collision_detection(&player,ball);
		ball_collision_detection(&ball, player, enemy);


		if (enemy.y_pos < ball.y_pos) {
			enemy.y_vel -= enemy.speed;
		}
		else {
			enemy.y_vel += enemy.speed;
		}
		enemy.x_pos -= enemy.x_vel / 60;
		enemy.y_pos -= enemy.y_vel / 60;
		enemy.destination.y = enemy.y_pos;
		enemy.destination.x = enemy.x_pos;

		enemy.y_vel = 0.0f;
		enemy.x_vel = 0.0f;

		//Clear the window
		SDL_RenderClear(render);

		//Draw the image in the window
		SDL_RenderCopy(render, ball.texture, NULL, &ball.destination);
		SDL_RenderCopy(render, player.texture, NULL, &player.destination);
		SDL_RenderCopy(render, player.texture, NULL, &enemy.destination);
		SDL_RenderPresent(render);


		SDL_Delay(1000/60);

	}


	SDL_DestroyTexture(ball.texture);
	SDL_DestroyTexture(player.texture);
	SDL_DestroyTexture(enemy.texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
