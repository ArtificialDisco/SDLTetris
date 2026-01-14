#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

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

SDL_Window* window;
SDL_Surface* window_surface;

Mix_Music* music = NULL;

// Keep track of all surfaces so we can free them correctly
// Total surfaces are currently 64. Update this when needed.
SDL_Surface* all_surfaces[40];
int total_surfaces = 0;

void free_surfaces() {
	printf("Freeing %i surfaces...\n", total_surfaces);
	for (int i = 0; i < total_surfaces; i++) {
		SDL_FreeSurface(all_surfaces[i]);
	}
}

SDL_Surface* load_surface(const char* path,
	int optimize) {
	SDL_Surface* optimizedSurface = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL) {
		printf("Unable to load image %s!\nSDL_image Error: %s\n", path, IMG_GetError());
	}
	else {
		if (!optimize) {
			all_surfaces[total_surfaces++] = optimizedSurface;
			return loadedSurface;
		}
		optimizedSurface = SDL_ConvertSurface(loadedSurface, window_surface->format, 0);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s!\nSDL Error: %s\n", path, SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	all_surfaces[total_surfaces++] = optimizedSurface;
	return optimizedSurface;
}

void init_sdl() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not be initialized: %s\n", SDL_GetError());
		close_program(-1);
	}

	printf("init video mode 800x600 8bpp ... ");

	window = SDL_CreateWindow("SDL Drums", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, VIDEO_WIDTH,
		VIDEO_HEIGHT, SDL_WINDOW_SHOWN);


	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		close_program(-1);
	}
	
	if(window == NULL) {
		printf("Couldn't open video mode: %s\n", SDL_GetError());
		close_program(-1);
	}
	
	printf("done!\n");
	
	window_surface = SDL_GetWindowSurface(window);
}

void close_program(int error_code) {
	free_surfaces();
	SDL_FreeSurface(window_surface);
	SDL_DestroyWindow(window);

	//Free the music
	Mix_FreeMusic(music);
	music = NULL;

	IMG_Quit();
	Mix_Quit();
	SDL_Quit();
	exit(error_code);
}

void init_colors(SDL_PixelFormat* format) {
	black = SDL_MapRGB(format, 0, 0, 0);
	white = SDL_MapRGB(format, 255, 255, 255);
}

void load_images(void) {
	int i;

	tetris_logo = load_surface("./img/logos/logo2.png", FALSE);
	pause_img = load_surface("./img/pause.png", FALSE);
	level_img = load_surface("./img/level.png", FALSE);
	score_img = load_surface("./img/score.png", FALSE);
	game_over_img = load_surface("./img/game_over.png", FALSE);
	on_img = load_surface("./img/on.png", FALSE);
	off_img = load_surface("./img/off.png", FALSE);

	level_digits[0] = load_surface("./img/level_digits/0.bmp", FALSE);
	level_digits[1] = load_surface("./img/level_digits/1.bmp", FALSE);
	level_digits[2] = load_surface("./img/level_digits/2.bmp", FALSE);
	level_digits[3] = load_surface("./img/level_digits/3.bmp", FALSE);
	level_digits[4] = load_surface("./img/level_digits/4.bmp", FALSE);
	level_digits[5] = load_surface("./img/level_digits/5.bmp", FALSE);
	level_digits[6] = load_surface("./img/level_digits/6.bmp", FALSE);
	level_digits[7] = load_surface("./img/level_digits/7.bmp", FALSE);
	level_digits[8] = load_surface("./img/level_digits/8.bmp", FALSE);
	level_digits[9] = load_surface("./img/level_digits/9.bmp", FALSE);

	score_digits[0] = load_surface("./img/digits/0.png", FALSE);
	score_digits[1] = load_surface("./img/digits/1.png", FALSE);
	score_digits[2] = load_surface("./img/digits/2.png", FALSE);
	score_digits[3] = load_surface("./img/digits/3.png", FALSE);
	score_digits[4] = load_surface("./img/digits/4.png", FALSE);
	score_digits[5] = load_surface("./img/digits/5.png", FALSE);
	score_digits[6] = load_surface("./img/digits/6.png", FALSE);
	score_digits[7] = load_surface("./img/digits/7.png", FALSE);
	score_digits[8] = load_surface("./img/digits/8.png", FALSE);
	score_digits[9] = load_surface("./img/digits/9.png", FALSE);

	brick[0] = load_surface("./img/bricks/brick0.png", FALSE);
	brick[1] = load_surface("./img/bricks/brick1.png", FALSE);
	brick[2] = load_surface("./img/bricks/brick2.png", FALSE);
	brick[3] = load_surface("./img/bricks/brick3.png", FALSE);
	brick[4] = load_surface("./img/bricks/brick4.png", FALSE);
	brick[5] = load_surface("./img/bricks/brick5.png", FALSE);
	brick[6] = load_surface("./img/bricks/brick6.png", FALSE);

	brick_size = brick[0]->w;

	if(brick[0]->w != brick[0]->h) {
		printf("brick 0 isn't square!\n");
		close_program(-1);
	}

	if(brick_size*20 > VIDEO_HEIGHT - 10) {
		printf("brick_size is too large!\n");
		close_program(-1);
	}

	for(i = 1; i < 7; i++)
		if(brick[i]->w != brick_size || brick[i]->h != brick_size) {
			printf("brick %i is the wrong size!\n", i);
			close_program(-1);
		}
}

void load_music() {
	music = Mix_LoadMUS("./audio/music1.wav");
	if (music == NULL) {
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		close_program(-1);
	}
}

void display_tetris_logo(SDL_Surface *window) {
	SDL_Rect img_rect;
	img_rect.x = 30, img_rect.y = 40;
	img_rect.w = tetris_logo->w, img_rect.h = tetris_logo->h;
	
	SDL_BlitSurface(tetris_logo, NULL, window, &img_rect);
}

void draw_game_area(SDL_Surface *window) {
	draw_rect(window, area_x-1, area_y-1, AREA_WIDTH+2, AREA_HEIGHT+2, white);
}

void draw_level_area(SDL_Surface *window) {
	SDL_Rect img_rect;

	img_rect.x = LEVEL_AREA_X + 10, img_rect.y = LEVEL_AREA_Y + 10;
	img_rect.w = level_img->w, img_rect.h = level_img->h;
	SDL_BlitSurface(level_img, NULL, window, &img_rect);

	draw_rect(window, LEVEL_AREA_X, LEVEL_AREA_Y, 190, 140, white);
}

void draw_score_area(SDL_Surface *window) {
	SDL_Rect img_rect;

	img_rect.x = SCORE_AREA_X, img_rect.y = SCORE_AREA_Y;
	img_rect.w = score_img->w, img_rect.h = score_img->h;

	SDL_BlitSurface(score_img, NULL, window, &img_rect);

	display_digits(window, score_digits, "0",
		img_rect.x + (img_rect.w - score_digits[0]->w)/2,
		img_rect.y + img_rect.h + 10);

	draw_rect(window, img_rect.x - 10, img_rect.y - 10, 190, 120, white);
}

