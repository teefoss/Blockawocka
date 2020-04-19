//
//  entity.h
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#ifndef entity_h
#define entity_h

#include <SDL2/SDL_rect.h>
#include "stdbool.h"
#include "vector.h"
#include "color.h"

typedef struct entity_s
{
    // entity's position (center) and velocity
    vector_t    pos;
    vector_t    vel;
	
	// hit box
	float		top;
	float		bottom;
	float		left;
	float		right;
	
	// size
	int 		width;
	int 		height;
	
	// rendering information
	SDL_Rect	rect;
	
	// current effect flags
	int			effects;
    
	int			lives;
	bool		active;
	struct entity_s * next;
} entity_t;

void E_SetPosition (entity_t * e, float x, float y);
void E_Accellerate (entity_t *e);
bool E_Collision (entity_t * e1, entity_t * e2);
void E_DrawEntity (entity_t * e, color_t c);

entity_t * E_AddEntity (entity_t ** first);
void E_RemoveEntity (entity_t * rem, entity_t ** first);
void E_RemoveAll (entity_t ** first);

#endif /* entity_h */
