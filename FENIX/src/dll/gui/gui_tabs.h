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
 * FILE        : gui_tabs.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_TABS_H
#define __GUI_TABS_H

/* ------------------------------------------------------------------------- * 
 *  TABBED DIALOG
 * ------------------------------------------------------------------------- */

typedef struct _tabpage
{
	CONTAINER *	container;			/**< Page controls container */
	int         title;				/**< Title string */
	int			title_x;			/**< X left coordinate of the title tab */
	int			title_width;		/**< Width in pixels of the title tab */
}
TABPAGE;

typedef struct _tabs
{
	CONTROL		control;			/**< Parent class data */
	TABPAGE *	pages;				/**< Dialog Pages */
	int			pages_count;		/**< Number of pages */
	int			pages_allocated;	/**< Number of slots availables for pages */
	int			current_page;		/**< Current tab */
	int			highlight_page;		/**< Current tab highlighted by mouse */
	int			tabs_width;			/**< Combined width of all title tabs */
	int			border;				/**< Inner border for containers */
}
TABS;

extern CONTROL * gui_tabs_new (int width, int height);
extern void      gui_tabs_init (TABS *); 
extern void      gui_tabs_addpage (CONTROL *, int page_string);
extern void		 gui_tabs_setpage (CONTROL *, int index);
extern int 		 gui_tabs_searchpage (CONTROL *, int page_string);
extern void		 gui_tabs_addcontrol (CONTROL *, int x, int y, CONTROL * control);

#endif
