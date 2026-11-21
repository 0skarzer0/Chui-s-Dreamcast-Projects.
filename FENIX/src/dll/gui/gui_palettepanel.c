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

/** @file
 *
 *  Contains the CONTROL:PALETTEPANEL class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Draw a palette panel.
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_palettepanel_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	PALETTEPANEL * panel = (PALETTEPANEL *)c;

	int a, b;
	int cw = c->width / 16;
	int ch = c->height / 16;
	int cx, cy;

	panel->cell_width = cw;
	panel->cell_height = ch;

	for (a = 0 ; a < 16 ; a++)
	{
		for (b = 0 ; b < 16 ; b++)
		{
			int c = a + 16*b;
			if (enable_16bits)
				gr_setcolor (gr_rgb ((*_palette)[c].r, (*_palette)[c].g, (*_palette)[c].b));
			else
				gr_setcolor (c);

			gr_box (dest, clip, cx = x+cw*a, cy = y+ch*b, cw, ch);
			
			if (panel->selection && panel->selection[c])
			{
				gr_setcolor(color_shadow);
				gr_hline (dest, clip, cx+1, cy+1, cw-2);
				gr_vline (dest, clip, cx+1, cy+1, ch-2);
				gr_setcolor(color_highlight);
				gr_hline (dest, clip, cx+1, cy+ch-2, cw-2);
				gr_vline (dest, clip, cx+cw-2, cy+1, ch-2);
			}
		}
	}

	if (panel->selected && *panel->selected >= 0)
	{
		a = (*panel->selected)&15;
		b = (*panel->selected)/16;
		gr_setcolor (color_selection);
		gr_rectangle (dest, clip, x+cw*a, y+ch*b, cw, ch);
		gr_rectangle (dest, clip, x+cw*a-1, y+ch*b-1, cw+2, ch+2);
	}

	if (panel->hovered >= 0)
	{
		a = (panel->hovered)&15;
		b = (panel->hovered)/16;
		gr_setcolor (color_highlight);
		gr_hline (dest, clip, x+cw*a, y+ch*b, cw);
		gr_vline (dest, clip, x+cw*a, y+ch*b, ch);
		gr_setcolor (color_shadow);
		gr_hline (dest, clip, x+cw*a, y+ch*b+ch, cw);
		gr_vline (dest, clip, x+cw*a+cw, y+ch*b, ch);
	}
}

/** Response to mouse move events
 *  This is a member function of the CONTROL:SCROLLBAR class
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param buttons	Button state (1 bit for button)
 * */

int gui_palettepanel_mousemove (CONTROL * c, int x, int y, int buttons)
{
	PALETTEPANEL * panel = (PALETTEPANEL *)c;

	int cx, cy;

	cx = x / panel->cell_width;
	cy = y / panel->cell_height;

	if (panel->selected && cx >= 0 && cx <= 15 && cy >= 0 && cy <= 15)
	{
		if (buttons & 1)
		{
			(*panel->selected) = cy* 16 + cx;
			if (panel->selection)
				panel->selection[cy*16 + cx] = 1;
		}
		panel->hovered = cy*16 + cx;
	}
	else
	{
		panel->hovered = -1;
	}
	return 1;
}

/** Response to mouse clicks
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the scrollbar pressed or released
 *  @param pressed	1 if the scrollbar was pressed, 0 if it was released
 * */

int gui_palettepanel_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	PALETTEPANEL * panel = (PALETTEPANEL *)c;

	if (b == 0 && pressed == 1)
	{
		if (panel->selected)
		{
			if (panel->selection && panel->hovered >= 0)
			{
				if (gr_key(KEY_LCTRL) || gr_key(KEY_RCTRL))
				{
					panel->selection[panel->hovered] ^= 1;
					(*panel->selected) = panel->hovered;
				}
				else if ((gr_key(KEY_LSHIFT) || gr_key(KEY_RSHIFT)) && *panel->selected >= 0)
				{
					int i, inc = panel->hovered > *panel->selected ? 1 : -1;
					memset (panel->selection, 0, 256);
					for (i = *panel->selected ; i != panel->hovered ; i += inc)
						panel->selection[i] = 1;
					panel->selection[i] = 1;
				}
				else
				{
					memset (panel->selection, 0, 256);
					panel->selection[panel->hovered] = 1;
					(*panel->selected) = panel->hovered;
				}
			}
			else
				(*panel->selected) = panel->hovered;
			
			return 1;
		}
	}
	return 0;
}

/** The mouse leaves the control
 *
 *  @param c		Pointer to the control
 * */

int gui_palettepanel_mouseleave (CONTROL *c)
{
	PALETTEPANEL * panel = (PALETTEPANEL *)c;
	panel->hovered = -1;
	return 1;
}

/** Set the selection array for a palette panel. This actually enables
 *  multiple-color selection (altough there will be still a primary
 *  selected color). Set it to 0 to disable multiple selections-
 *
 *	@param c			Pointer to the control
 *	@param selection	Pointer to a 256-bytes selection array
 **/

void gui_palettepanel_selection (CONTROL * c, char * selection)
{
	((PALETTEPANEL *)c)->selection = selection;
}

/** Create a new palette panel
 *
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_palettepanel_new (int * selected)
{
	PALETTEPANEL * palettepanel ;

	/* Alloc memory for the struct */
	palettepanel = (PALETTEPANEL *) malloc(sizeof(PALETTEPANEL));
	if (palettepanel == NULL)
		return NULL;
	gui_control_init (&palettepanel->control);

	palettepanel->selected = selected;
	palettepanel->hovered = -1;
	palettepanel->selection = 0;

	/* Fill the control control struct data members */
	palettepanel->control.bytes  = sizeof(PALETTEPANEL);
	palettepanel->control.width  = 16 * 4;
	palettepanel->control.height = 16 * 4;
	palettepanel->control.bgmode = BG_SOLID;

	/* Fill the control control struct member functions */
	palettepanel->control.draw = gui_palettepanel_draw;
	palettepanel->control.mousemove = gui_palettepanel_mousemove;
	palettepanel->control.mousebutton = gui_palettepanel_mousebutton;
	palettepanel->control.mouseleave = gui_palettepanel_mouseleave;
	palettepanel->control.vresizable = 1;
	palettepanel->control.hresizable = 1;

	return &palettepanel->control;
}

/** Create a new palette panel with a given size
 *
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_palettepanel_news (int width, int height, int *selected)
{
	CONTROL * palettepanel = gui_palettepanel_new(selected);

	palettepanel->width = (width & ~15);
	palettepanel->height = (height & ~15);
	palettepanel->min_width = 16*4;
	palettepanel->min_height = 16*4;
	palettepanel->height_step = 16;
	palettepanel->width_step = 16;

	return palettepanel;
}

