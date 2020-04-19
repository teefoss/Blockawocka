//
//  powerup.c
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#include <stdlib.h>
#include "breakout.h"
#include "powerup.h"
#include "sound.h"

typedef struct
{
    int     chance;
    int     color;
    char    symbol[3];
    int     flag;
    
    textlabel_t image;
    textlabel_t shadow;
} pupdef_t;

pupdef_t pupdefs[NUM_POWERUPS] =
{
    {  3, RED, "+", 0 },            // speedup
    {  3, BLUE, "-", 0 },           // slowdown
    {  6, YELLOW, "<>", 0 },        // lengthener
    {  6, YELLOW, "><", 0 },        // shortener
    {  9, WHITE, ":", 0 },          // multiball
    {  9, GREEN, "*", EF_STICKY },  // stickyball
    { 12, PURPLE, "+1", 0 },        // 1up
    { 12, BLACK, " ", EF_SUPER },   // superball
    { 18, YELLOW, "^", EF_LASER },  // laser
    { 38, PURPLE, "+3", 0 }         // 3up
};

// powerup linked list
powerup_t *first = NULL;


void InitPowerupImages ()
{
	for (int i=0 ; i<NUM_POWERUPS ; i++) {
        SetColor( &bright_colors[pupdefs[i].color] );
        pupdefs[i].image = T_MakeLabel(pupdefs[i].symbol, 0, 0);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SHADOW_OPACITY);
        pupdefs[i].shadow = T_MakeLabel(pupdefs[i].symbol, 0, 0);
	}
}




//
//	NewPowerUp
//	Generate a random powerup or none
//
putype_t RandomPowerUp ()
{
//	DEBUG:
// 	return speedup;
//	return slowdown;
//	return multiball;
//	return stickyball;
//	return oneup;
//	return threeup;
//	return lengthener;
//	return shortener;
//	return superball;
//	return laser;
	
	putype_t p = rand() % NUM_POWERUPS;
    printf(" ...trying to a %d\n", p);
//	return p;
    return (rand() % pupdefs[p].chance == 0) ? p : none;
}


//
//	DropPowerUp
// 	Possibly drop a powerup at block 'bl'
//
void DropPowerup( block_t * bl )
{
	powerup_t * p;
	putype_t	type;
    float x, y;
	
    x = bl->rect.x + BLOCK_W / 2;
    y = bl->rect.y + BLOCK_H / 2;
    
    if ( rand() < 85 ) {
        type = RandomPowerUp();
    } else {
        return;
    }
	
    printf("dropping a %s...\n", pupdefs[type].symbol);
	
	p = malloc(sizeof(powerup_t));
	p->type = type;
	switch (p->type) {
		case stickyball: 	p->flag = EF_STICKY; break;
		case laser: 	 	p->flag = EF_LASER; break;
		case superball:  	p->flag = EF_SUPER; break;
		default: 			p->flag = 0;	break;
	}
	p->x = x;
	p->y = y;
    p->image = pupdefs[p->type].image;
    p->shadow = pupdefs[p->type].shadow;
	p->top = y - p->image.rect.h / 2;
	p->bottom = y + p->image.rect.h / 2;
	p->left = x - p->image.rect.w / 2;
	p->right = x + p->image.rect.w / 2;
	p->image.rect.x = p->left;
	p->image.rect.y = p->top;
    p->next = NULL;

	// add to list
    if (first) {
        p->next = first;
    }
    first = p;
	
}

powerup_t *
RemovePowerup (powerup_t * p)
{
	powerup_t * prev; // pup previous to p
	powerup_t * rtn;
	
	// remove from list
	if (p == first) {
		first = p->next;
	} else {
		prev = first;
		while (prev->next != p) {
			prev = prev->next;
		}
		prev->next = p->next;
	}
	rtn = p->next;
	free(p);
	return rtn;
}

void RemoveAllPowerups ()
{
	powerup_t *p, *temp;
	
	if (!first) return;
	
	p = first;
	do {
		temp = p;
		p = p->next;
		free(temp);
	} while (p);
	first = NULL;
}



//
//	UpdatePowerup
// 	Move all powerups, eat, or remove if off-screen
//
void UpdatePowerups (entity_t * player)
{
	powerup_t * p = first;
	
	while (p)
	{
		p->y += PU_SPEED; // move and update hit box
		p->top += PU_SPEED;
		p->bottom += PU_SPEED;
		p->image.rect.x = p->left; // update image location
		p->image.rect.y = p->top;
		p->shadow.rect.x = p->left + SHAWDOW_OFFSET;
		p->shadow.rect.y = p->top + SHAWDOW_OFFSET;
		
		// paddle collision?
		if (p->bottom > player->top &&
			p->top < player->bottom &&
			p->right > player->left &&
			p->left < player->right)
		{
			p = EatPowerup(p, player);
		} else if (p->top > WINDOW_H) { // off bottom?
			p = RemovePowerup(p);
		} else {
			p = p->next;
		}
	}
}



//
//	EatPowerup
//	Player hit a power up, apply it
//	Returns a pointer to the next
//	power up in the list
//
powerup_t *
EatPowerup ( powerup_t * p, entity_t * player )
{
	entity_t * 	ball;

	player->effects |= p->flag;
	
	switch (p->type)
	{
			// update a global ball speed
		case speedup:
//			ball->dx += 1.0f * sign(ball->dx);
//			ball->dy += 1.0f * sign(ball->dy);
			// TODO limit
			break;
		case slowdown:
//			ball->dx -= 1.0f * sign(ball->dx);
//			ball->dy -= 1.0f * sign(ball->dy);
			// TODO limit
			break;
		case lengthener:
			player->width += 12;
			if (player->width > PLAYER_W * 2)
				player->width = PLAYER_W * 2;
            E_SetPosition(player, player->pos.x, player->pos.y);
			break;
		case shortener:
			player->width -= 12;
			if (player->width < PLAYER_W / 2)
				player->width = PLAYER_W / 2;
            E_SetPosition(player, player->pos.x, player->pos.y);
			break;
		case multiball:
			ball = firstball;
			while (ball) {
				ball->effects |= EF_MULTI;
				ball = ball->next;
			}
			break;
		case stickyball:
			break;
		case oneup:
			SetPlayerLives(player->lives + 1);
			break;
		case threeup:
			SetPlayerLives(player->lives + 3);
			break;
			
		default:
			break;
	}
	
	S_PlaySound(SND_POWERUP);
	return RemovePowerup(p);
}



// update image location and draw it
void DrawPowerups ()
{
	powerup_t * p;
	int 		c;
	SDL_Rect 	r;
	
	p = first;
	while (p)
	{
		if (p->type == superball) {
			c = rand() % NUMCOLORS;
			SetColor(&bright_colors[c]);
			r = (SDL_Rect){ p->x-4, p->y-4, 8, 8 };
			SDL_RenderFillRect(renderer, &r);
		} else {
			T_RenderLabel(&p->image);
			T_RenderLabel(&p->shadow);
		}
		p = p->next;
	}
}
