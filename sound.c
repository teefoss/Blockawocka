//
//  sound.c
//  Breakout
//
//  Created by Thomas Foster on 6/8/19.
//

#include <stdio.h>
#include "sound.h"

#define VOLUME 8

static Mix_Chunk *	sounds[NUM_SOUNDS];

static void Error (const char * message)
{
	if (message && *message) {
		puts(message);
	}
	exit(1);
}

static void S_LoadSound (Mix_Chunk ** chunk, const char * wavfile)
{
	*chunk = Mix_LoadWAV(wavfile);
	if (!*chunk) {
		printf("S_LoadSound: Error, could not load %s!\n",wavfile);
		Error(NULL);
	}
	Mix_VolumeChunk(*chunk, VOLUME);
}

void S_InitSound ()
{
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512) < 0) {
		Error("S_InitSound: Error. Could not init audio!");
	}
	S_LoadSound( &sounds[ SND_SIDEHIT ], "assets/sidehit.wav");
	S_LoadSound( &sounds[ SND_GOAL ],    "assets/goal.wav");
	S_LoadSound( &sounds[ SND_PADHIT ],  "assets/padhit.wav");
	S_LoadSound( &sounds[ SND_BRICK ],   "assets/brick.wav");
	S_LoadSound( &sounds[ SND_POWERUP ], "assets/power.wav");
}

void S_ShutdownSound ()
{
	for (int i = 0; i < NUM_SOUNDS; i++) {
		Mix_FreeChunk(sounds[i]);
	}
	Mix_CloseAudio();
}

void S_PlaySound (sound_t s)
{
	Mix_PlayChannel(-1, sounds[s], 0);
}
