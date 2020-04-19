//
//  text.h
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#ifndef text_h
#define text_h

#include <SDL2_ttf/SDL_ttf.h>

#define CENTERED -1

typedef struct
{
	TTF_Font * 	ttf_font;
	int 		size;
	int 		scale;
} font_t;

typedef struct
{
	SDL_Rect rect; // render location
	SDL_Texture * texture;
} textlabel_t;

extern font_t font;

void T_InitText (SDL_Renderer * r);
void T_ShutdownText (void);

textlabel_t T_MakeLabel (const char * text, int x, int y);
textlabel_t T_MakeLabel_d (const char * text, int d, int x, int y);
void T_DestroyLabel (textlabel_t * l);
void T_RenderLabel (textlabel_t * l);

#endif /* text_h */
