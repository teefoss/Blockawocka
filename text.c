//
//  text.c
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#include "text.h"
#include "breakout.h"

#define FONT_SIZE 		8
#define FONT_SCALE 		2

font_t font;

static void Error (const char * message)
{
	if (message && *message) {
		puts(message);
	}
	exit(1);
}


void T_InitText (SDL_Renderer * r)
{
	if (TTF_Init() == -1) {
		Error("main: Error. Could not init SDL2_ttf");
	}
	font.ttf_font = TTF_OpenFont("assets/bios.ttf", FONT_SIZE);
	if (!font.ttf_font) {
		Error("InitFont: Error. Could not initialize font");
	}
	font.size = FONT_SIZE;
	font.scale = FONT_SCALE;
}


void T_ShutdownText ()
{
	TTF_CloseFont(font.ttf_font);
	TTF_Quit();
}


//
//	Make / Destroy / Render TextLabel
//	pass -1 for x or y to center on screen
//
textlabel_t T_MakeLabel (const char * text, int x, int y)
{
	SDL_Surface * 	s;
	textlabel_t 	l;
	SDL_Color 		c;
	
	SDL_GetRenderDrawColor(renderer, &c.r, &c.g, &c.b, &c.a);
	s = TTF_RenderText_Solid(font.ttf_font, text, c);
	if (!s) Error("TextLabel: Error. Could not create surface from text");
	
	l.texture = SDL_CreateTextureFromSurface(renderer, s);
	l.rect = (SDL_Rect){ x, y, s->w * FONT_SCALE, s->h * FONT_SCALE };
	if (x == CENTERED) {
		l.rect.x = WINDOW_W / 2 - l.rect.w / 2;
	}
	if (y == CENTERED) {
		l.rect.y = WINDOW_H / 2 - l.rect.h / 2;
	}
	
	SDL_FreeSurface(s);
	
	return l;
}

textlabel_t T_MakeLabel_d (const char * text, int d, int x, int y)
{
	char buffer[80];
	
	sprintf(buffer, "%s%d", text, d);
	return T_MakeLabel(buffer, x, y);
}

void T_DestroyLabel (textlabel_t * l)
{
	SDL_DestroyTexture(l->texture);
}

void T_RenderLabel (textlabel_t * l)
{
	SDL_RenderCopy(renderer, l->texture, NULL, &l->rect);
}
