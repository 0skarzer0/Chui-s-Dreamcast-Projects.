/*
 * $Id: soundmanager.c,v 1.4 2003/02/09 07:34:16 kenta Exp $
 *
 * Copyright 2002 Kenta Cho. All rights reserved.
 */

/**
 * BGM/SE manager(using SDL_mixer).
 *
 * @version $Revision: 1.4 $
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "SDL.h"
#include <SDL_mixer.h>
#include "soundmanager.h"

static int useAudio = 0;

#define MUSIC_NUM 7

//static char *musicFileName[MUSIC_NUM] = { "stg0.mod", "stg1.mod", "stg2.mod", "stg3.mod", "stg4.mod", "stg5.mod", "stg6.mod" };
//static Mix_Music *music[MUSIC_NUM];

static Mix_Music *music=NULL;
static SDL_CD *cdrom=NULL;

#define CHUNK_NUM 7

static char *chunkFileName[CHUNK_NUM] = {
  "shot.wav", "hit.wav", "foedst.wav", "bossdst.wav", "shipdst.wav", "bonus.wav", "extend.wav", 
};
static Mix_Chunk *chunk[CHUNK_NUM];
static int chunkFlag[CHUNK_NUM];

void closeSound() {
  int i;
  if ( !useAudio ) return;
  if (cdrom)
  {
	  SDL_CDStop(cdrom);
	  SDL_CDClose(cdrom);
	  cdrom=NULL;
  }
  else
  if ( Mix_PlayingMusic() ) {
    Mix_HaltMusic();
  }
/*
  for ( i=0 ; i<MUSIC_NUM ; i++ ) {
    if ( music[i] ) {
      Mix_FreeMusic(music[i]);
    }
  }
*/
  for ( i=0 ; i<CHUNK_NUM ; i++ ) {
    if ( chunk[i] ) {
      Mix_FreeChunk(chunk[i]);
    }
  }
  Mix_CloseAudio();
}


// Initialize the sound.

static void loadSounds() {
  int i;
  char name[64];
/*
  for ( i=0 ; i<MUSIC_NUM ; i++ ) {
    strcpy(name, DATA_PREFIX "sounds" FILE_SEPARATOR);
    strcat(name, musicFileName[i]);
    if ( NULL == (music[i] = Mix_LoadMUS(name)) ) {
      fprintf(stderr, "Couldn't load: %s\n", name);
      useAudio = 0;
      return;
    }
  }
*/
  for ( i=0 ; i<CHUNK_NUM ; i++ ) {
    strcpy(name, DATA_PREFIX "sounds" FILE_SEPARATOR);
    strcat(name, chunkFileName[i]);
    if ( NULL == (chunk[i] = Mix_LoadWAV(name)) ) {
      fprintf(stderr, "Couldn't load: %s\n", name);
      useAudio = 0;
      return;
    }
    chunkFlag[i] = 0;
  }
}

void initSound() {
  int audio_rate;
  Uint16 audio_format;
  int audio_channels;
  int audio_buffers;
  int interactive = 0;

/*
  if ( SDL_InitSubSystem(SDL_INIT_AUDIO) < 0 ) {
    fprintf(stderr, "Unable to initialize SDL_AUDIO: %s\n", SDL_GetError());
    return;
  }
*/

  audio_rate = 44100;
  audio_format = AUDIO_S16;
  audio_channels = 1;
  audio_buffers = 1024;
  
  if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
    fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
    return;
  } else {
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
  }

  useAudio = 1;
  loadSounds();

  cdrom=SDL_CDOpen(0);
  if (cdrom)
  {
	  if (!( CD_INDRIVE(SDL_CDStatus(cdrom)) )) 
	  {
		SDL_CDStop(cdrom);
		cdrom=NULL;
	  }
	  else
		  if (cdrom->numtracks<4)
		  {
			SDL_CDStop(cdrom);
			cdrom=NULL;
		  }
  }
}

// Play/Stop the music/chunk.

void playMusic(int idx) {
  char name[64];
  if ( !useAudio ) return;
  if (idx>CHUNK_NUM)
	idx=CHUNK_NUM;
  if ( !cdrom )
  {
  	Mix_HaltMusic();
 	 if (music)
 	     Mix_FreeMusic(music);
 	 sprintf((char*)name, DATA_PREFIX "musics" FILE_SEPARATOR "stg%d.mod", idx);

 	 music=Mix_LoadMUS(name);
	 Mix_PlayMusic(music, -1);
   }
   else
   {
	idx+=1;
#ifdef DREAMCAST
	idx+=2;
#endif
	if( CD_INDRIVE(SDL_CDStatus(cdrom)) ) {
		SDL_CDPlayTracks(cdrom, idx, 0, 1, 0);
	}
   }
}

void fadeMusic() {
  if ( !useAudio ) return;
  if (!cdrom)
  	Mix_FadeOutMusic(1280);
  else
	  SDL_CDStop(cdrom);
}

void stopMusic() {
  if ( !useAudio ) return;
  if (cdrom)
	  SDL_CDStop(cdrom);
  else
  if ( Mix_PlayingMusic() ) 
    Mix_HaltMusic();
}

void playChunk(int idx) {
  if ( !useAudio ) return;
  Mix_PlayChannel(idx, chunk[idx], 0);
}
