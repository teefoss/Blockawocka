//
//  vector.h
//  Breakout
//
//  Created by Thomas Foster on 6/15/19.
//

#ifndef vector_h
#define vector_h

#include <stdbool.h>

typedef struct
{
	float x, y;
} vector_t;

vector_t 	Vector (float x, float y);

void 		AddVector (vector_t *v1, vector_t *v2);
vector_t 	SubVector (const vector_t *v1, const vector_t *v2);
void 		MultVector (vector_t *v, float val);
void 		DivVector (vector_t *v, float val);

void 		RotateVector (vector_t *v, float degrees);
float 		VectorMagSqr (vector_t *v);
float 		VectorMag (vector_t *v);
void 		NormalizeVector (vector_t *v);

#endif /* vector_h */
