#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>

#define MAX_BUTTONS 50

typedef struct {
	SDL_Rect rect;
	SDL_Surface *img;
	SDL_Surface *hover_img;
	SDL_Surface *press_img;
	int hover;
	int pressed;
} Button;

//check if button was clicked -- which happens if
//event.type == SDL_MOUSEBUTTONDOWN and mouse-pointer is located
//within button->rect
int button_clicked(SDL_Surface *window, SDL_Event *event, Button *button);

//create a button.  button must have an image for normal mode and
//hovering, which is an SDL_Surface*.  button is preferably an empty
//Button struct which gets its value assigned here.
void create_button(SDL_Surface *dest, Button *button, int x, int y,
	char *img, char *hover_img, char *press_img);

int buttons_equal(Button b1, Button b2);
Button *get_button_click(SDL_Surface *window, SDL_Event *event);
void update_buttons(SDL_Surface *window);

void release_buttons(SDL_Surface *window);
int is_pressed(Button *button, SDL_Event *event);
Button *get_button_press(SDL_Surface *window, SDL_Event *event);

void delete_button(Button *button);

#endif //BUTTON_H
