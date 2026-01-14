#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "useful.h"

int min_int(int a[], int n)
{
	int i, x = a[0];
	for(i = 1; i < n; i++)
		if(a[i] < x)
			x = a[i];
	return x;
}

int max_int(int a[], int n)
{
	int i, x = a[0];
	for(i = 1; i < n; i++)
		if(a[i] > x)
			x = a[i];
	return x;
}

Uint32 next_time;

Uint32 time_left(void)
{
	Uint32 now;
	now = SDL_GetTicks();
	if(next_time <= now)
		return 0;
	else
		return next_time - now;
}

int is_number(char *s)
{
	if(*s == '-')
		s++;

	while(isdigit(*s) || *s == '.')
		s++;
	
	if(*s == '\0') {
		return TRUE;
	}
	return FALSE;
}

double get_double()
{
	double d;

	char ch, s[10];
	int i = 0;

	while((ch = getchar()) != '\n' && i < 10)
		s[i++] = ch;
	
	if(is_number(s)) {
		d = strtod(s, NULL);
		return d;
	}

	printf("error: %s is not a number\n", s);
	exit(1);
}

void shellsort(int v[], int n)
{
	int gap, i, j, tmp;

	for(gap = n/2; gap > 0; gap /= 2)
		for(i = gap; i < n; i++)
			for(j = i - gap; j >= 0 && v[j] > v[j + gap]; j -= gap) {
				tmp = v[j];
				v[j] = v[j+gap];
				v[j+gap] = tmp;
			}
}

int getline(char line[], int maxline)
{
	int i, c;

	for(i = 0; i < maxline && (c = getchar()) != EOF && c != '\n'; i++)
		line[i] = c;
	line[i] = '\0';

	return i;
}

unsigned int random_int(int min, int max)
{
	static FILE *dev_urandom = NULL;
	static int use_stdlib = FALSE;

	unsigned int x;

	//check if /dev/urandom exists, if so - use it else use rand()
	if(!use_stdlib) {
		if((dev_urandom = fopen("/dev/urandom", "r")) == NULL) {
			printf("couldn't open /dev/urandom, using standard library\n");
			use_stdlib = TRUE;
			srand(time(NULL));
			return rand() % (max - min) + min;
		} else {
			fread(&x, 1, sizeof(x), dev_urandom);
			return x % (max - min) + min;
			//return min + (x % (max - min + 1));
		}
	} else {
		return rand() % (max - min) + min;
	}
}

int inside_rect(SDL_Rect *rect, int x, int y)
{
	if(x >= rect->x && x <= rect->x + rect->w
	  && y >= rect->y && y <= rect->y + rect->h)
		return TRUE;
	return FALSE;
}

int rect_collide(SDL_Rect r1, SDL_Rect r2)
{
	//collision at bottom right
	if(r1.x >= r2.x && r1.x <= r2.x + r2.w
	  && r1.y >= r2.y && r1.y <= r2.y + r2.h) {
		return TRUE;
	}

	//collision at top right
	else if(r1.x >= r2.x && r1.x <= r2.x + r2.w
	  && r1.y + r1.h >= r2.y && r1.y + r1.h <= r2.y + r2.h) {
		return TRUE;
	}

	//collision at bottom left
	else if(r1.x + r1.w >= r2.x && r1.x + r1.w <= r2.x + r2.w
	  && r1.y >= r2.y && r1.y <= r2.y + r2.h) {
		return TRUE;
	}
	 
	//collision at top right
	else if(r1.x + r1.w >= r2.x && r1.x + r1.w <= r2.x + r2.w
	  && r1.y + r1.h >= r2.y && r1.y + r1.h <= r2.y + r2.h) {
		return TRUE;
	}
	
	return FALSE;
}

int rect_equal(SDL_Rect r1, SDL_Rect r2)
{
	if(r1.x == r2.x && r1.y == r2.y && r1.w == r2.w && r1.h == r2.h)
		return TRUE;
	return FALSE;
}

void set_img(SDL_Surface *surface, SDL_Rect *rect, SDL_Surface *img)
{
	SDL_BlitSurface(img, NULL, surface, rect);
}

//load image and check if it worked
SDL_Surface *load_image(char *filename)
{
	SDL_Surface *img = IMG_Load(filename);
	if(img == NULL) {
		printf("error: %s\n", SDL_GetError());
		exit(1);
	}
	return img;
}

void update_rect_safe(SDL_Surface *window, int x, int y, int w, int h)
{
	if(x < 0)
		if(x + w < 0) return;
		else x = 0;

	if(y < 0)
		if(y + h < 0) return;
		else y = 0;

	if(x + w > window->w)
		if(x > window->w) return;
		else w = window->w - x;

	if(y + h > window->h)
		if(y > window->h) return;
		else h = window->h - y;
}

void blit_surface_safe(SDL_Surface *src, SDL_Surface *dest, SDL_Rect *rect)
{
	SDL_Rect r;

	if(rect->x < 0)
		if(rect->x + rect->w < 0) return;
		else rect->x = 0;

	if(rect->y < 0)
		if(rect->y + rect->h < 0) return;
		else rect->y = 0;

	if(rect->x + rect->w > dest->w)
		if(rect->x > dest->w) return;
		else rect->w = dest->w - rect->x;

	if(rect->y + rect->h > dest->h)
		if(rect->y > dest->h) return;
		else rect->h = dest->h - rect->y;

	SDL_BlitSurface(src, NULL, dest, rect);
}

void display_digits(SDL_Surface *surface, SDL_Surface *digits[9], char s[], 
	int x, int y)
{
	int i, w, h;
	SDL_Rect img_rect = {x, y, w = digits[0]->w, h = digits[0]->h};

	for(i = 0; s[i] != '\0'; i++) {
		//if(digit['0' + i]->w == w && digits
		set_img(surface, &img_rect, digits[s[i] - '0']);
		img_rect.x += w;
	}
}

