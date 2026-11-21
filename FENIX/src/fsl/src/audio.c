/*
 * Fenix SDL Loader
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
----------------------------------
- Musicas y efectos sonoros -
----------------------------------
*/

/* Definiciones generales */
#include "fsl.h"

#define RATE 44100
#define AUDIOBUF 512

static Mix_Music *music_menu=NULL;
static Mix_Chunk *sound_ping=NULL;

int fsl_init_audio(void)
{
#ifndef NOSOUND

	if (Mix_OpenAudio(RATE, AUDIO_S16, 2, AUDIOBUF))
		return 0;
	music_menu=Mix_LoadMUS(DPRE "music.mod");
	if (music_menu!=NULL)
		Mix_PlayMusic(music_menu,-1);
	sound_ping=Mix_LoadWAV(DPRE "ping.wav");
#endif

	return -1;
}

void fsl_play_ping(void)
{
	if (sound_ping)
		Mix_PlayChannel(-1, sound_ping,0);
}

void fsl_quit_audio(void)
{
	if (music_menu)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music_menu);
	}
	if (sound_ping)
	{
		Mix_FreeChunk(sound_ping);	
	}
}

