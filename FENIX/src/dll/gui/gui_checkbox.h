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
 * FILE        : gui_checkbox.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_CHECKBOX_H
#define __GUI_CHECKBOX_H

/* ------------------------------------------------------------------------- * 
 *  CHECKBOX
 * ------------------------------------------------------------------------- */

#define CHECKBOX_TEXT_MAX		48

/** CONTROL:CHECKBOX, A two-state button that can be pressed by the user */

typedef struct _checkbox
{
	CONTROL		control;					/**< Parent class data */
	char		text[CHECKBOX_TEXT_MAX];	/**< Text of the checkbox */
	int			pressed;					/**< 1 if checkbox is pressed */
	int		  * checked;					/**< 1 if checkbox is checked */
	void 	 (* callback ) ();				/**< Callback function */
}
CHECKBOX;

#define CHECKBOX_READ	0
#define CHECKBOX_SET  	1
#define CHECKBOX_UNSET	2
#define CHECKBOX_TOGGLE	3

extern CONTROL * gui_checkbox_new (const char * text, int * var);
extern int		 gui_checkbox_state (CONTROL * c, int action);

#endif
