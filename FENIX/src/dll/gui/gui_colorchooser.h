/*
 *  Fenix - Videogame compiler/interpreter
 *  Current release       : FENIX - PROJECT 1.0 - R 0.9
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
 * FILE        : gui_colorchooser.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_COLORCHOOSER_H
#define __GUI_COLORCHOOSER_H

/* ------------------------------------------------------------------------- * 
 *  COLOR CHOOSER / PALETE EDITOR
 * ------------------------------------------------------------------------- */

#define CCM_ENABLETRANSPARENT		0x01
#define CCM_ENABLENONE				0x02
#define CCM_ENABLEPALETTE			0x04
#define CCM_ENABLEOK				0x08
#define CCM_8BITS					0x10

/** CONTROL:CONTAINER:COLORCHOOSER, a color chooser and palette editor control */

typedef struct _colorchooser
{
	CONTAINER	container;					/**< Parent class data */
	int *		color;						/**< Color to edit, or NULL if none */
	int			previous_color;				/**< Old value of color, to detect external changes */
	SDL_Color *	palette;					/**< Palette to use, or NULL if none */
	int			mode;						/**< CCM_* flags */
	int			h, s, l;					/**< Last H S L values */
	int			r, g, b;					/**< Last R G B values */
	char		tr[8], tg[8], tb[8];		/**< Inputline texts */
	char		th[8], ts[8], tl[8];		/**< Inputline texts */
	int			transparent;				/**< 1 if transparent is checked */
	int			none;						/**< 1 if none is checked */
}
COLORCHOOSER;

/** CONTROL:CLICKABLECOLOR, a color the user can change clicking on it via a pop-up */

typedef struct _clickablecolor
{
	CONTROL		control;					/**< Parent class data */
	int *		color;						/**< Color to edit, or NULL if none */
	int			mode;						/**< CCM_* flags */
}
CLICKABLECOLOR;

extern CONTROL * gui_colorchooser_newc (int * color, int mode);
extern CONTROL * gui_color_newc (int * color, int mode);
extern CONTROL * gui_clickablecolor_new (int * color, int mode);

#endif
