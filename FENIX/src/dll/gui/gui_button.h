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
 * FILE        : gui_button.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_BUTTON_H
#define __GUI_BUTTON_H

/* ------------------------------------------------------------------------- * 
 *  PUSH BUTTON
 * ------------------------------------------------------------------------- */


#define BUTTON_TEXT_MAX		48

/** CONTROL:BUTTON, A button that can be pressed by the user */

typedef struct _button
{
	CONTROL		control;					/**< Parent class data */
	char		text[BUTTON_TEXT_MAX];		/**< Text of the button */
	int			pressed;					/**< 1 if button is pressed */
	void 	 (* callback ) ();				/**< Callback function */
	long		callbackparam[2];			/**< Callback parameters */
	int			callbackparamcount;			/**< Number of parameters */
	int			was_pressed;				/**< Is set to 1 when the button is pressed */
}
BUTTON;

extern CONTROL * gui_button_new (const char * text);
extern void      gui_button_init (BUTTON *, const char * text);
extern void      gui_button_callback (CONTROL *, void (*callback)());
extern void      gui_button_callbackp (CONTROL *, void (*callback)(), void * param);
extern void      gui_button_callbacki (CONTROL *, void (*callback)(), int param);
extern void		 gui_button_callbackpi (CONTROL * control, void (*callback)(), void * p, int i);
extern void      gui_button_text (CONTROL *, const char * text);
extern void      gui_button_action (BUTTON * button);

#endif
