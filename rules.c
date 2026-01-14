#include <SDL.h>
#include <SDL_image.h>

#include <stdio.h>
#include <string.h>

#include "init.h"
#include "rules.h"
#include "useful.h"
#include "tetris.h"

extern SDL_Surface *level_digits[10], *score_digits[10], *score_img, *level_img;
extern char board[20][11];
extern Uint32 black, white;

int clear_lines(SDL_Surface *window)
{
	int nline = 0, lines_cleared = 0;
	int i, j, k, l;
	int update = FALSE;
	//char old_board[20][11];

	//for(i = 0; i < 20; i++) {
	//	for(j = 0; j < 10; j++)
	//		old_board[i][j] = board[i][j];
	//	old_board[i][10] = '\0';
	//}
	
	for(i = 0; i < 20; i++) {
		for(j = 0; j < 10; j++)
			if(board[i][j] != ' ') {
				nline++;
			}
		if(nline == 10) {
			for(j = 0; j < 10; j++)
				board[i][j] = ' ';
			for(k = i; k > 0; k--)
				for(l = 0; l < 10; l++)
					if(board[k][l] != ' ') {
						board[k+1][l] = board[k][l];
						board[k][l] = ' ';
					}
			update = TRUE;
			lines_cleared++;
		}
		nline = 0;
	}

	//if(lines_cleared < 4) {
	//	for(i = 0; i < 20; i++)
	//		for(j = 0; j < 10; j++)
	//			board[i][j] = old_board[i][j];
	//	update = FALSE;
	//	lines_cleared = 0;
	//}
	
	if(update)
		update_board(window, TRUE);
	
	return lines_cleared;
}

int calc_score(int level, int lines)
{
	switch(lines) {
		case 0:
			return 0;
		case 1:
			return (level+1)*40;
		case 2:
			return (level+1)*100;
		case 3:
			return (level+1)*300;
		case 4:
			return (level+1)*1200;
		default:
			return (level+1)*1200;
	}
}

void display_score(SDL_Surface *window, int score)
{
	char s[10];
	SDL_Rect img_rect, score_rect;

	sprintf(s, "%i", score);
	
	img_rect.x = SCORE_AREA_X, img_rect.y = SCORE_AREA_Y;
	img_rect.w = score_img->w, img_rect.h = score_img->h;
	
	score_rect.w = score_digits[0]->w*strlen(s);
	score_rect.x = img_rect.x + (img_rect.w - score_rect.w)/2;
	score_rect.y = img_rect.y + score_img->h;
	score_rect.h = score_digits[0]->h + 10;

	SDL_FillRect(window, &score_rect, black);
	display_digits(window, score_digits, s, score_rect.x,
		img_rect.y + img_rect.h + 10);

	strcpy(s, "");
}

void zero_score(SDL_Surface *window, int *score)
{
	int len = 0, score_left = *score;
	SDL_Rect img_rect, score_rect;
	
	do {
		score_left /= 10;
		len++;
	} while(score_left > 0);
	
	img_rect.x = SCORE_AREA_X, img_rect.y = SCORE_AREA_Y;
	img_rect.w = score_img->w, img_rect.h = score_img->h;

	score_rect.w = score_digits[0]->w*len;
	score_rect.x = img_rect.x + (img_rect.w - score_rect.w)/2;
	score_rect.y = img_rect.y + score_img->h;
	score_rect.h = score_digits[0]->h + 10;

	SDL_FillRect(window, &score_rect, black);
	display_digits(window, score_digits, "0",
		img_rect.x + (img_rect.w - score_digits[0]->w)/2,
		img_rect.y + img_rect.h + 10);
	
	*score = 0;
	printf("%i\n", len);
}

void display_level(SDL_Surface *window, int level)
{
	SDL_Rect img_rect;

	char s[2];

	sprintf(s, "%i", level);

	img_rect.x = LEVEL_AREA_X + 90 - level_digits[0]->w/2;
	img_rect.y = LEVEL_AREA_Y + 70 + level_img->h/2 - level_digits[0]->h/2;
	img_rect.w = level_digits[0]->w, img_rect.h = level_digits[0]->h;

	SDL_FillRect(window, &img_rect, black);
	
	display_digits(window, level_digits, s, img_rect.x, img_rect.y);
}

int calc_level(int nlines, int current)
{
	int l = MAX(current, (nlines - 1) / 10);
	if(l > 9)
		return 9;
	return l;
}

int dropdown_interval(int level)
{
	return 50*(10 - level);
}

