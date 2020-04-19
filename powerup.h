//
//  powerup.h
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#ifndef powerup_h
#define powerup_h

#include <SDL2/SDL_rect.h>
#include "entity.h"

#define PU_SIZE		16
#define PU_SPEED	2

typedef enum
{
	speedup,	// 1 in 5	+
	slowdown,	// 1 in 5	-
	lengthener,	// 1 in 10	<>
	shortener,	// 1 in 10	><
	multiball,	// 1->2, 1 in 15	:
	stickyball, // stick to paddle, 1 in 15	*
	oneup,		// 1 in 20	+1
	superball,	// block plow-thru, 1 in 20, square
	laser,		// space invaders, 1 in 30	^
	threeup,	// 1 in 50	+3
	
	NUM_POWERUPS,
	none
} putype_t;

typedef struct powerup_s
{
	putype_t	type;
	int			flag;	// flag to apply to paddle
	textlabel_t	image;	// holds draw location
	textlabel_t shadow;
	
	// powerup's center
	float		x;
	float		y;

	// hit box
	float		top;
	float		bottom;
	float		left;
	float		right;
	
	bool		active;
	
	struct powerup_s * next;
} powerup_t;

// constant effects
typedef enum
{
	EF_MULTI	= 1,
	EF_STICKY	= 2,
	EF_LASER	= 4,
	EF_SUPER	= 8,
} effect_t;

void InitPowerupImages (void);

putype_t RandomPowerUp (void);
void DropPowerup( block_t * bl );
powerup_t * RemovePowerup (powerup_t * p);
void RemoveAllPowerups (void);

void UpdatePowerups (entity_t * player);
void DrawPowerups (void);
powerup_t * EatPowerup (powerup_t * p, entity_t * player);


#endif /* powerup_h */
