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
 *  Contains the CONTROL:DROPDOWN class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

static void callback (DROPDOWN * dropdown, int option)
{
	if (*dropdown->option != option)
	{
		if (*dropdown->option)
			string_discard (*dropdown->option);
		*dropdown->option = option;
		if (option)
			string_use (option);
	}
}

/** Show the drop-down control's menu
 *
 *	@param c		Pointer to the control object
 */

void gui_dropdown_showmenu (CONTROL * c)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;
	MENUITEM * m;
	int i;
	int selected = -1;
	CONTROL * menu;

	m = (MENUITEM *)malloc(sizeof(MENUITEM) * dropdown->options_count);
	for (i = 0 ; i < dropdown->options_count ; i++)
	{
		if (strcmp (string_get(dropdown->options[i]), string_get(*dropdown->option)) == 0)
			selected = i;
		m[i] = gui_menuitem (string_get(dropdown->options[i]));
		m[i].callbackparam[0] = (int)c;
		m[i].callbackparam[1] = dropdown->options[i];
		m[i].callback = callback;
	}
	menu = gui_menu_new (m, i);
	((MENU *)menu)->selected = selected;
	if (menu->width < c->width)
		menu->width = c->width;
	menu->parent = c;
	gui_desktop_addpopup (c->window, menu, c->x, c->y + c->height);
	dropdown->popup = menu;
	free (m);
}

/** Draw a drop-down control.
 *  This is a member function of the CONTROL:DROPDOWN class
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_dropdown_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;
	REGION region;

	gr_setcolor (color_border) ;
	if (c->focused)
	{
		gr_hline (dest, clip, x, y, c->width-1);
		gr_vline (dest, clip, x, y, c->height-1);
	}
	gr_hline (dest, clip, x+1, y+c->height-1, c->width-1);
	gr_vline (dest, clip, x+c->width-1, y+1, c->height-1);
	gr_setcolor (c->highlight ? color_highlightface : color_face);
	gr_box (dest, clip, x+2, y+2, c->width-4, c->height-4);
	gr_setcolor (!dropdown->pressed ? color_highlight : color_shadow);
	gr_hline (dest, clip, x+1, y+1, c->width-3);
	gr_vline (dest, clip, x+1, y+1, c->height-3);
	gr_setcolor (!dropdown->pressed ? color_shadow : color_face);
	gr_hline (dest, clip, x+2, y+c->height-2, c->width-3);
	gr_vline (dest, clip, x+c->width-2, y+2, c->height-3);

	if (dropdown->pressed) x++, y++;

	/*
	if (!c->highlight)
	{
		gr_setcolor (color_input);
		gr_box (dest, clip, x+3, y+2, c->width - 24, c->height-4);
	}
	if (c->focused)
	{
		gr_setcolor (color_shadow);
		gr_rectangle (dest, clip, x+3, y+2, c->width - 24, c->height-5);
	}
	*/

	region.x = x+1;
	region.y = y+1;
	region.x2 = x+c->width-2;
	region.y2 = y+c->height-2;
	region_union (&region, clip);

	gr_setcolor (color_text);
	gui_text_put (dest, &region, x+6, y+3, string_get(*dropdown->option), 0, 0);

	x += c->width - 16;
	y += 5;
	gr_hline (dest, &region, x,   y,   8);
	gr_hline (dest, &region, x+1, y+1, 6);
	gr_hline (dest, &region, x+2, y+2, 4);
	gr_hline (dest, &region, x+3, y+3, 2);
}

/** Change the dropdown's pressed state when the user clicks it.
 *  Also, create and invoke a pop-up menu with the available options.
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the dropdown pressed or released
 *  @param pressed	1 if the dropdown was pressed, 0 if it was released
 * */

int gui_dropdown_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;

	if (pressed)
	{
		gui_desktop_removepopup_p (0, c);
		gui_dropdown_showmenu (c);
	}
	dropdown->pressed = pressed;
	return 1;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 */

int gui_dropdown_leave (CONTROL * c)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;
	if (dropdown->popup)
	{
		gui_desktop_removepopup_p (dropdown->popup, c);
		dropdown->popup = 0;
	}
	dropdown->pressed = 0;
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** The user moves the focus to the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *  @return			1 if the control is capable of focusing
 */

int gui_dropdown_enter (CONTROL * c)
{
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the control member (at offset 0 of the object)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_dropdown_key (CONTROL * c, int scancode, int character)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;

	if (scancode == KEY_SPACE || scancode == KEY_RETURN || scancode == KEY_DOWN || scancode == KEY_UP)
	{
		gui_dropdown_showmenu (c);
		return 1;
	}
	return 0;
}

/** Destructor for drop-down controls
 *
 *	@param c		Drop-down control object
 */

void gui_dropdown_destructor (CONTROL * c)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;
	int i;

	for (i = 0 ; i < dropdown->options_count ; i++)
		string_discard (dropdown->options[i]);
	if (dropdown->options)
		free (dropdown->options);
	
	gui_control_destructor (c);
}

/** Create a new drop-down control 
 *
 *  @param width 	Width of the control in pixels
 *  @param var 		Pointer to the STRING variable with current text
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_dropdown_new (int width, int * var)
{
	DROPDOWN * dropdown ;

	/* Alloc memory for the struct */
	dropdown = (DROPDOWN *) malloc(sizeof(DROPDOWN));
	if (dropdown == NULL)
		return NULL;
	gui_control_init (&dropdown->control);

	/* Fill the control control struct data members */
	dropdown->control.bytes = sizeof(DROPDOWN);
	dropdown->control.width = width;
	dropdown->control.height = 14;
	dropdown->control.hresizable = 1;
	dropdown->control.min_width = 48;

	/* Fill the control control struct member functions */
	dropdown->control.draw = gui_dropdown_draw;
	dropdown->control.mousebutton = gui_dropdown_mousebutton;
	dropdown->control.enter = gui_dropdown_enter;
	dropdown->control.leave = gui_dropdown_leave;
	dropdown->control.key = gui_dropdown_key;
	dropdown->control.destructor = gui_dropdown_destructor;

	/* Fill the rest of data members */
	dropdown->options = 0;
	dropdown->options_count = 0;
	dropdown->options_allocated = 0;
	dropdown->pressed = 0;
	dropdown->option = var;
	dropdown->popup = 0;

	return &dropdown->control;
}

/** Add a option to the drop-down control
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *	@param option	FXI string with the option
 */

void gui_dropdown_addoption (CONTROL * c, int string)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;

	if (dropdown->options_allocated == dropdown->options_count)
	{
		dropdown->options_allocated += 16;
		dropdown->options = (int *) realloc (dropdown->options,
			sizeof(int) * dropdown->options_allocated);
	}

	dropdown->options[dropdown->options_count++] = string;
	string_use(string);
}

/** Add multiple options to the drop-down control
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *	@param option	FXI string with the option
 */

void gui_dropdown_addoptions (CONTROL * c, int * option, int count)
{
	DROPDOWN * dropdown = (DROPDOWN *)c;

	if (dropdown->options_allocated + count >= dropdown->options_count)
	{
		dropdown->options_allocated += ((count + 32) & ~15);
		dropdown->options = (int *) realloc (dropdown->options,
			sizeof(int) * dropdown->options_allocated);
	}

	while (count--)
	{
		dropdown->options[dropdown->options_count++] = *option;
		string_use(*option);
		option++;
	}
}
