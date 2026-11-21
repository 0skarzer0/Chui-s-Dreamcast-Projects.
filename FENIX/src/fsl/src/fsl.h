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

#ifndef FENIX_SDL_LOADER_H
#define FENIX_SDL_LOADER_H

#ifdef DREAMCAST
#include <kos.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "SFont.h"
#include "tvfilter/tvfilter.h"

#define FSL_EXEC_TYPE_DCB 0
#define FSL_EXEC_TYPE_BIN 1

#define _FSL_FPS_ 50
#define FSL_FPS (1000 / _FSL_FPS_)

#ifdef GP32
#define DPRE "gp:\\gpmm\\fenix\\"
#else
#ifdef DREAMCAST
//#define DPRE "/rd/"
#define DPRE "/cd/data/"
#else
#define DPRE "data/"
#endif
#endif

typedef void* _p_fsl_game;

typedef struct {
        char *name;
	char *author;
	char *description;
	char *exec;
	SDL_Surface *image;
	int type;
	_p_fsl_game *next, *back;
}fsl_game;

typedef fsl_game * p_fsl_game;

extern p_fsl_game fsl_first;
extern char *fsl_games_name;

extern char *fsl_unknow;
extern char *fsl_anonymous;
extern char *fsl_no_description;
extern char *fsl_empty;

extern SDL_Surface *fsl_screen, *fsl_dcimage;

extern SFont_FontInfo fsl_font;

#define font_put(X,Y,MSG) (SFont_PutStringInfo(fsl_screen,&fsl_font,X,Y,MSG))

extern int fsl_init_audio(void);
extern int fsl_init_video(void);
extern void fsl_stretch16(SDL_Surface *source, SDL_Rect *r);
extern int fsl_parse_game(void);
extern p_fsl_game fsl_process(void);
extern void fsl_draw(p_fsl_game actual, int frame);
extern void fsl_init_display_error(void);
extern void fsl_invert_screen_color(void);
extern void fsl_display_error_black(char *error_display);
extern void fsl_display_error(char *error_display);
extern void fsl_display_error_invert(char *error_display);
extern void fsl_clean_events(void);
extern void fsl_exec_bin(char *name);
extern void fsl_set_window_title(void);

extern void fsl_quit_audio(void);
extern void fsl_quit_display_error(void);
extern void fsl_quit_video(void);

extern void fsl_play_ping(void);

extern int fsl_run(char* exec);
extern void fsl_sayme_error(void);

#endif
