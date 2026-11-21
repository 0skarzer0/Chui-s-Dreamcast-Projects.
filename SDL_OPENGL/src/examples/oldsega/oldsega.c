#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

/* Old sega logo and sound files */
#include "data.h"

SDL_Surface *screen, *image;
Mix_Chunk *sound;


void waitEvent(void)
{
	SDL_Event event;
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
	while(!SDL_PollEvent(&event))
		SDL_Delay(100);
}

int main(int argn, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK))
		return -1;
	else
	{
		screen=SDL_SetVideoMode(320,240,16,SDL_HWSURFACE|SDL_DOUBLEBUF);
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_JoystickOpen(0);
		if (screen==NULL)
			return -2;
		else
		{
			SDL_Surface *tmp=IMG_Load_RW(SDL_RWFromMem(&data_image,IMAGE_LEN),0);
			image=SDL_DisplayFormat(tmp);
			SDL_SetColorKey(image,SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(screen->format, 0x80, 0x80, 0x80));
			SDL_FreeSurface(tmp);
			
			if (Mix_OpenAudio(44100, AUDIO_S16, 2, 512))
				return -3;
			else
			{
				SDL_Rect r;

				sound=Mix_LoadWAV_RW(SDL_RWFromMem(&data_sound,SOUND_LEN),0);
				Mix_PlayChannel(-1,sound,0);

				r.w=image->w;
				r.h=image->h;
				r.x=(320-r.w)/2;
				r.y=(240-r.h)/2;
				SDL_FillRect(screen,NULL,0);
				SDL_Flip(screen);
				SDL_Delay(600);
				SDL_BlitSurface(image,NULL,screen,&r);
				SDL_Flip(screen);

				waitEvent();
			}
		}
	}

	SDL_Quit();
	return 0;
}
