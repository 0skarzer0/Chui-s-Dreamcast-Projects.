#pragma comment (lib, "smpeg")
#pragma comment (lib, "SDL_mixer")
#pragma comment (lib, "SDL")


#include <fxdll.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include "smpeg.h"
#include "SDL_mixer.h"


typedef struct
{
	SMPEG *mpeg;
	SMPEG_Info info;
	SDL_Surface *temp_surface;  
	GRAPH *graph_mpeg;
	int audio;
	int video;
	
} mpeg_video;

int audio_hooked=0;


/*

/*
 *  FUNCTION : MPEG_CURRENT_TIME
 *
 *  return the mpeg current time in centiseconds
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		int current time
 *  
 */

static int mpeg_current_time (INSTANCE * my, int * params)
{

	
	int current_time;
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];

	SMPEG_getinfo(mpeg->mpeg, &mpeg->info );

	current_time=(int )(mpeg->info.current_time*100);

	return current_time;
}


/*
 *  FUNCTION : MPEG_TOTAL_TIME
 *
 *  return the mpeg total time in centiseconds
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		int total_time
 *  
 */

static int mpeg_total_time (INSTANCE * my, int * params)
{

	
	int total_time;
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];
	
	total_time=(int )(mpeg->info.total_time*100);

	return total_time;
}



/*
 *  FUNCTION : MPEG_FILTER
 *
 *  sets de volume from a mpeg video
 *
 *  PARAMS:
 *      mpeg id
 *		int filter  0=null    1=bilinear   2=deblocking
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */

static int mpeg_filter (INSTANCE * my, int * params)
{

	SMPEG_Filter *filter;

	mpeg_video *mpeg;
	mpeg=(mpeg_video *)params[0];
	if (params[1]==2) {
		filter = SMPEGfilter_deblocking();
	} else {
		if (params[1]==1) {
			filter = SMPEGfilter_bilinear();
		} else {
			filter = SMPEGfilter_null();
		}
	}    

    filter = SMPEG_filter( mpeg->mpeg, filter );
    filter->destroy(filter);

	return 1;
}
                        
/*
 *  FUNCTION : LOOP_MPEG
 *
 *  sets de volume from a mpeg video
 *
 *  PARAMS:
 *      mpeg id
 *		int loop        0=loop false    1= loop true
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */

static int loop_mpeg (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;
	mpeg=(mpeg_video *)params[0];
	if (params[1]==0) {
		SMPEG_loop(mpeg->mpeg, 0 );
	} else {
		SMPEG_loop( mpeg->mpeg, 1 );
	}
	return 1;
}




/*
 *  FUNCTION : SET_MPEG_VOLUME
 *
 *  sets de volume from a mpeg video
 *
 *  PARAMS:
 *      mpeg id
 *		int volume (0-100)
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */

static int set_mpeg_volume (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];
	SMPEG_setvolume(mpeg->mpeg, params[1]);
	return 1;
}


/*
 *  FUNCTION : IS_PLAYING_MPEG
 *
 *  check if the desired mpeg is playing
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		true or false
 *  
 */

static int is_playing_mpeg (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;
	mpeg=(mpeg_video *)params[0];
	return (SMPEG_status(mpeg->mpeg)==SMPEG_PLAYING);
}


/*
 *  FUNCTION : REWIND_MPEG
 *
 *  Rewinds and stops the mpeg playing
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */


static int rewind_mpeg (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];
	SMPEG_rewind (mpeg->mpeg);
	if (mpeg->video==1) gr_clear (mpeg->graph_mpeg);
	return 1;
}


/*
 *  FUNCTION : PAUSE_MPEG
 *
 *  Pauses the mpeg playing. The image continues active.
 *  If you call again this function the playing continues
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */



static int pause_mpeg (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;
	mpeg=(mpeg_video *)params[0];

	SMPEG_pause (mpeg->mpeg);
	return 1;
}


/*
 *  FUNCTION : SET_MPEG_VOLUME
 *
 *  Rewinds and stops the a mpeg play
 *
 *  PARAMS:
 *      mpeg id
 *		
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */


static int stop_mpeg (INSTANCE * my, int * params)
{
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];
	SMPEG_stop (mpeg->mpeg);
	if (mpeg->video==1) gr_clear (mpeg->graph_mpeg);
	return 1;
}


/*
 *  FUNCTION : PLAY_MPEG
 *
 *  Start the mpeg playing. It returns the map code where the 
 *	playing is showed.
 *
 *  PARAMS:
 *      mpeg id;
 *
 *  RETURN VALUE: 
 *      
 *		graph code;
 *  
 */


static int play_mpeg (INSTANCE * my, int * params) 
{
	mpeg_video *mpeg;

	mpeg=(mpeg_video *)params[0];
	SMPEG_play(mpeg->mpeg);
	mpeg->graph_mpeg->modified = -1;
	return mpeg->graph_mpeg ? mpeg->graph_mpeg->code : 0 ;
	
}



/*
 *  FUNCTION : LOAD_MPEG
 *
 *  Load a mpeg file and sets its parameters.
 *
 *  PARAMS:
 *      string file;
 *		video_active (0-1)
 *		audio_active (0-1)
 *
 *  RETURN VALUE: 
 *      
 *		mpeg id
 *  
 */


static int load_mpeg (INSTANCE * my, int * params)
{
	SDL_AudioSpec audiofmt;
	Uint16 format;
	int freq, channels;	
	mpeg_video *mpeg;
	
	if (!scr_initialized) gr_init (320, 200) ;

	mpeg = malloc(sizeof(mpeg_video));
	
	mpeg->mpeg = SMPEG_new(string_get(params[0]), &mpeg->info, 0);
	string_discard(params[0]) ;


	if ( SMPEG_error(mpeg->mpeg) ) 
	{			
		SMPEG_delete(mpeg->mpeg);
		free(mpeg);
		return -1;
    }
	
	SMPEG_enableaudio(mpeg->mpeg, 0);
	
	if ((mpeg->info.has_audio) && (params[2]!=0) && (audio_hooked==0)) 
	{		
		audio_hooked=1;
		Mix_QuerySpec(&freq, &format, &channels);
		audiofmt.format = format;
		audiofmt.freq = freq;
		audiofmt.channels = channels;
		SMPEG_actualSpec(mpeg->mpeg, &audiofmt);
		
		Mix_HookMusic(SMPEG_playAudioSDL, mpeg->mpeg);
		SMPEG_enableaudio(mpeg->mpeg, 1);
		mpeg->audio=1;
	} 
	else 
	{
		mpeg->audio=0;
	}
	
	if ((mpeg->info.has_video) && (params[1]!=0))	
	{	
		mpeg->graph_mpeg=bitmap_new_syslib (mpeg->info.width, mpeg->info.height, 16);
		gr_clear (mpeg->graph_mpeg);
		
		mpeg->temp_surface = SDL_CreateRGBSurfaceFrom(
			mpeg->graph_mpeg->data,
			mpeg->info.width, 
			mpeg->info.height, 
			16, 
			mpeg->graph_mpeg->pitch, 
			screen->format->Rmask, 
			screen->format->Gmask,
			screen->format->Bmask,
			screen->format->Amask);
		
		SMPEG_enablevideo(mpeg->mpeg, 1);
		SMPEG_setdisplay(mpeg->mpeg, mpeg->temp_surface, NULL, NULL);
		mpeg->video=1;		    
	} 
	else 
	{
		SMPEG_enablevideo(mpeg->mpeg, 0);
		mpeg->video=0;
	}
	
    return ((int)mpeg);
	
}



/*
 *  FUNCTION : UNLOAD_MPEG
 *
 *  Stop if playing and free all resources needed to play the mpeg.
 *
 *  PARAMS:
 *      mpeg id;
 *
 *  RETURN VALUE: 
 *      
 *		1
 *  
 */


static int unload_mpeg (INSTANCE * my, int * params) 
{
	mpeg_video *mpeg;
	mpeg=(mpeg_video *)params[0];
	
	SMPEG_stop(mpeg->mpeg);
	
	if (mpeg->video==1) 
	{
		SDL_FreeSurface (mpeg->temp_surface);
		mpeg->video=0;
		grlib_unload_map (syslib,mpeg->graph_mpeg->code);
	}		

	if (mpeg->audio==1) 
	{
		Mix_HookMusic(NULL, NULL);			
		audio_hooked=0;
	}
	SMPEG_delete(mpeg->mpeg);
	free(mpeg);
	return 1;
}



// Faltaria una funcion para liberar (automatica desde fenix)

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport
		
	FENIX_export ("LOAD_MPEG", "SII", TYPE_DWORD, load_mpeg );
	FENIX_export ("PLAY_MPEG", "I",  TYPE_DWORD, play_mpeg );
	FENIX_export ("PAUSE_MPEG", "I",  TYPE_DWORD, pause_mpeg );
	FENIX_export ("STOP_MPEG", "I",  TYPE_DWORD, stop_mpeg );
	FENIX_export ("REWIND_MPEG", "I",  TYPE_DWORD, rewind_mpeg );
	FENIX_export ("SET_MPEG_VOLUME", "II",  TYPE_DWORD, set_mpeg_volume );
	FENIX_export ("IS_PLAYING_MPEG", "I",  TYPE_DWORD, is_playing_mpeg );
	FENIX_export ("UNLOAD_MPEG", "I",  TYPE_DWORD, unload_mpeg );
	FENIX_export ("LOOP_MPEG", "II",  TYPE_DWORD, loop_mpeg );
	FENIX_export ("MPEG_FILTER", "II",  TYPE_DWORD, mpeg_filter );
	FENIX_export ("MPEG_TOTAL_TIME", "I",  TYPE_DWORD, mpeg_total_time);
	FENIX_export ("MPEG_CURRENT_TIME", "I",  TYPE_DWORD, mpeg_current_time);

}
