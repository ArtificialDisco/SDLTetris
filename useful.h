#ifndef USEFUL_SDL_H
#define USEFUL_SDL_H

#include <SDL.h>
#include <SDL_image.h>

#define MAXLINE 1000

#define TRUE 1
#define FALSE 0

#define MAX(x,y) ((x) < (y)) ? (y) : (x)
#define MIN(x,y) ((x) < (y)) ? (x) : (y)

int min_int(int a[], int n);
int max_int(int a[], int n);
Uint32 time_left(void);

int is_number(char *s);
double get_double();
void shellsort(int c[], int n);
int getline(char line[], int maxline);

int inside_rect(SDL_Rect*, int, int);
int rect_collide(SDL_Rect r1, SDL_Rect r2);
int rect_equal(SDL_Rect r1, SDL_Rect r2);

unsigned int random_int(int min, int max);

void set_img(SDL_Surface*, SDL_Rect*, SDL_Surface*);

SDL_Surface *load_image(char*);
void update_rect_safe(SDL_Surface *window, int x, int y, int w, int h);
void blit_surface_safe(SDL_Surface *src, SDL_Surface *dest, SDL_Rect *rect);

void display_digits(SDL_Surface *surface, SDL_Surface *digits[9], char s[],
	int x, int y);

#endif //USEFUL_SDL_H
