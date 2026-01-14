#include <SDL.h>
#include <math.h>
#include <stdio.h>

#include "drawing.h"

//update a rect just big enough to circumscribe a circle with radius r
void update_from_radius(SDL_Surface *surface, int x, int y, int r)
{
	int xpos, ypos, xupd, yupd;
	
	xpos = x - r, ypos = y - r;
	xupd = yupd = 2*r;
	
	if(xpos < 0) xupd = 2*r + xpos, xpos = 0;
	if(ypos < 0) yupd = 2*r + ypos, ypos = 0;

	if(xpos+2*r > surface->w)
		xupd = surface->w - xpos;
	if(ypos+2*r > surface->h)
		yupd = surface->h - ypos;
}

//draw a point on screen. don't update screen
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8*)surface->pixels + y*surface->pitch + x*bpp;

	if(x > surface->w || x < 0) return;
	if(y > surface->h || y < 0) return;

	switch(bpp) {
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16*)p = pixel;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32*)p = pixel;
			break;
	}
}

//draw a line from (x1,y1) to (x2,y2). works in any direction.
//don't update screen
void putline(SDL_Surface *surface, int x1, int y1, int x2, int y2,
	Uint32 pixel)
{
	//saves a whole lot of trouble
	if(x1 == x2 && y1 == y2) return;
	
	double r = sqrt(ABS((x1+x2*x2)+(y1+y2*y2)));
	double cosx = (x2-x1) / r;
	double siny = (y2-y1) / r;
	int i;

	LOCK(surface);

	for(i = 0; i < r; i++)
		putpixel(surface, (int)(x1 + (cosx * (double)i)),
			(int)(y1 + (siny * (double)i)), pixel);

	UNLOCK(surface);
}

//call putpixel. update screen.
void draw_point(SDL_Surface * surface, int x, int y, Uint32 pixel)
{
	LOCK(surface);
	
	putpixel(surface, x, y, pixel);
	//SDL_UpdateRect(surface, x, y, 1, 1);
	
	UNLOCK(surface);
}

//call putline. update screen.
void draw_line(SDL_Surface *surface, int x1, int y1, int x2, int y2,
	Uint32 pixel)
{
	LOCK(surface);

	putline(surface, x1, y1, x2, y2, pixel);
	
	UNLOCK(surface);
}

//draw a circle with radius r. update screen.
void draw_circle(SDL_Surface *surface, int x, int y, int r, Uint32 pixel)
{
	double i;
	int xpos, ypos, xupd, yupd;

	LOCK(surface);

	for(i = 0; i < 2*M_PI; i+=0.01) {
		xpos = x + (int)(cos(i)*r);
		ypos = y + (int)(sin(i)*r);
		
		putpixel(surface, xpos, ypos, pixel);
	}

	update_from_radius(surface, x, y, r);
	
	UNLOCK(surface);
}

//draw a rectangle from (x1,y1) to (x2,y2). works in any direction.
//update screen.
/*void draw_rect(SDL_Surface *surface, int x1, int y1, int x2, int y2,
	Uint32 pixel)
{
	int xmax = MAX(x1, x2), ymax = MAX(y1, y2);
	int xmin = MIN(x1, x2), ymin = MIN(y1, y2);

	int xpos, ypos;

	LOCK(surface);

	for(xpos = xmin; xpos <= xmax; xpos++) {
		putpixel(surface, xpos, ymin, pixel);
		putpixel(surface, xpos, ymax, pixel);
	}

	for(ypos = ymin; ypos <= ymax; ypos++) {
		putpixel(surface, xmin, ypos, pixel);
		putpixel(surface, xmax, ypos, pixel);
	}
	
	if(xmin < 0) xmin = 0;
	if(ymin < 0) ymin = 0;
	
	if(xmax >= surface->w) xmax = surface->w-1;
	if(ymax >= surface->h) ymax = surface->h-1;

	SDL_UpdateRect(surface, xmin, ymin, xmax-xmin+1, ymax-ymin+1);
	
	UNLOCK(surface);
}*/

void draw_rect(SDL_Surface *surface, int x, int y, int w, int h,
	Uint32 pixel)
{
	int xpos, ypos;
	
	int i;
	
	LOCK(surface);

	if(x < 0 || x > surface->w)
		fprintf(stderr, "draw_rect: x out of bounds!\n");

	if(y < 0 || y > surface->h)
		fprintf(stderr, "draw_rect: y out of bounds!\n");
	
	if(x + w > surface->w)
		w = surface->w - x;
	
	if(y + h > surface->h)
		h = surface->h - y;
	
	for(i = 0; i <= w; i++) {
		putpixel(surface, x + i, y, pixel);
		putpixel(surface, x + i, y + h, pixel);
	}

	for(i = 1; i < h; i++) {
		putpixel(surface, x, y + i, pixel);
		putpixel(surface, x + w, y + i, pixel);
	}
	
	UNLOCK(surface);
}

//draw a regular polygon, as circumscribed in a circle with radius r, with
//n sides. update screen.
void draw_regular_polygon(SDL_Surface *surface, int x, int y, int r, int n,
	Uint32 color)
{
	int xpos, ypos, last_xpos, last_ypos, xupd, yupd;
	double i;

	LOCK(surface);

	last_xpos = x + (double)r;  //cos(0)*r
	last_ypos = y + 0;  		//sin(0)*r

	for(i = 2*M_PI/n; i <= 2*M_PI+2*M_PI/n; i+=2*M_PI/n) {
		xpos = x + (double)cos(i)*r;
		ypos = y + (double)sin(i)*r;
		
		putline(surface, last_xpos, last_ypos, xpos, ypos, color);
	
		last_xpos = xpos, last_ypos = ypos;
	}

	update_from_radius(surface, x, y, r);
	
	UNLOCK(surface);
}
