//
//  vector.c
//  Breakout
//
//  Created by Thomas Foster on 6/15/19.
//

#include "vector.h"
#include <math.h>

vector_t Vector (float x, float y)
{
	vector_t v = { x, y };
	return v;
}

// Sum in v1
void AddVector (vector_t *v1, vector_t *v2)
{
	v1->x += v2->x;
	v1->y += v2->y;
}

vector_t SubVector (const vector_t *v1, const vector_t *v2)
{
	vector_t new;
	
	new.x = v1->x - v2->x;
	new.y = v1->y - v2->y;
	
	return new;
}

// returns product in v
void MultVector (vector_t *v, float val)
{
	v->x *= val;
	v->y *= val;
}

// returns quotient in v
void DivVector (vector_t *v, float val)
{
	v->x /= val;
	v->y /= val;
}

float VectorMagSqr (vector_t *v)
{
	return (v->x * v->x) + (v->y * v->y);
}

float VectorMag (vector_t *v)
{
	return sqrtf(v->x*v->x + v->y*v->y);
}

void NormalizeVector (vector_t *v)
{
	float mag = VectorMag(v);
	DivVector(v, mag);
}

void RotateVector (vector_t *v, float degrees)
{
	float angle = degrees*M_PI/180;
	
	v->x = cos(angle) * v->x - sin(angle) * v->y;
	v->y = sin(angle) * v->x + cos(angle) * v->y;
}
