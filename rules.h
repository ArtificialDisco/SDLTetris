#ifndef RULES_H
#define RULES_H

#include <SDL.h>
#include <SDL_image.h>

int clear_lines(SDL_Surface *window);
int calc_score(int level, int lines);
void display_score(SDL_Surface *window, int score);
void zero_score(SDL_Surface *window, int *score);
void display_level(SDL_Surface *window, int level);
int calc_level(int nlines, int current);
int dropdown_interval(int level);

#endif //RULES_H
