#include <SDL.h>

#define ABS(x) ((x) < 0) ? -(x) : (x)

#define MAX(x,y) ((x) < (y)) ? (y) : (x)
#define MIN(x,y) ((x) < (y)) ? (x) : (y)

#define LOCK(surface) \
	if(SDL_MUSTLOCK(surface)) \
		if(SDL_LockSurface(surface) < 0) { \
			fprintf(stderr, "Can't lock window: %s\n", SDL_GetError()); \
			return; \
		}

#define UNLOCK(surface) \
	if(SDL_MUSTLOCK(surface)) \
		SDL_UnlockSurface(surface);

//functions for updating the screen
void update_from_radius(SDL_Surface*, int, int, int);

//draw without locking/unlocking the surface or updating screen
void putpixel(SDL_Surface*, int, int, Uint32);
void putline(SDL_Surface*, int, int, int, int, Uint32);

//all functions below both lock/unlock the surface and update the screen
void draw_point(SDL_Surface*, int, int, Uint32);
void draw_line(SDL_Surface*, int, int, int, int, Uint32);
void draw_circle(SDL_Surface *surface, int x, int y, int r, Uint32 pixel);
void draw_rect(SDL_Surface*, int, int, int, int, Uint32);
void draw_regular_polygon(SDL_Surface*, int, int, int, int, Uint32);

