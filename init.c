#include <SDL.h>
#include <SDL_image.h>

#include "drawing.h"
#include "init.h"
#include "tetris.h"
#include "useful.h"

SDL_Surface *tetris_logo, *brick[7], *pause_img, *level_img, *score_img,
	*score_digits[10], *level_digits[10], *game_over_img, *on_img, *off_img;
int brick_size;
Uint32 black, white;

extern const int area_x, area_y;
extern video_flags;

void init_colors(SDL_PixelFormat *format)
{
	black = SDL_MapRGB(format, 0, 0, 0);
	white = SDL_MapRGB(format, 255, 255, 255);
}

SDL_Window* window;
SDL_Surface* window_surface;
void init_video()
{
	printf("init video mode 800x600 8bpp ... ");

	window = SDL_CreateWindow("SDL Drums", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, VIDEO_WIDTH,
		VIDEO_HEIGHT, SDL_WINDOW_SHOWN);
	
	if(window == NULL) {
		printf("Couldn't open video mode: %s\n", SDL_GetError());
		exit(1);
	}
	
	printf("done!\n");
	
	window_surface = SDL_GetWindowSurface(window);
}

void load_images(void)
{
	int i;

	tetris_logo = IMG_Load("./img/logos/logo2.png");
	pause_img = IMG_Load("./img/pause.png");
	level_img = IMG_Load("./img/level.png");
	score_img = IMG_Load("./img/score.png");
	game_over_img = IMG_Load("./img/game_over.png");
	on_img = IMG_Load("./img/on.png");
	off_img = IMG_Load("./img/off.png");

	level_digits[0] = IMG_Load("./img/level_digits/0.bmp");
	level_digits[1] = IMG_Load("./img/level_digits/1.bmp");
	level_digits[2] = IMG_Load("./img/level_digits/2.bmp");
	level_digits[3] = IMG_Load("./img/level_digits/3.bmp");
	level_digits[4] = IMG_Load("./img/level_digits/4.bmp");
	level_digits[5] = IMG_Load("./img/level_digits/5.bmp");
	level_digits[6] = IMG_Load("./img/level_digits/6.bmp");
	level_digits[7] = IMG_Load("./img/level_digits/7.bmp");
	level_digits[8] = IMG_Load("./img/level_digits/8.bmp");
	level_digits[9] = IMG_Load("./img/level_digits/9.bmp");

	score_digits[0] = IMG_Load("./img/digits/0.png");
	score_digits[1] = IMG_Load("./img/digits/1.png");
	score_digits[2] = IMG_Load("./img/digits/2.png");
	score_digits[3] = IMG_Load("./img/digits/3.png");
	score_digits[4] = IMG_Load("./img/digits/4.png");
	score_digits[5] = IMG_Load("./img/digits/5.png");
	score_digits[6] = IMG_Load("./img/digits/6.png");
	score_digits[7] = IMG_Load("./img/digits/7.png");
	score_digits[8] = IMG_Load("./img/digits/8.png");
	score_digits[9] = IMG_Load("./img/digits/9.png");

	brick[0] = IMG_Load("./img/bricks/brick0.png");
	brick[1] = IMG_Load("./img/bricks/brick1.png");
	brick[2] = IMG_Load("./img/bricks/brick2.png");
	brick[3] = IMG_Load("./img/bricks/brick3.png");
	brick[4] = IMG_Load("./img/bricks/brick4.png");
	brick[5] = IMG_Load("./img/bricks/brick5.png");
	brick[6] = IMG_Load("./img/bricks/brick6.png");

	brick_size = brick[0]->w;

	if(brick[0]->w != brick[0]->h) {
		printf("brick 0 isn't square!\n");
		exit(1);
	}

	if(brick_size*20 > VIDEO_HEIGHT - 10) {
		printf("brick_size is too large!\n");
		exit(1);
	}

	for(i = 1; i < 7; i++)
		if(brick[i]->w != brick_size || brick[i]->h != brick_size) {
			printf("brick %i is the wrong size!\n", i);
			exit(1);
		}
}

void display_tetris_logo(SDL_Surface *window)
{
	SDL_Rect img_rect;
	img_rect.x = 30, img_rect.y = 40;
	img_rect.w = tetris_logo->w, img_rect.h = tetris_logo->h;
	
	SDL_BlitSurface(tetris_logo, NULL, window, &img_rect);
}

void draw_game_area(SDL_Surface *window)
{
	draw_rect(window, area_x-1, area_y-1, AREA_WIDTH+2, AREA_HEIGHT+2, white);
}

void draw_level_area(SDL_Surface *window)
{
	SDL_Rect img_rect;

	img_rect.x = LEVEL_AREA_X + 10, img_rect.y = LEVEL_AREA_Y + 10;
	img_rect.w = level_img->w, img_rect.h = level_img->h;
	SDL_BlitSurface(level_img, NULL, window, &img_rect);

	draw_rect(window, LEVEL_AREA_X, LEVEL_AREA_Y, 190, 140, white);
}

void draw_score_area(SDL_Surface *window)
{
	SDL_Rect img_rect;

	img_rect.x = SCORE_AREA_X, img_rect.y = SCORE_AREA_Y;
	img_rect.w = score_img->w, img_rect.h = score_img->h;

	SDL_BlitSurface(score_img, NULL, window, &img_rect);

	display_digits(window, score_digits, "0",
		img_rect.x + (img_rect.w - score_digits[0]->w)/2,
		img_rect.y + img_rect.h + 10);

	draw_rect(window, img_rect.x - 10, img_rect.y - 10, 190, 120, white);
}

