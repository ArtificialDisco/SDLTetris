#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "button.h"
#include "drawing.h"
#include "init.h"
#include "options.h"
#include "tetris.h"
#include "useful.h"

const int area_x = (VIDEO_WIDTH - AREA_WIDTH) / 2;
const int area_y = (VIDEO_HEIGHT - AREA_HEIGHT) / 2;

char board[20][11];
	
int fullscreen = FALSE;
int needs_redraw = FALSE;

//colors, bricks and digits - initialized in init.c
extern Uint32 black, white;
extern SDL_Surface *brick[7], *pause_img, *level_img, *score_img,
	/**score_digits[0], *level_digits[10], */ *game_over_img;
extern int brick_size;

extern Mix_Music* music;

//these are the actual pieces
Shape t_shape = {{0, 1, 2, 1}, {0, 0, 0, 1}};
Shape s_shape = {{1, 2, 0, 1}, {0, 0, 1, 1}};
Shape z_shape = {{0, 1, 1, 2}, {0, 0, 1, 1}};
Shape i_shape = {{0, 1, 2, 3}, {0, 0, 0, 0}};
Shape l_shape = {{0, 1, 2, 0}, {0, 0, 0, 1}};
Shape j_shape = {{0, 1, 2, 2}, {0, 0, 0, 1}};
Shape o_shape = {{0, 1, 0, 1}, {0, 0, 1, 1}};

extern Button new_button, options_button, pause_button, quit_button;
extern Uint32 next_time;

extern SDL_Window* window;
extern SDL_Surface* window_surface;

void insert_brick(SDL_Surface *window, int xpos, int ypos, int x, int y,
	char color)
{
	SDL_Surface *img = NULL;
	SDL_Rect img_rect = {
	  xpos + x*brick_size,
		ypos + y*brick_size,
		brick_size,
		brick_size
	};
	
	switch(color) {
	case 'y':
		img = brick[0];
		break;
	case 'r':
		img = brick[1];
		break;
	case 'b':
		img = brick[2];
		break;
	case 'g':
		img = brick[3];
		break;
	case 'B':
		img = brick[4];
		break;
	case 'o':
		img = brick[5];
		break;
	case 'l':
		img = brick[6];
		break;
	};
	SDL_BlitSurface(img, NULL, window, &img_rect);
}

void remove_brick(SDL_Surface *window, int x, int y)
{
	int area_x, area_y;
	SDL_Rect img_rect;

	area_x = (VIDEO_WIDTH - AREA_WIDTH)/2;
	area_y = (VIDEO_HEIGHT - AREA_HEIGHT)/2;
	
	img_rect.x = area_x + x*brick_size;
	img_rect.y = area_y + y*brick_size;
	img_rect.w = img_rect.h = brick_size;

	SDL_FillRect(window, &img_rect, black);
}

void copy_piece(Piece *to, Piece *from)
{
	int i;
	to->type = from->type;
	for(i = 0; i < 4; i++) {
		to->x[i] = from->x[i];
		to->y[i] = from->y[i];
	}
}

void update_piece(SDL_Surface *window, Piece *old_p, Piece *new_p)
{
	int x1, y1, x2, y2, i;

	for(i = 0; i < 4; i++)
		remove_brick(window, old_p->x[i], old_p->y[i]);
	for(i = 0; i < 4; i++)
		insert_brick(window, area_x, area_y, new_p->x[i], new_p->y[i],
			new_p->color);

	x1 = MIN(min_int(old_p->x, 4), min_int(new_p->x, 4));
	y1 = MIN(min_int(old_p->y, 4), min_int(new_p->y, 4));
	x2 = MAX(max_int(old_p->x, 4), max_int(new_p->x, 4));
	y2 = MAX(max_int(old_p->y, 4), max_int(new_p->y, 4));
}

int new_piece(SDL_Surface *window, int xpos, int ypos, Piece *p, int type)
{
	int i;
	Shape *shape = NULL;

	if(type < 1 || type > 7)
		return type;
	
	p->type = type;
	
	switch(type) {
	case T_SHAPE:
		shape = &t_shape; p->color = 'y';
		break;
	case S_SHAPE:
		shape = &s_shape; p->color = 'b';
		break;
	case Z_SHAPE:
		shape = &z_shape; p->color = 'B';
		break;
	case I_SHAPE:
		shape = &i_shape; p->color = 'g';
		break;
	case L_SHAPE:
		shape = &l_shape; p->color = 'o';
		break;
	case J_SHAPE:
		shape = &j_shape; p->color = 'r';
		break;
	case O_SHAPE:
		shape = &o_shape; p->color = 'l';
		break;
	default:
	  shape = NULL;
		return -1; 
	}

	for(i = 0; i < 4; i++) {
		p->x[i] = 4 + shape->x[i];
		p->y[i] = 1 + shape->y[i];
	}

	for(i = 0; i < 4; i++)
		if(board[p->y[i]][p->x[i]] != ' ')
			return -1;

	for(i = 0; i < 4; i++)
		insert_brick(window, xpos, ypos, p->x[i], p->y[i], p->color);

	return type;
}

int move_piece(SDL_Surface *window, Piece *p, int dir, int n)
{
	int i;
	Piece old_p;

	if(dir < 1 || dir > 4)
		return -1;

	for(i = 0; i < 4; i++) {
		if(p->x[i] < 0 || p->y[i] < 0 || p->x[i] > 9 || p->y[i] > 19)
			return -1;
	}

	if(p->type == NO_SHAPE)
		return -1;

	copy_piece(&old_p, p);
	if(dir == LEFT) {
		p->x[0] -= n, p->x[1] -= n;
		p->x[2] -= n, p->x[3] -= n;
	} else if(dir == RIGHT) {
		p->x[0] += n, p->x[1] += n;
		p->x[2] += n, p->x[3] += n;
	} else if(dir == DOWN) {
		p->y[0] += n, p->y[1] += n;
		p->y[2] += n, p->y[3] += n;
	}
	
	for(i = 0; i < 4; i++) {
		if(board[p->y[i]][p->x[i]] != ' ') {
			copy_piece(p, &old_p);
			return -1;
		}
		  
		if(p->x[i] < 0 || p->y[i] < 0 || p->x[i] > 9 || p->y[i] > 19) {
			copy_piece(p, &old_p);
			return -1;
		}
	}
	
	update_piece(window, &old_p, p);
	
	return dir;
}

void rotate_left(Piece *p, int cx, int cy, int n)
{
	int i;
	Piece new_p;

	if(n > 1)
		rotate_left(p, cx, cy, n-1);

	copy_piece(&new_p, p);

	for(i = 0; i < 4; i++) {
		if(p->x[i] == cx - 1) {
			if(p->y[i] == cy - 1 || p->y[i] == cy) {
				new_p.x[i] = p->x[i]; new_p.y[i] = p->y[i] + 1;
			} else if(p->y[i] == cy + 1) {
				new_p.x[i] = p->x[i] + 1; new_p.y[i] = p->y[i];
			}
		} else if(p->x[i] == cx) {
			if(p->y[i] == cy - 1) {
				new_p.x[i] = p->x[i] - 1; new_p.y[i] = p->y[i];
			} else if(p->y[i] == cy + 1) {
				new_p.x[i] = p->x[i] + 1; new_p.y[i] = p->y[i];
			}
		} else if(p->x[i] == cx + 1) {
			if(p->y[i] == cy - 1) {
				new_p.x[i] = p->x[i] - 1; new_p.y[i] = p->y[i];
			} else if(p->y[i] == cy || p->y[i] == cy + 1) {
				new_p.x[i] = p->x[i]; new_p.y[i] = p->y[i] - 1;
			}
		}
	}

	copy_piece(p, &new_p);
}

void rotate_piece(SDL_Surface *window, Piece *p)
{
	int miny, minx, i, cx, cy;
	Piece old_p;
	
	if(p->type == O_SHAPE)
		return;
	
	copy_piece(&old_p, p);
	switch(p->type) {
	case I_SHAPE:
		if(p->x[0] == p->x[1]) {
			minx = p->x[0];
			miny = min_int(p->y, 4);
			for(i = 0; i < 4; i++)
				p->x[i] = minx + i - 2;
			p->y[0] = p->y[1] = p->y[2] = p->y[3] = miny + 2;
			
		} else if(p->y[0] == p->y[1]) {
			minx = min_int(p->x, 4);
			miny = p->y[0];
			for(i = 0; i < 4; i++)
				p->y[i] = miny + i - 2;
			p->x[0] = p->x[1] = p->x[2] = p->x[3] = minx + 2;
		}
		break;
	case S_SHAPE:
		//take a look at this later!
		if(p->x[2] + 2 == p->x[1]) {
			cx = p->x[3]; cy = p->y[3];
			rotate_left(p, cx, cy, 6);
		} else {
			cx = p->x[3]; cy = p->y[3];
			rotate_left(p, cx, cy, 2);
		}
		break;
	case Z_SHAPE:
		if(p->x[0] + 2 == p->x[3]) {
			cx = p->x[1]; cy = p->y[1];
			rotate_left(p, cx, cy, 6);
		} else {
			cx = p->x[1]; cy = p->y[1];
			rotate_left(p, cx, cy, 2);
		}
		break;
	case L_SHAPE:
	case J_SHAPE:
	case T_SHAPE:
		cx = p->x[1]; cy = p->y[1];
		rotate_left(p, cx, cy, 2);
		break;
	}

	for(i = 0; i < 4; i++) {
		if(board[p->y[i]][p->x[i]] != ' ') {
			copy_piece(p, &old_p);
			return;
		}
		  
		if(p->x[i] < 0 || p->y[i] < 0 || p->x[i] > 9 || p->y[i] > 19) {
			copy_piece(p, &old_p);
			return;
		}
	}

	update_piece(window, &old_p, p);
}

int shortest_dist_down(Piece *p)
{
	int i, j, d[4] = {0, 0, 0, 0};
	for(j = 0; j < 4; j++)
		for(i = 1; i < 20; i++)
			if(board[p->y[j] + i][p->x[j]] != ' '
			  || p->y[j] + i == 20) {
				d[j] = i;
				break;
			}
	
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if((d[i] == 0 || d[i] == 1)
			  && p->y[i] + 1 == p->y[j] && p->x[i] == p->x[j])
				d[i] = 20;

	return min_int(d, 4) - 1;
}

int shortest_dist_right(Piece *p)
{
	int i, j, d[4] = {0, 0, 0, 0};
	for(j = 0; j < 4; j++)
		for(i = 1; i < 10; i++)
			if(board[p->y[j]][p->x[j] + i] != ' '
			  || p->x[j] + i > 20) {
				d[j] = i;
				break;
			}

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if((d[i] == 1 || d[i] == 0) && i != j
			  && (p->x[i] + 1 == p->x[j] && p->y[i] == p->y[j]))
				d[i] = 20;

	return min_int(d, 4) - 1;
}

int shortest_dist_left(Piece *p)
{
	int i, j, d[4] = {0, 0, 0, 0};
	for(j = 0; j < 4; j++)
		for(i = 1; i < 10; i++)
			if(board[p->y[j]][p->x[j] - i] != ' '
			  || p->x[j] - i < 0) {
				d[j] = i;
				break;
			}

	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if((d[i] == 1 || d[i] == 0) && i != j
			  && (p->x[i] - 1 == p->x[j] && p->y[i] == p->y[j]))
				d[i] = 20;

	return min_int(d, 4) - 1;
}

void clear_board(void)
{
	int i;
	for(i = 0; i < 20; i++)
		strcpy(board[i], "          ");
}

void write_to_board(Piece *p, char color)
{
	int i;
	for(i = 0; i < 4; i++)
		board[p->y[i]][p->x[i]] = color;
}

void update_board(SDL_Surface *window, int update)
{
	int i, j;
	for(i = 0; i < 20; i++)
		for(j = 0; j < 10; j++)
			if(board[i][j] != ' ')
				insert_brick(window, area_x, area_y, j, i, board[i][j]);
			else
				remove_brick(window, j, i);
}

void toggle_fullscreen()
{
	fullscreen = !fullscreen;
	if (fullscreen) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	} else {
		SDL_SetWindowFullscreen(window, 0);
	}
	SDL_FreeSurface(window_surface);
	window_surface = SDL_GetWindowSurface(window);
	needs_redraw = TRUE;
}

void toggle_pause(SDL_Surface *window, Piece *current_p, int *pause)
{
	int i;
	SDL_Rect r = {area_x, area_y, 10*brick_size-1,
		20*brick_size-1};
	SDL_Rect img_rect = { area_x + 5*brick_size - pause_img->w/2,
		area_y  + 8*brick_size - pause_img->h/2,
		pause_img->w, pause_img->h};

	if(*pause == FALSE) {
		SDL_FillRect(window, &r, black);
		SDL_BlitSurface(pause_img, NULL, window, &img_rect);
		*pause = TRUE;
	} else {
		SDL_FillRect(window, &img_rect, black);
		update_board(window, FALSE);
		for(i = 0; i < 4; i++)
			insert_brick(window, area_x, area_y, current_p->x[i],
				current_p->y[i], current_p->color);
		*pause = FALSE;
	}
}

void display_game_over(SDL_Surface *window)
{
	SDL_Rect img_rect = { area_x + 5*brick_size - game_over_img->w/2,
		area_y  + 9*brick_size - game_over_img->h/2,
		game_over_img->w, game_over_img->h};

	SDL_BlitSurface(game_over_img, NULL, window, &img_rect);
}

void draw_everything() {

}

unsigned int LCG(unsigned int min, unsigned int max)
{
	static int seed = FALSE;
	if(!seed) {
		srand(time(NULL));
		seed = TRUE;
	}
	return (rand() % max) + min;
}

int main(int argc, char** argv) {
	SDL_Event event;
	Piece p;

	int level = 0, score = 0, nlines = 0, nlines_total = 0, prev_score = 0;
	int tmp;

	int dropdown;
	int r, next, pause = FALSE, game_running = TRUE;
	int left_pressed = FALSE, up_pressed = FALSE,
		down_pressed = FALSE, right_pressed = FALSE,
		space_pressed = FALSE, p_pressed = FALSE;

	unsigned int (*randint)(unsigned int, unsigned int);

	randint = LCG;

	clear_board();
	init_sdl();
	init_colors(window_surface->format);
	load_images();
	load_music();
	display_tetris_logo(window_surface);
	draw_game_area(window_surface);
	draw_level_area(window_surface);
	draw_score_area(window_surface);
	create_menu_dlg(window_surface);
	display_level(window_surface, level);
	dropdown = dropdown_interval(level) / TICK_INTERVAL;
	
	new_piece(window_surface, area_x, area_y, &p, randint(1, 7));
	next = randint(1, 7);
	show_next_piece(window_surface, next);

	if (Mix_PlayingMusic() == 0) {
		//Play the music
		Mix_PlayMusic(music, -1);
	}

	r = 0;
	while(1) {
		if(p.type == NO_SHAPE && r == dropdown) {
			if(new_piece(window_surface, area_x, area_y, &p, next) == -1) {
				game_running = FALSE;
				display_game_over(window_surface);
			} else {
				next = (randint(1, 7));
				nlines = clear_lines(window_surface);
				show_next_piece(window_surface, next);
				
				prev_score = score;
			}

			if(nlines > 0) {
				score += calc_score(level, nlines);
				nlines_total += nlines;
				tmp = level;
				level = calc_level(nlines_total, level);
				if(level != tmp)
					display_level(window_surface, level);
				dropdown = dropdown_interval(level) / TICK_INTERVAL;
				r = 0;
			}	
			
			if(prev_score != score)
				display_score(window_surface, score);
			continue;
		}
	
		while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT) {
			close_program(0);
		} else if(event.type == SDL_MOUSEBUTTONDOWN) {
			get_button_press(window_surface, &event);
		} else if(event.type == SDL_MOUSEBUTTONUP) {
			
			if(is_pressed(&new_button, &event)) {
				game_running = TRUE;
				p.type = NO_SHAPE;
				pause = FALSE;
				prev_score = level = nlines_total = 0;
				clear_board();
				update_board(window_surface, TRUE);
				display_score(window_surface, score);
				zero_score(window_surface, &score);
				display_level(window_surface, level);
				dropdown = dropdown_interval(level) / TICK_INTERVAL;
			
			} else if(is_pressed(&options_button, &event) && game_running) {
				release_buttons(window_surface);
				options_dlg(window_surface);
				r = 0;
			} else if(is_pressed(&pause_button, &event) && game_running) {
				toggle_pause(window_surface, &p, &pause);
			} else if(is_pressed(&quit_button, &event)) {
				close_program(0);
			}
			release_buttons(window_surface);

		} else if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					close_program(0);
					break;
				case SDLK_f:
					toggle_fullscreen(window_surface);
					break;
				case SDLK_UP:
					if(!up_pressed && !pause) {
						rotate_piece(window_surface, &p);
						up_pressed = TRUE;
					}
					break;
				case SDLK_LEFT:
					if(!left_pressed && !pause) {
						move_piece(window_surface, &p, LEFT, 1);
						left_pressed = TRUE;
					}
					break;
				case SDLK_RIGHT:
					if(!right_pressed && !pause) {
						move_piece(window_surface, &p, RIGHT, 1);
						right_pressed = TRUE;
					}
					break;
				case SDLK_DOWN:
					if(!down_pressed && !pause) {
						move_piece(window_surface, &p, DOWN, 1);
						down_pressed = TRUE;
					}
					break;
				case ' ':
					if(!space_pressed && !pause) {
						space_pressed = TRUE;
						move_piece(window_surface, &p, DOWN,
							shortest_dist_down(&p));
						write_to_board(&p, p.color);
						p.type = NO_SHAPE;
						r = 0;
					}
					break;
				case SDLK_p:
					if(!p_pressed && game_running) {
						p_pressed = TRUE;
						toggle_pause(window_surface, &p, &pause);
					}
					break;
				case SDLK_m:
					if (Mix_PausedMusic() == 1) {
						Mix_ResumeMusic();
					} else {
						Mix_PauseMusic();
					}
					break;
			}
		} else if(event.type == SDL_KEYUP) {
			switch(event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_k:
					up_pressed = FALSE;
					break;
				case SDLK_LEFT:
				case SDLK_j:
					left_pressed = FALSE;
					break;
				case SDLK_RIGHT:
				case SDLK_l:
					right_pressed = FALSE;
					break;
				case SDLK_DOWN:
				case SDLK_m:
					down_pressed = FALSE;
					break;
				case ' ':
					space_pressed = FALSE;
					break;
				case SDLK_p:
					p_pressed = FALSE;
					break;
			}
		}
		}
		if(r == dropdown && !pause) {
			if(move_piece(window_surface, &p, DOWN, 1) == -1) {
				write_to_board(&p, p.color);
				p.type = NO_SHAPE;
			}
			r = 0;
		} else
			if(!pause && game_running) 
				r++;

		if (needs_redraw) {
			display_tetris_logo(window_surface);
			draw_game_area(window_surface);
			draw_level_area(window_surface);
			draw_score_area(window_surface);
			create_menu_dlg(window_surface);
			display_level(window_surface, level);
			show_next_piece(window_surface, next);
			update_board(window_surface, TRUE);
		  needs_redraw = FALSE;
		}

		SDL_UpdateWindowSurface(window);

		next_time += TICK_INTERVAL;
		SDL_Delay(time_left());
	}
}

