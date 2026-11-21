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
 * FILE        : gui_toolbar.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_TOOLBAR_H
#define __GUI_TOOLBAR_H

/* ------------------------------------------------------------------------- * 
 *  MENU BAR / TOOL BAR
 * ------------------------------------------------------------------------- */

typedef struct _toolbar
{
	CONTAINER	container;			/**< Parent class data */
	CONTROL  ** tools;				/**< Array of all menu types (for left/right key navigation) */
	int *		tools_type;			/**< String of "window type" for each element */
	int         tools_allocated;	/**< Measure of space allocated for the array */
	int			tools_count;		/**< Number of tools in the array */
	int			pressed;			/**< Index of last pressed button */
}
TOOLBAR;

extern CONTROL * gui_toolbar_new ();
extern void      gui_toolbar_addmenu (CONTROL *, const char * text, MENUITEM *, int count);
extern void		 gui_toolbar_settype (CONTROL *, int index, int type);
extern void      gui_toolbar_addbutton (CONTROL *, GRAPH * graph);
extern void      gui_toolbar_addbutton_tooltip (CONTROL *, GRAPH * graph, int tooltip);
extern void      gui_toolbar_addbutton_radio (CONTROL *, GRAPH * graph, int tooltip, int * var, int state);
extern void      gui_toolbar_addbutton_check (CONTROL *, GRAPH * graph, int tooltip, int * var);
extern void      gui_toolbar_addseparator (CONTROL *);
extern void		 gui_toolbar_addcontrol (CONTROL * c, CONTROL * user);
extern void      gui_toolbar_init (TOOLBAR *); 

#endif
