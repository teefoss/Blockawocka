//
//  entity.c
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#include <SDL2/SDL.h>
#include "entity.h"
#include "breakout.h"

//
// E_SetPosition
// Move entity to x, y and update its hitbox and drawrect
//
void E_SetPosition (entity_t * e, float x, float y)
{
    e->pos.x = x;
    e->pos.y = y;
	
    e->top 		= e->pos.y - (float)(e->height / 2);
    e->bottom 	= e->pos.y + (float)(e->height / 2);
    e->left 	= e->pos.x - (float)(e->width / 2);
    e->right 	= e->pos.x + (float)(e->width / 2);
	
	e->rect = (SDL_Rect){ e->left, e->top, e->width, e->height };
}




static void E_UpdatePosition (entity_t *e)
{
    E_SetPosition(e, e->pos.x, e->pos.y);
}




void E_Accellerate (entity_t *e)
{
    AddVector(&e->pos, &e->vel);
    E_UpdatePosition(e);
}




//
// 	E_Collision
//	Returns true if entities collide
//
bool E_Collision (entity_t * e1, entity_t * e2)
{
	return
    e1->top < e2->bottom &&
    e1->bottom > e2->top &&
    e1->left < e2->right &&
    e1->right > e2->left;
}


//
// Entity Linked List
//
// 'first' is a * to start of list to be added to
//

// add to list, no initialization
entity_t *
E_AddEntity (entity_t ** first)
{
	entity_t * new;

	new = calloc(1, sizeof(entity_t));
	new->active = true;
	
	// add to list
	if (*first == NULL) {
		*first = new;
		new->next = NULL;
	} else {
		new->next = *first;
		*first = new;
	}
	return new;
}

void
E_RemoveEntity (entity_t * rem, entity_t ** first)
{
	entity_t * prev;
	
	// the first and only
	if (rem == *first && !rem->next) {
		*first = NULL;
	}
	// the first of multiple
	else if (rem == *first && rem->next) {
		*first = rem->next;
	}
	// it's somewhere down the list
	else {
		prev = *first; // find the one prior
		while (prev->next != rem) {
			prev = prev->next;
		}
		if (!rem->next) { // at the end
			prev->next = NULL;
		} else { // somewhere in the middle
			prev->next = rem->next;
		}
	}
	free(rem);
	rem = NULL;
}

// clear the entire list
void
E_RemoveAll (entity_t ** first)
{
	entity_t * 	e;
	entity_t *	temp;
	
	if (*first == NULL) return;
	
	e = *first;
	while (e) {
		temp = e;
		e = e->next;
		free(temp);
	};
	*first = NULL;
}




void E_DrawEntity (entity_t * e, color_t c)
{
    DrawShadow(e->rect);
    
    SetBrightColor(c);
    SDL_RenderFillRect(renderer, &e->rect);
    SetDarkColor(c);
    SDL_RenderDrawRect(renderer, &e->rect);
}
