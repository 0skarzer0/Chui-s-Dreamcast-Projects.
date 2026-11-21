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

#ifdef DREAMCAST
//extern uint8 romdisk[];
KOS_INIT_FLAGS(INIT_DEFAULT);
//KOS_INIT_ROMDISK(romdisk);
#endif

p_fsl_game fsl_first=NULL;

static char *parse_error="PARSE ERROR !";

static void fsl_quit(void)
{
#ifdef DREAMCAST
	arch_reboot();
	void (*eje)()__noreturn;
	eje=(void *)0x8c004000;
	irq_disable();
	arch_shutdown();
	eje();
#else
#ifdef GP32
	asm("mov        r3,#0; bx       r3;");
#else
    	TV_Quit();
#endif
#endif
	exit(0);
}

int fsl_theend=0;

void main_loop(void)
{
#ifdef DREAMCAST
puts("MAIN!!!");
#endif
      p_fsl_game my_game=NULL;
      while(!fsl_theend)
      {
    	if (!fsl_init_video())
 	 {
		SDL_JoystickEventState (SDL_ENABLE) ;
		SDL_JoystickOpen(0);
		SDL_JoystickOpen(1);
		SDL_JoystickOpen(2);
		SDL_JoystickOpen(3);
		fsl_init_display_error();
 	   	fsl_init_audio();
 	   	if (!fsl_parse_game())
 		{
			if (fsl_first!=NULL)
 	   			my_game=fsl_process();
			else
			{
				fsl_display_error_black(parse_error);
				fsl_theend=1;
			}
    		}
    		else
		{
			fsl_display_error_black(parse_error);
			fsl_theend=1;
		}
		fsl_quit_audio();
		fsl_quit_display_error();
		fsl_quit_video();
   	  }
		else fsl_theend=1;
    	  if (my_game!=NULL)
		  fsl_theend=fsl_run(my_game->exec);
      }
      fsl_quit();
}

#ifndef TARGET_GP32
int main(int n_arg, char** args)
#else
void GpMain(void *argv)
#endif
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) >= 0)
	    main_loop();
#ifndef TARGET_GP32
    return 0;
#endif
}

void fsl_exit(int n)
{
	if (n)
		fsl_sayme_error();
//	main_loop();
	fsl_quit();
        exit(n);
}
