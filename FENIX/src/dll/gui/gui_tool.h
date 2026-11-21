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
 * FILE        : gui_tool.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_TOOL_H
#define __GUI_TOOL_H

/* ------------------------------------------------------------------------- * 
 *  TOOL BUTTON (BUTTON WITH AN OPTIONAL GRAPHIC)
 * ------------------------------------------------------------------------- */

#define TOOL_STATE_CHECK 0x0FFFFFF0

typedef struct _tool
{
	BUTTON		button;				/**< Parent class data */
	GRAPH	  * graph;				/**< Graphic of the tool */
	MENUITEM  * menuoptions;		/**< Pointer to the menu */
	int			menucount;			/**< Number of options */
	int			offsetx;			/**< Offset of the text/graphic */
	int			offsety;			/**< Offset of the text/graphic */
	CONTROL   * menu;				/**< Opened menu */
	int			frame;				/**< 1 to draw a frame around the button */
	CONTROL   * lefttool;			/**< Tool to open when the user presses LEFT */
	CONTROL	  * righttool;			/**< Tool to open when the user presses RIGHT */
	int		  * variable;			/**< State variable (for stated buttons) */
	int		    state;				/**< Pressed state value or TOOL_STATE_CHECK if it is radio style */
}
TOOL;

extern CONTROL * gui_tool_new (GRAPH * graph);
extern CONTROL * gui_tool_newt (const char * text);
extern CONTROL * gui_tool_newm (const char * text, MENUITEM *, int);
extern CONTROL * gui_tool_new_check (GRAPH * graph, int * variable);
extern CONTROL * gui_tool_new_radio (GRAPH * graph, int * variable, int value);
extern void      gui_tool_context (CONTROL *, CONTROL * left, CONTROL * right);

#endif
