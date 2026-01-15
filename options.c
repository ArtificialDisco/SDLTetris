#include <SDL.h>
#include <SDL_image.h>

#include <stdio.h>

#include "button.h"
#include "drawing.h"
#include "tetris.h"
#include "useful.h"

#define OPTIONS_DLG_X VIDEO_WIDTH - 237
#define OPTIONS_DLG_Y area_y

extern Uint32 white, black;
extern const int area_y;
extern int brick_size;
extern int next_time;
extern int video_flags;
extern int nbuttons;
extern int fullscreen;

extern SDL_Surface *on_img, *off_img;
extern SDL_Window* window;

Button new_button, options_button, pause_button, fullscreen_button,
	quit_button;

void create_menu_dlg(SDL_Surface *window)
{
	int xpos = OPTIONS_DLG_X, ypos = OPTIONS_DLG_Y;

	draw_rect(window, xpos, area_y, 210, 180, white);
	
	create_button(window, &new_button, xpos+10, ypos+10,
		"./img/buttons/new_game.png", NULL,
		"./img/buttons/new_game_pressed.png");
	create_button(window, &options_button, xpos + 10, ypos + 50,
		"./img/buttons/options.png", NULL,
		"./img/buttons/options_pressed.png");
	create_button(window, &pause_button, xpos+10, ypos+90,
		"./img/buttons/pause.png", NULL,
		"./img/buttons/pause_pressed.png");
	create_button(window, &quit_button, xpos+10, ypos+130,
		"./img/buttons/quit.png", NULL,
		"./img/buttons/quit_pressed.png");

	//draw_rect(window, xpos, ypos + 210, xpos + 6*brick_size,
	//	ypos + 210 + 4*brick_size, white);

	draw_rect(window, xpos, ypos + 210, 6*brick_size, 4*brick_size, white);
}

void show_next_piece(SDL_Surface *window, int type)
{
	int xpos = OPTIONS_DLG_X, ypos = area_y + 210;
	int width = 6*brick_size, height = 4*brick_size;
	char color;
	SDL_Surface *img;
	Piece next;

	static Piece *last = NULL;
	SDL_Rect r = {xpos + 1, ypos + 1, width - 1, height - 1};
	
	SDL_FillRect(window, &r, black);

	if(type == O_SHAPE)
		new_piece(window, xpos - 2*brick_size, ypos, &next, type);
	else
		new_piece(window, xpos - 3*brick_size, ypos, &next, type);

	/*if (last != NULL)
		SDL_UpdateRect(window, r.x, r.y, width, height);
	else {
		SDL_UpdateRect(window, r.x, r.y, width, height);
	}*/
	last = &next;
}

void display_on(SDL_Surface *window)
{
	
}

void options_dlg(SDL_Surface *window_surface)
{
	int width = 400, height = 300;
	int xpos = (window_surface->w - width)/2, ypos = (window_surface->h - height)/2;
	int exit = FALSE;
	int fullscreen_value = fullscreen;
	SDL_Event event;
	SDL_Surface *backup_window = SDL_ConvertSurface(window_surface, window_surface->format, 0);
	SDL_Surface* on_off;

	SDL_Rect opt_rect = {xpos, ypos, width+1, height+1};
	SDL_Rect fill_rect = {xpos+1, ypos+1, width-1, height-1};
	SDL_Rect on_off_rect;

	draw_rect(window_surface, xpos, ypos, width, height, white);
	SDL_FillRect(window_surface, &fill_rect, black);

	create_button(window_surface, &fullscreen_button, xpos+30, ypos+30,
		"./img/buttons/fullscreen.png", NULL,
		"./img/buttons/fullscreen_pressed.png");

	if (fullscreen_value) {
		on_off = on_img;
		fullscreen_value = TRUE;
	} else {
		on_off = off_img;
		fullscreen_value = FALSE;
	}

	on_off_rect.x = xpos + fullscreen_button.rect.w + 50,
		on_off_rect.y = ypos + 30;
	on_off_rect.w = off_img->w, on_off_rect.h = off_img->h;

	SDL_BlitSurface(on_off, NULL, window_surface, &on_off_rect);

	while(!exit) {
		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					exit = TRUE;
				}
			} else if(event.type == SDL_MOUSEBUTTONDOWN) {
				get_button_press(window_surface, &event);
			} else if(event.type == SDL_MOUSEBUTTONUP) {
				if(is_pressed(&fullscreen_button, &event)) {
				  fullscreen_value = !fullscreen_value;
					if(fullscreen_value) {
						on_off = on_img;
					} else {
						on_off = off_img;
					}

					SDL_FillRect(window_surface, &on_off_rect, black);
					SDL_BlitSurface(on_off, NULL, window_surface, &on_off_rect);
					on_off_rect.w = off_img->w;
	
					release_buttons(window_surface);
				}
			}
		
		}
		SDL_UpdateWindowSurface(window);
		next_time += TICK_INTERVAL;
		SDL_Delay(time_left());
	}
  if (fullscreen_value != fullscreen)
		toggle_fullscreen(window);

	delete_button(&fullscreen_button);

	SDL_BlitSurface(backup_window, &opt_rect, window_surface, &opt_rect);
}

