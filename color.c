//
//  color.c
//  Breakout
//
//  Created by Thomas Foster on 4/9/20.
//

#include <SDL2/SDL.h>
#include "breakout.h"
#include "color.h"

SDL_Color romero_blue = { 0x04, 0x14, 0x40, SDL_ALPHA_OPAQUE };

SDL_Color dark_colors[NUMCOLORS] = {
    {   0,   0,   0, SDL_ALPHA_OPAQUE },
    {   0, 170, 170, SDL_ALPHA_OPAQUE },     // blue
    {   0, 170,   0, SDL_ALPHA_OPAQUE },     // green
    { 170, 170,   0, SDL_ALPHA_OPAQUE },    // yellow
    { 170,   0,   0, SDL_ALPHA_OPAQUE },     // red
    { 170,   0, 170, SDL_ALPHA_OPAQUE },     // purple
    { 170, 170, 170, SDL_ALPHA_OPAQUE }        // white
};

SDL_Color bright_colors[NUMCOLORS] = {
    {   0,   0,   0, SDL_ALPHA_OPAQUE },    // black
    {  85, 255, 255, SDL_ALPHA_OPAQUE },     // blue
    {  85, 255,  85, SDL_ALPHA_OPAQUE },     // green
    { 255, 255,  85, SDL_ALPHA_OPAQUE },    // yellow
    { 255,  85,  85, SDL_ALPHA_OPAQUE },     // red
    { 255,  85, 255, SDL_ALPHA_OPAQUE },     // purple
    { 255, 255, 255, SDL_ALPHA_OPAQUE }        // white
};

void SetColor (SDL_Color * c)
{
    SDL_SetRenderDrawColor(renderer, c->r, c->g, c->b, c->a);
}

void SetBrightColor (color_t c)
{
    SetColor(&bright_colors[c]);
}

void SetDarkColor (color_t c)
{
    SetColor(&dark_colors[c]);
}
