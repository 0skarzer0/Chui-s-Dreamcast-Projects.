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
 * FILE        : gui_palettepanel.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_PALETTEPANEL_H
#define __GUI_PALETTEPANEL_H

/* ------------------------------------------------------------------------- * 
 *  PALETTE PANEL
 * ------------------------------------------------------------------------- */

typedef struct _palettepanel
{
	CONTROL		control;			/*< Parent class data */
	int	*		selected;			/*< Variable with the selected color */
	int			hovered;			/*< Number of the hovered color */
	int			cell_width;			/*< Width in pixels of a color cell */
	int			cell_height;		/*< Height in pixels of a color cell */
	char *		selection;			/*< Pointer to selection */
}
PALETTEPANEL;

extern CONTROL * gui_palettepanel_new (int * selected);
extern CONTROL * gui_palettepanel_news (int width, int height, int * selected);
extern void      gui_palettepanel_selection (CONTROL * pp, char * selection);

#endif
