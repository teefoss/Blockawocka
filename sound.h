//
//  sound.h
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//
//	Handles sound system, loading and playing game sounds
//

#ifndef sound_h
#define sound_h

#include <SDL2_mixer/SDL_mixer.h>

typedef enum
{
	SND_SIDEHIT,
	SND_PADHIT,
	SND_GOAL,
	SND_BRICK,
	SND_POWERUP,
	
	NUM_SOUNDS
} sound_t;

void S_InitSound (void);
void S_ShutdownSound (void);
//void S_LoadSound (Mix_Chunk ** chunk, const char * wavfile);
void S_PlaySound (sound_t s);

#endif /* sound_h */
