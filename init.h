#ifndef INIT_H
#define INIT_H

#include <SDL.h>

#define SCORE_AREA_X 40
#define SCORE_AREA_Y 270

#define LEVEL_AREA_X 30
#define LEVEL_AREA_Y 100

SDL_Surface* load_surface(const char* path,
	int optimize);
void init_colors(SDL_PixelFormat *format);
void display_tetris_logo(SDL_Surface *window);
void load_images(void);
void init_sdl();
void close_program(int error_code);
void draw_game_area(SDL_Surface *window);
void draw_score_area(SDL_Surface* window);
void draw_level_area(SDL_Surface* window);
void load_music();


#endif //INIT_H

