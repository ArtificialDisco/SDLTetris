#ifndef TETRIS_H
#define TETRIS_H

#include <SDL.h>

#define DROPDOWN_INTERVAL 300
#define TICK_INTERVAL 30

#define AREA_WIDTH 270
#define AREA_HEIGHT 540

#define VIDEO_WIDTH 800
#define VIDEO_HEIGHT 600

enum {
	UP = 1,
	RIGHT,
	DOWN,
	LEFT
};

enum {
	NO_SHAPE = 0,
	I_SHAPE = 1,
	T_SHAPE,
	S_SHAPE,
	Z_SHAPE,
	L_SHAPE,
	J_SHAPE,
	O_SHAPE
};

typedef struct {
	int type;
	int x[4], y[4];
	char color;
} Piece;

typedef struct {
	int x[4];
	int y[4];
} Shape;

int min_int(int a[], int n);
int max_int(int a[], int n);
Uint32 time_left(void);

void insert_brick(SDL_Surface *window, int xpos, int ypos, int x, int y,
	char color);
void remove_brick(SDL_Surface *window, int x, int y);

void copy_piece(Piece *to, Piece *from);
void update_piece(SDL_Surface *window, Piece *old_p, Piece *new_p);
int new_piece(SDL_Surface *window, int xpos, int ypos, Piece *p, int type);
int move_piece(SDL_Surface *window, Piece *p, int dir, int n);
void rotate_left(Piece *p, int cx, int cy, int n);
void rotate_piece(SDL_Surface *window, Piece *p);

int shortest_dist_down(Piece *p);
int shortest_dist_right(Piece *p);
int shortest_dist_left(Piece *p);

void clear_board(void);
void write_to_board(Piece *p, char color);
void update_board(SDL_Surface *window, int update);

int clear_lines(SDL_Surface *window);
SDL_Surface *toggle_fullscreen(SDL_Surface *window);
void toggle_pause(SDL_Surface *window, Piece *current_p, int *pause);

int calc_score(int level, int lines);
void display_score(SDL_Surface *window, int score);
void zero_score(SDL_Surface *window, int *score);

void display_level(SDL_Surface *window, int level);
int calc_level(int nlines, int current);
int dropdown_interval(int level);

#endif //TETRIS_H
