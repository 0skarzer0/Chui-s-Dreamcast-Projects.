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

#include "fsl.h"

static Uint32 next_time, fps_time;

static inline void init_frame_time(void)
{
	next_time=SDL_GetTicks()+FSL_FPS;
}


static inline void delay_frame_time(void)
{
	Uint32 t=SDL_GetTicks();

	if (t<next_time)
	{
		SDL_Delay(next_time-t);
		next_time+=FSL_FPS;
	}
	else
		next_time=t+FSL_FPS;
}

void fsl_clean_events(void)
{
	SDL_Event event;

	SDL_Delay(150);
	while( SDL_PollEvent( &event ) )
		SDL_Delay(50);
}


p_fsl_game fsl_process(void)
{
    SDL_Event event;
    int quit=0, frame=0;
    p_fsl_game actual=fsl_first;
    extern int fsl_theend;
    int resized=0;

#ifdef DREAMCAST
    SDL_Delay(500);
#endif
    init_frame_time();
    while (!quit)
    {
	int left=0, right=0, fire=0;
	while(SDL_PollEvent( &event ))
	{
		int key=0;
		switch(event.type)
		{
#ifndef DREAMCAST
			case SDL_QUIT:
				quit=fsl_theend=1;
				actual=NULL;
				break;
			case SDL_VIDEORESIZE:
				if (!resized)
				{
					TV_ResizeWindow(event.resize.w, event.resize.h);
					resized=20;
				}
				break;
#endif
			case SDL_KEYDOWN:
				key=1;
			case SDL_KEYUP:
				switch(event.key.keysym.sym)
				{
#ifndef DREAMCAST
					case SDLK_ESCAPE:
						actual=NULL;
						quit=fsl_theend=1; break;
#endif
					case SDLK_LEFT:
						left=key; break;
					case SDLK_RIGHT:
						right=key; break;
					case SDLK_RETURN:
						fire=key; break;
				}
				break;
		}
	}

	if (actual)
	{
		if (left)
		{
			if (actual->back)
			{
				fsl_play_ping();
				actual=(p_fsl_game)actual->back;
				frame=0;
			}
		}
		else if (right)
		{
			if (actual->next)
			{
				fsl_play_ping();
				actual=(p_fsl_game)actual->next;
				frame=0;
			}	
		}
		if ((fire) || (actual->next==NULL && actual->back==NULL))
		{
			fsl_draw(NULL,0);
			SDL_Delay(333);
			if (actual->type==FSL_EXEC_TYPE_BIN)
				fsl_exec_bin(actual->exec);
			else
				quit=1;
		}
	}
	if (resized)
	{
		resized--;
		if (!resized)
		{
			SDL_ShowCursor(SDL_FALSE);
			SDL_WM_SetCaption(fsl_games_name,fsl_games_name);
			SDL_WM_SetIcon(SDL_LoadBMP(DPRE "icon.bmp"),NULL);
		}
	}
	fsl_draw(actual,frame);
	delay_frame_time();
#ifndef GP32
	frame+=2;
#else
	frame+=3;
#endif
    }
    fsl_draw(NULL,0);
    return actual;
}
