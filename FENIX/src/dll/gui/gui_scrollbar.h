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
 * FILE        : gui_scrollbar.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_SCROLLBAR_H
#define __GUI_SCROLLBAR_H

/* ------------------------------------------------------------------------- * 
 *  SCROLL BARS
 * ------------------------------------------------------------------------- */

typedef struct _scrollpos
{
	int		min;				/**< Minimum position */
	int		max;				/**< Maximum position */
	int		step;				/**< Ammount of minimum scroll */
	int		pagestep;			/**< Ammount of scroll for page operations */
	int		pos;				/**< Current scroll position (min <= pos <= max) */
}
SCROLLPOS;

#define SCROLLBAR_BUTTONS	0x01
#define SCROLLBAR_FRAME		0x02
#define SCROLLBAR_FOCUSABLE 0x04
#define SCROLLBAR_FIXED		0x08

typedef struct _scrollbar
{
	CONTROL		control;		/**< Parent class data */
	int			style;			/**< Scrollbar style */
	int			pressed;		/**< Number of pressed element or -1 if none */
	int			pressedsection;	/**< If pressed the frame, which section of it */
	int			pressedtime;	/**< Time of the last mouse button press */
	SCROLLPOS * scroll;			/**< Pointer to scroll parameters struct */
	int			last_mousex;	/**< Last mouse position received or -1 if none */
	int			last_mousey;	/**< Last mouse position received or -1 if none */
	int			last_pos   ;	/**< Last scroll bar position on mouse button down */

	/** Position of each element (buttons, frame and thumb) */
	struct _scrollbarelement
	{
		int x, y, width, height;
	}
	element[4];
}
SCROLLBAR;

extern void      gui_hscrollbar_init (SCROLLBAR *, int width, SCROLLPOS * data);
extern void      gui_vscrollbar_init (SCROLLBAR *, int height, SCROLLPOS * data);
extern CONTROL * gui_hscrollbar_new (int width, SCROLLPOS * data);
extern CONTROL * gui_vscrollbar_new (int height, SCROLLPOS * data);
extern void gui_scrollbar_style (CONTROL *, int style);

#endif
