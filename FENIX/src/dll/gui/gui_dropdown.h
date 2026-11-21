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
 * FILE        : gui_dropdown.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_DROPDOWN_H
#define __GUI_DROPDOWN_H

/* ------------------------------------------------------------------------- * 
 *  DROP-DOWN LIST
 * ------------------------------------------------------------------------- */

/** CONTROL:DROPDOWN, A button with a pop-up menu */

typedef struct _dropdown
{
	CONTROL		control;					/**< Parent class data */
	CONTROL   * popup;						/**< Popup menu, if any */
	int		  * option;						/**< Variable with current value */
	int		  * options;					/**< Array of FXI strings */
	int			options_count;				/**< Number of FXI strings */
	int			options_allocated;			/**< Allocated space for options */
	int			pressed;					/**< 1 if checkbox is pressed */
	int			processtype;				/**< Callback process type */
}
DROPDOWN;

extern CONTROL * gui_dropdown_new (int width, int * var);
extern void		 gui_dropdown_addoption (CONTROL * c, int option);
extern void		 gui_dropdown_addoptions (CONTROL * c, int * option, int count);

#endif
