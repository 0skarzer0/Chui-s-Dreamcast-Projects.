/*
 *  Fenix - Videogame compiler/interpreter
 *  Current release       : FENIX - PROJECT 1.0 - R 0.84
 *  Last stable release   : 
 *  Project documentation : http://fenix.divsite.net
 *
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Copyright © 1999 José Luis Cebrián Pagüe
 *  Copyright © 2002 Fenix Team
 *
 */

/*
 * FILE        : mixer.c
 * DESCRIPTION : Sound Mixer DLL
 */



#include "dumb.h"
#include "dumbogg.h"
#include "wav.h"

#include <fxdll.h>


SDL_AudioSpec spec;

typedef struct _MMusic {
	struct _MMusic * next;
	DUH *duh;
	int num;

} MMusic;


typedef struct _MPlayer {
	
	DUH_SIGRENDERER *sr;	
	struct _MPlayer * next;
	float delta;
	float volume;
	int depth;
	int n_channels;
	int num;
	int music_num;
	int paused;

} MPlayer;



union {
		short s16[8192];
		char s8[16384];
} buffer;	

int bufsize;
	
MMusic *music;
MPlayer *player;	

MPlayer *player_song;	


	
static DUMBFILE_SYSTEM dfs;

void fillerup(void *unused, Uint8 *stream, int len)
{

	MPlayer *player_audio;
	MPlayer *previous=NULL;
	int l=0;
		

		player_audio=player;

		while (player_audio->next!=NULL) {   
			if (player_audio->paused==0) {
				l = duh_render(player_audio->sr, player_audio->depth, 0, player_audio->volume, player_audio->delta, bufsize, &buffer);
				SDL_MixAudio(stream, &buffer, l * spec.channels * (player_audio->depth >> 3) , SDL_MIX_MAXVOLUME);
			
				if (l<bufsize) {
					if (previous==NULL) {
						duh_end_sigrenderer(player->sr);										
						player_audio=player_audio->next;	
						free (player);
						player=player_audio;
																					
					} else {
						previous->next=player_audio->next;
						duh_end_sigrenderer(player_audio->sr);
						free (player_audio);
						player_audio=previous;
					
					}

				} else {
					previous=player_audio;
					player_audio=player_audio->next;
				}
			} else {
				previous=player_audio;
				player_audio=player_audio->next;
			}
			

		}
	

		if ((player_song->sr!=NULL) && (player_song->paused==0)) {
		
			l = duh_render(player_song->sr, player_song->depth, 0, player_song->volume, player_song->delta, bufsize, &buffer);
			SDL_MixAudio(stream, &buffer, l * spec.channels * (player_song->depth >> 3) , SDL_MIX_MAXVOLUME);
			if (l<bufsize) {
				duh_end_sigrenderer(player_song->sr);
				player_song->sr=NULL;

			}
		}
		

}




void *file_open_mixer (const char *filename) {
	 return (file_open (filename, "rb"));
}



int  file_skip_mixer (void *f, long n) {
	  
	 file_seek (f, n, SEEK_CUR);
	
	 return 0;
	
}


int file_getc_mixer (void *f) {
	
	unsigned char ptr;
		
	file_read (f, &ptr,1);	
	return ptr;
}



int file_getnc_mixer (char *ptr, long n, void *f) {
	

	return file_read (f, ptr,  n);

}


void file_close_mixer (void *f) {
		
	file_close (f);
}






static int fxi_mixer_init (INSTANCE * my, int * params) {
	
	
	music=malloc(sizeof(MMusic));	
	music->next=NULL;
	music->num=1;

	player=malloc(sizeof(MPlayer));
	player->next=NULL;
	player->sr=NULL;
	player->num=1;

	player_song=malloc(sizeof(MPlayer));
	player_song->sr=NULL;

	sound_close();


	

	if (GLODWORD(SOUND_FREQ)> 22050)
		spec.freq = 44100;
	else if (GLODWORD(SOUND_FREQ) > 11025)
		spec.freq = 22050;
	else
		spec.freq = 11025;
	
	if (GLODWORD(SOUND_MODE)==1)
		spec.channels = 2; 
    else
		spec.channels = 1; 
	
	spec.format = AUDIO_S16;	
	spec.samples = 4096; //4096; 
	spec.callback = fillerup;

	atexit(&dumb_exit);
	

	dfs.open=&file_open_mixer;
	dfs.skip=&file_skip_mixer;	
	dfs.getc=&file_getc_mixer;
	dfs.getnc=&file_getnc_mixer;
	dfs.close=&file_close_mixer;

	register_dumbfile_system(&dfs);


	dumb_resampling_quality=1;
	dumb_it_max_to_mix = 256;


	SDL_OpenAudio(&spec, NULL);

	bufsize =  8192/2;
	  
	
	SDL_PauseAudio(0);
	
		
	

	return 0;
}
	

static int fxi_mixer_load (INSTANCE * my, int * params) {

		MMusic *music_load;
		const char *fn = NULL;
		int i;

		music_load=music;
		while (music_load->next!=NULL) {
			music_load=music_load->next;
		}


		fn=string_get(params[0]);			
		string_discard(params[0]);
 
	    music_load->duh = dumb_load_wav(fn);
		if (!music_load->duh) {
			
			music_load->duh = dumb_load_xm(fn);
			if (!music_load->duh) {
				music_load->duh = dumb_load_it(fn);
				
				if (!music_load->duh) {
					
					music_load->duh = dumb_load_mod(fn);
					if (!music_load->duh) {
						
						music_load->duh = dumb_load_ogg(fn,0);
						if (!music_load->duh) {
							
							music_load->duh= dumb_load_s3m(fn);
							
							if (!music_load->duh) {
									gr_con_printf ("Unable to open the file\n") ;
									return -1;
							}
						}
					}
				}
			}
		}

		i=music_load->num;

		music_load->next=malloc(sizeof(MMusic));
		music_load->next->next=NULL;
		music_load->next->num=i+1;
		
		
		
			return i;
   
}


static int fxi_mixer_play (INSTANCE * my, int * params) {

		MPlayer *music_player;
		MMusic *music_load;

		int i;
		int j;

		i=params[0];

		music_load=music;
		
		while ((music_load->next!=NULL) && (music_load->num!=i)) {
			music_load=music_load->next;
		}

		if (music_load->num!=i) return -1;


		music_player=player;
		
		while (music_player->next!=NULL) {
			music_player=music_player->next;
		}

		j=music_player->num;
		music_player->music_num=i;
		music_player->paused=0;
		music_player->n_channels=spec.channels;
		music_player->depth=16;
		music_player->volume=1.0f;
		music_player->delta=65536.0f /spec.freq;
		music_player->sr=duh_start_sigrenderer(music_load->duh, 0, music_player->n_channels, 0);	
		
		{
			DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(music_player->sr);
			dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
			dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
		}


		music_player->next=malloc(sizeof(MPlayer));
		music_player->next->next=NULL;
		music_player->next->num=j+1;

		
		gr_con_printf ("Numero de player %d \n",j);

   return j;   
}


static int fxi_mixer_play_volume (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;
	float _volume;

	i=params[0];

	music_player=player;



	while ((music_player->next!=NULL) && (music_player->num!=i)) {
			music_player=music_player->next;
		}

		if (music_player->num!=i) return -1;
		_volume=*(float *)&params[1];;
		if (_volume<0.0f) _volume=0.0f;
		music_player->volume=_volume;
	return 0;

}


static int fxi_mixer_play_freq (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;
	float quant;

	i=params[0];

	music_player=player;

	while ((music_player->next!=NULL) && (music_player->num!=i)) {
			music_player=music_player->next;
		}

		if (music_player->num!=i) return -1;
	
		quant=(*(float *)&params[1]);
		if (quant<256) quant=256;
		if (quant>131072) quant=131072;
		music_player->delta=65536.0f/quant;
	return 0;

}


static int fxi_mixer_pause (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;


	i=params[0];

	music_player=player;

	while ((music_player->next!=NULL) && (music_player->num!=i)) {
			music_player=music_player->next;
		}

	if (music_player->num!=i) return -1;
	
	
	music_player->paused=1;

	return 0;

}


static int fxi_mixer_resume (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;


	i=params[0];

	music_player=player;

	while ((music_player->next!=NULL) && (music_player->num!=i)) {
			music_player=music_player->next;
		}

	if (music_player->num!=i) return -1;

	music_player->paused=0;


	return 0;

}


static int fxi_mixer_stop (INSTANCE * my, int * params) {
	MPlayer *player_audio;
	MPlayer *previous=NULL;
	int i;

	i=params[0];

	player_audio=player;

	while ((player_audio->num!=i) && (player_audio->next!=NULL)) {
		previous=player_audio;
		player_audio=player_audio->next;

	}

	if (player_audio->num!=i) return -1;

	
	if (previous==NULL) {
		duh_end_sigrenderer(player->sr);										
		player_audio=player_audio->next;	
		free (player);
		player=player_audio;
																					
	} else {
		previous->next=player_audio->next;
		duh_end_sigrenderer(player_audio->sr);
		free (player_audio);				
	}

	return 0;

}




static int fxi_mixer_rewind (INSTANCE * my, int * params) {

		MPlayer *player_audio;
		MMusic *music_load;

		int i;
		int j;

		i=params[0];

		player_audio=player;

		while ((player_audio->num!=i) && (player_audio->next!=NULL)) {		
			player_audio=player_audio->next;

		}

		if (player_audio->num!=i) return -1;

		j=player_audio->music_num;


		music_load=music;
		
		while ((music_load->next!=NULL) && (music_load->num!=j)) {
			music_load=music_load->next;
		}

		if (music_load->num!=j) return -1;

	
		duh_end_sigrenderer(player_audio->sr);

		player_audio->n_channels=spec.channels;						
		player_audio->sr=duh_start_sigrenderer(music_load->duh, 0, player_audio->n_channels, 0);	
		
		{
			DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(player_audio->sr);
			dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
			dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
		}

   return 0;   
}


static int fxi_mixer_is_playing (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;


	i=params[0];

	music_player=player;

	while ((music_player->next!=NULL) && (music_player->num!=i)) {
			music_player=music_player->next;
		}

	if (music_player->num!=i) return 0; else return 1;
	

}


static int fxi_mixer_unload (INSTANCE * my, int * params) {
	MPlayer *player_audio;
	MMusic  *music_load;
	MPlayer *previous=NULL;
	MMusic  *previous_music=NULL;

	int i;

		i=params[0];
	
		player_audio=player;

		while (player_audio->next!=NULL) {        		
			if (player_audio->music_num==i) {
			
				if (previous==NULL) {
						duh_end_sigrenderer(player->sr);										
						player_audio=player_audio->next;	
						free (player);
						player=player_audio;
																					
				} else {
					previous->next=player_audio->next;
					duh_end_sigrenderer(player_audio->sr);
					free (player_audio);
					player_audio=previous;
					
				}

			} else {
				previous=player_audio;
				player_audio=player_audio->next;
			}
					
		}
	

		if (player_song->music_num==i) {
			duh_end_sigrenderer(player_song->sr);
			player_song->sr=NULL;

		}

	    music_load=music;
		
		while ((music_load->next!=NULL) && (music_load->num!=i)) {
			previous_music=music_load;
			music_load=music_load->next;
		}

		if (music_load->num!=i) return -1;

		if (previous_music==NULL) {
			previous_music=music_load;
			music=music_load->next;
			unload_duh(previous_music->duh);
			free(previous_music);

		} else {
			previous_music->next=music_load->next;
			unload_duh(music_load->duh);
			free (music_load);
		}
		
		return 0;

}



//SONGS

static int fxi_mixer_play_song (INSTANCE * my, int * params) {

		MPlayer *music_player;
		MMusic *music_load;

		int i;
	

		i=params[0];

		music_load=music;
		
		while ((music_load->next!=NULL) && (music_load->num!=i)) {
			music_load=music_load->next;
		}

		if (music_load->num!=i) return -1;


		music_player=player_song;

		if (music_player->sr!=NULL) {
			duh_end_sigrenderer(music_player->sr);
		}


		music_player->music_num=i;
		music_player->paused=0;
		music_player->n_channels=spec.channels;
		music_player->depth=16;
		music_player->volume=1.0f;
		music_player->delta=65536.0f /spec.freq;
		music_player->sr=duh_start_sigrenderer(music_load->duh, 0, music_player->n_channels, 0);	
		
		{
			DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(music_player->sr);
			dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
			dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
		}



   return 0;
}


static int fxi_mixer_is_playing_song (INSTANCE * my, int * params) {
	if (player_song->sr==NULL) 
		return 0;
	else
		return 1;

}



static int fxi_mixer_stop_song (INSTANCE * my, int * params) {
	if (player_song->sr==NULL) 
		return -1;

	duh_end_sigrenderer(player_song->sr);
	player_song->sr=NULL;	
	return 1;

}

static int fxi_mixer_pause_song (INSTANCE * my, int * params) {
	if (player_song->sr==NULL) 
		return -1;

	player_song->paused=1;	
	return 1;

}

static int fxi_mixer_resume_song (INSTANCE * my, int * params) {
	if (player_song->sr==NULL) 
		return -1;

	player_song->paused=0;	
	return 1;

}






static int fxi_mixer_song_volume (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;
	float _volume;

	i=params[0];

	music_player=player_song;
	
		if (music_player->sr==NULL) return -1;
		_volume=*(float *)&params[0];;
		if (_volume<0.0f) _volume=0.0f;
		music_player->volume=_volume;
	return 0;

}


static int fxi_mixer_song_freq (INSTANCE * my, int * params) {
	MPlayer *music_player;

	int i;
	float quant;

	i=params[0];

	music_player=player_song;

		if (music_player->sr==NULL) return -1;
	
		quant=(*(float *)&params[0]);
		if (quant<256) quant=256;
		if (quant>131072) quant=131072;
		music_player->delta=65536.0f/quant;
	return 0;

}




static int fxi_mixer_rewind_song (INSTANCE * my, int * params) {

		MPlayer *player_audio;
		MMusic *music_load;

		int i;
		int j;

		i=params[0];

		player_audio=player_song;


		if (player_audio->sr==NULL) return -1;

		j=player_audio->music_num;


		music_load=music;
		
		while ((music_load->next!=NULL) && (music_load->num!=j)) {
			music_load=music_load->next;
		}

		if (music_load->num!=j) return -1;

	
		duh_end_sigrenderer(player_audio->sr);

		player_audio->n_channels=spec.channels;						
		player_audio->sr=duh_start_sigrenderer(music_load->duh, 0, player_audio->n_channels, 0);	
		
		{
			DUMB_IT_SIGRENDERER *itsr = duh_get_it_sigrenderer(player_audio->sr);
			dumb_it_set_loop_callback(itsr, &dumb_it_callback_terminate, NULL);
			dumb_it_set_xm_speed_zero_callback(itsr, &dumb_it_callback_terminate, NULL);
		}

   return 0;   
}






int get_mixer_channels (void) {

	return spec.channels;
}


FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("MIXER_INIT",					"",			TYPE_DWORD, fxi_mixer_init );
	FENIX_export ("MIXER_LOAD_WAV",				"S",		TYPE_DWORD, fxi_mixer_load );
	FENIX_export ("MIXER_UNLOAD_WAV",			"I",		TYPE_DWORD, fxi_mixer_unload );
	FENIX_export ("MIXER_PLAY_WAV",				"I",		TYPE_DWORD, fxi_mixer_play );
	FENIX_export ("MIXER_SET_WAV_VOLUME",		"IF",		TYPE_DWORD, fxi_mixer_play_volume );
	FENIX_export ("MIXER_SET_WAV_FREQ",			"IF",		TYPE_DWORD, fxi_mixer_play_freq);
	FENIX_export ("MIXER_PAUSE_WAV",			"I",		TYPE_DWORD, fxi_mixer_pause);
	FENIX_export ("MIXER_RESUME_WAV",			"I",		TYPE_DWORD, fxi_mixer_resume);
	FENIX_export ("MIXER_STOP_WAV",				"I",		TYPE_DWORD, fxi_mixer_stop);
	FENIX_export ("MIXER_REWIND_WAV",			"I",		TYPE_DWORD, fxi_mixer_rewind);
	FENIX_export ("MIXER_IS_PLAYING_WAV",		"I",		TYPE_DWORD, fxi_mixer_is_playing);

	FENIX_export ("MIXER_LOAD_SONG",			"S",		TYPE_DWORD, fxi_mixer_load );
	FENIX_export ("MIXER_UNLOAD_SONG",			"I",		TYPE_DWORD, fxi_mixer_unload );
	FENIX_export ("MIXER_IS_PLAYING_SONG",		"",			TYPE_DWORD, fxi_mixer_is_playing_song);
	FENIX_export ("MIXER_STOP_SONG",			"",			TYPE_DWORD, fxi_mixer_stop_song);
	FENIX_export ("MIXER_PAUSE_SONG",			"",			TYPE_DWORD, fxi_mixer_pause_song);
	FENIX_export ("MIXER_RESUME_SONG",			"",			TYPE_DWORD, fxi_mixer_resume_song);
	FENIX_export ("MIXER_SET_SONG_VOLUME",		"F",		TYPE_DWORD, fxi_mixer_song_volume );
	FENIX_export ("MIXER_SET_SONG_FREQ",		"F",		TYPE_DWORD, fxi_mixer_song_freq);
	FENIX_export ("MIXER_REWIND_SONG",			"",			TYPE_DWORD, fxi_mixer_rewind_song);
	FENIX_export ("MIXER_PLAY_SONG",			"I",		TYPE_DWORD, fxi_mixer_play_song );

}

