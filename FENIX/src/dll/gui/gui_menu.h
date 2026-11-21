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
 * FILE        : gui_menu.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_MENU_H
#define __GUI_MENU_H

/* ------------------------------------------------------------------------- * 
 *  MENU
 * ------------------------------------------------------------------------- */

#define MENUITEM_SEPARATOR	0
#define MENUITEM_ACTION		1
#define MENUITEM_TOGGLE		2
#define MENUITEM_RADIO		3
#define MENUITEM_SUBMENU	4

#define MENUITEM_TEXT_MAX	64
typedef struct _menuitem
{
	int		type;
	char	text[64];
	void *	data;
	int		value;
	int		width;
	int		height;
	int		enabled;
	void (* callback ) ();				/**< Callback function */
	long    callbackparam[2];			/**< Callback parameters */

	void	(*draw)(struct _menuitem *, GRAPH * graph, REGION * clip, int x, int y, int width);
}
MENUITEM;

MENUITEM gui_menuitem (const char * text);
MENUITEM gui_menuitem_separator ();
MENUITEM gui_menuitem_toggle (const char * text, int * variable);
MENUITEM gui_menuitem_radio (const char * text, int * variable, int value);
MENUITEM gui_menuitem_submenu (const char * text, MENUITEM * options, int count);

typedef struct _menu
{
	CONTROL  		control;		/**< Parent class data */
	CONTROL   *		caller;			/**< Control that created the menu */
	int				count;			/**< Item count */
	int				selected;		/**< Item selected */
	int				selectedtime;	/**< Time of selection of the item */
	int				pressed;		/**< 1 if mouse button is pressed */
	struct _menu *	submenu;		/**< Any submenu opened */
	struct _menu *	parentmenu;		/**< If the menu is a child, parent */
	int				submenuitem;	/**< Number of item with the opened submenu */
	int				parentmenuitem;	/**< Number of item in the parent */
	CONTROL		  * lefttool;		/**< Tool to open when the user presses LEFT */
	CONTROL		  * righttool;		/**< Tool to open when the user presses RIGHT */
	MENUITEM 		item[1];		/**< Item array */
}
MENU;

CONTROL * gui_menu_new (MENUITEM * items, int count);
void      gui_menu_context (CONTROL * menu, CONTROL * left, CONTROL * right);

#endif
