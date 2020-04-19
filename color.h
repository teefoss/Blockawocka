//
//  color.h
//  Breakout
//
//  Created by Thomas Foster on 4/9/20.
//

#ifndef color_h
#define color_h

typedef enum
{
    BLACK,
    BLUE,
    GREEN,
    YELLOW,
    RED,
    PURPLE,
    WHITE,
    NUMCOLORS
} color_t;

void SetColor (SDL_Color * c);
void SetBrightColor (color_t c);
void SetDarkColor (color_t c);

#endif /* color_h */
