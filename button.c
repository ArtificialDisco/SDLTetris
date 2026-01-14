#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "button.h"
#include "useful.h"

Button *button_list[MAX_BUTTONS];
int nbuttons = 0;

int button_pressed(SDL_Surface *window, SDL_Event *event, Button *button)
{
	if(inside_rect(&button->rect, event->button.x, event->button.y)
	  && event->type == SDL_MOUSEBUTTONDOWN) {
		if(button->press_img != NULL)
			set_img(window, &button->rect, button->press_img);
		button->pressed = TRUE;
		return TRUE;
	}
	return FALSE;
}

Button *get_button_press(SDL_Surface *window, SDL_Event *event)
{
	int i;
	for(i = 0; i < nbuttons; i++)
		if(inside_rect(&button_list[i]->rect, event->button.x, event->button.y)
		  && event->type == SDL_MOUSEBUTTONDOWN) {
			if(button_list[i]->press_img != NULL)
				set_img(window, &button_list[i]->rect,
					button_list[i]->press_img);
			button_list[i]->pressed = TRUE;
			return button_list[i];
		}
	return NULL;
}

int is_pressed(Button *button, SDL_Event *event)
{
	if(inside_rect(&button->rect, event->button.x, event->button.y)
	  && button->pressed)
		return TRUE;
	return FALSE;
}

void release_buttons(SDL_Surface *window)
{
	int i;
	for(i = 0; i < nbuttons; i++)
		if(button_list[i]->pressed) {
			button_list[i]->pressed = FALSE;
			set_img(window, &button_list[i]->rect, button_list[i]->img);
		}
}

int button_clicked(SDL_Surface *window, SDL_Event *event, Button *button)
{
	if(inside_rect(&button->rect, event->button.x, event->button.y)) {
		if(button->press_img != NULL)
			set_img(window, &button->rect, button->press_img);
		
		//SDL_CreateThread(wait_button, (void*)event);
		
		while(event->type != SDL_MOUSEBUTTONUP) {
			if(event->type == SDL_KEYDOWN
			  && event->key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			SDL_PollEvent(event);
		}
		if(button->hover_img != NULL)
			set_img(window, &button->rect, button->hover_img);
		else
			set_img(window, &button->rect, button->img);
		
		if(inside_rect(&button->rect, event->button.x, event->button.y)) 
			return TRUE;
	}
	return FALSE;
}

Button *get_button_click(SDL_Surface *window, SDL_Event *event)
{
	int i;
	
	for(i = 0; i < nbuttons; i++)
		if(button_clicked(window, event, button_list[i]))
			return button_list[i];
	return NULL;
}

void create_button(SDL_Surface *dest, Button *button, int x, int y,
	char *img, char *hover_img, char *press_img)
{
	button->rect.x = x;
	button->rect.y = y;

	button->hover = FALSE;
	button->pressed = FALSE;

	button->img = IMG_Load(img);
	if(hover_img != NULL)
		button->hover_img = IMG_Load(hover_img);
	else
		button->hover_img = NULL;

	if(press_img != NULL)
		button->press_img = IMG_Load(press_img);
	else
		button->press_img = NULL;
	
	button->rect.w = button->img->w;
	button->rect.h = button->img->h;
		
	SDL_BlitSurface(button->img, NULL, dest, &button->rect);

	button_list[nbuttons++] = button;
}

void delete_button(Button *button)
{
	int i, j;
	for(i = 0; i < nbuttons; i++)
		if(button_list[i] == button) {
			button->rect.x = button->rect.y = 0;
			button->rect.w = button->rect.h = 0;
			button->img = button->hover_img = button->press_img = NULL;
		
			for(j = i; j < nbuttons-1; j++)
				button_list[j] = button_list[j+1];
			nbuttons--;
		}
}

int buttons_equal(Button b1, Button b2)
{
	//if b1 and b2 share same location and size
	//they're 'equal'
	if(b1.rect.x == b2.rect.x && b1.rect.y == b2.rect.y
	  && b1.rect.w == b2.rect.w && b1.rect.h == b2.rect.h)
		return TRUE;
	return FALSE;
}

void update_buttons(SDL_Surface *window)
{
	int i;
	int x, y;

	SDL_GetMouseState(&x, &y);

	for(i = 0; i < nbuttons; i++) {
		if(inside_rect(&button_list[i]->rect, x, y)
		  && !button_list[i]->hover) {
			set_img(window, &button_list[i]->rect, button_list[i]->hover_img);
			button_list[i]->hover = TRUE;
		} else if(!inside_rect(&button_list[i]->rect, x, y)
		  && button_list[i]->hover) {
			set_img(window, &button_list[i]->rect, button_list[i]->img);
			button_list[i]->hover = FALSE;
		}
	}		
}

