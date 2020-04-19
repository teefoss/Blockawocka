#ifndef breakout_h
#define breakout_h

#include <SDL2/SDL.h>
#include "text.h"
#include "entity.h"

#define BLOCK_W			48
#define BLOCK_H			12
#define BLOCK_ROWS		8
#define BLOCK_COLS		14

#define BOARD_Y			64
#define WINDOW_W		(BLOCK_COLS * BLOCK_W)
#define WINDOW_H		WINDOW_W

#define MS_PER_FRAME	16

#define PLAYER_SPEED	5.0f
#define PLAYER_W		(BLOCK_W * 1.5f)
#define PLAYER_H		(BLOCK_H - 2)
#define PLAYER_Y		(WINDOW_H - PLAYER_H * 4)

#define BALL_SPEED		3.0f
#define BALL_SIZE		(BLOCK_H - 2)
#define BALL_MIN_X		(BALL_SIZE / 2)
#define BALL_MAX_X		(WINDOW_W - BALL_SIZE / 2)
#define BALL_MIN_Y		(BALL_SIZE / 2)

#define LASER_SPEED		10.0f
#define LASER_DELAY		15

#define SHAWDOW_OFFSET	(BLOCK_H / 2)
#define SHADOW_OPACITY	128

#define clamp(x,min,max)	x = x < min ? min : (x > max ? max : x)
#define sign(x)				(x) < 0 ? -1 : ((x) > 0 ? 1 : 0)

typedef enum
{
	dead,
	weak,
	normal,
	strong,
	mega,
	
	numtypes
} blocktype_t;

typedef struct
{
	blocktype_t type;
	SDL_Rect	rect;
	
	// hit box
	int			top;
	int			bottom;
	int			left;
	int			right;
} block_t;

typedef enum
{
	top 	= 1,
	bottom	= 2,
	left	= 4,
	right	= 8,
} collision_t;

extern SDL_Renderer * renderer;
extern SDL_Color dark_colors[NUMCOLORS];
extern SDL_Color bright_colors[NUMCOLORS];

extern entity_t * firstball;

void SetPlayerLives (int newval);
void DrawShadow (SDL_Rect rect);

#endif /* breakout_h */
