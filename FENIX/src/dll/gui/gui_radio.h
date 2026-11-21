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
 * FILE        : gui_radio.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_RADIO_H
#define __GUI_RADIO_H

/* ------------------------------------------------------------------------- * 
 *  RADIO
 * ------------------------------------------------------------------------- */


#define RADIO_TEXT_MAX		48

/** CONTROL:RADIO, A two-state button that can be pressed by the user */

typedef struct _radio
{
	CONTROL		control;					/**< Parent class data */
	char		text[RADIO_TEXT_MAX];		/**< Text of the button */
	int			pressed;					/**< 1 if radio is pressed */
	int		  * variable;					/**< ID of pressed radio */
	int			value;						/**< ID of this radio */
	void 	 (* callback ) ();				/**< Callback function */
}
RADIO;

#define RADIO_READ		0
#define RADIO_SET  		1

extern CONTROL * gui_radio_new (const char * text, int * var, int value);
extern int		 gui_radio_state (CONTROL * c, int action);

#endif
