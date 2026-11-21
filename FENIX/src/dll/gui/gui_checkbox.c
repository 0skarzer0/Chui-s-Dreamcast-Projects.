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
 *  Contains the CONTROL:CHECKBOX class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

#define BWIDTH	12

/** Draw a checkbox.
 *  This is a member function of the CONTROL:CHECBOX class
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the checkbox)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_checkbox_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	CHECKBOX * checkbox   = (CHECKBOX *)c ;

	int BHEIGHT = c->height-2;

	/* Draw the checkbox */
	gr_setcolor (color_border) ;
	if (c->focused)
	{
		gr_hline (dest, clip, x, y, BWIDTH-1);
		gr_vline (dest, clip, x, y, BHEIGHT-1);
	}
	gr_hline (dest, clip, x+1, y+BHEIGHT-1, BWIDTH-1);
	gr_vline (dest, clip, x+BWIDTH-1, y+1, BHEIGHT-1);
	gr_setcolor (c->highlight ? color_highlightface : color_face);
	gr_box (dest, clip, x+2, y+2, BWIDTH-4, BHEIGHT-4);
	gr_setcolor (!checkbox->pressed ? color_highlight : color_shadow);
	gr_hline (dest, clip, x+1, y+1, BWIDTH-3);
	gr_vline (dest, clip, x+1, y+1, BHEIGHT-3);
	gr_setcolor (!checkbox->pressed ? color_shadow : color_face);
	gr_hline (dest, clip, x+2, y+BHEIGHT-2, BWIDTH-3);
	gr_vline (dest, clip, x+BWIDTH-2, y+2, BHEIGHT-3);

	/* Draw the check mark */
	if (*checkbox->checked)
	{
		int off = (checkbox->pressed ? 0:-1);

		gr_setcolor (color_border);
		gr_line (dest, clip, off+x+4, off+y+BHEIGHT-5, 1, 2);
		gr_line (dest, clip, off+x+5, off+y+BHEIGHT-3, BWIDTH-8, -BHEIGHT+6);
		gr_line (dest, clip, off+x+BWIDTH-3, off+y+3, 2, 0);
	}

	/* Draw the text */
	gr_setcolor (color_border);
	x += BWIDTH + 6;
	y += (BHEIGHT - gr_text_height (0, checkbox->text)) / 2 ;
	gr_setcolor (color_text);
	gr_text_setcolor (color_text);
	gr_text_put (dest, clip, 0, x, y, checkbox->text) ;

	/* Draw the focus border */
	if (c->focused)
	{
		gr_line (dest, clip, x, y+gr_text_height(0, checkbox->text), 
				gr_text_width(0,checkbox->text), 0 /*, 0x55555555*/);
	}
}

/** Change the checkbox's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:CHECKBOX class
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the checkbox)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the checkbox pressed or released
 *  @param pressed	1 if the checkbox was pressed, 0 if it was released
 * */

int gui_checkbox_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	CHECKBOX * checkbox = (CHECKBOX *)c;

	if (checkbox->pressed != pressed)
	{
		if (checkbox->pressed && !pressed)
		{
			*checkbox->checked = !*checkbox->checked;
			checkbox->control.actionflags |= ACTION_CHANGE;
			if (checkbox->callback)
				(*checkbox->callback)();
		}

		checkbox->pressed = pressed;
		c->redraw = 1;
	}
	return 1;
}

/** The mouse leaves a checkbox, change its pressed state.
 *  This is a member function of the CONTROL:CHECKBOX class
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the checkbox)
 * */

int gui_checkbox_mouseleave (CONTROL *c)
{
	CHECKBOX * checkbox = (CHECKBOX *)c ;

	if (checkbox->pressed)
	{
		checkbox->pressed = 0;
		c->redraw = 1;
	}
	return 1;
}

/** The user moves the focus to the checkbox
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the checkbox)
 *  @return			1 if the control is capable of focusing
 */

int gui_checkbox_enter (CONTROL * c)
{
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the checkbox)
 */

int gui_checkbox_leave (CONTROL * c)
{
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the checkbox.control member (at offset 0 of the checkbox)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_checkbox_key (CONTROL * c, int scancode, int character)
{
	CHECKBOX * checkbox = (CHECKBOX *)c;

	if (scancode == KEY_SPACE || scancode == KEY_RETURN)
	{
		*checkbox->checked = !*checkbox->checked;
		checkbox->control.actionflags |= ACTION_CHANGE;
		c->redraw = 1;
		return 1;
	}
	return 0;
}


/** Check if the control supports the given acces key (or any at all)
 *  This is done searching the text of the control for the \b control character
 *
 *  @param control	Pointer to the control
 *	@param key		Character pressed or 0 to check if any is present
 **/

int gui_checkbox_syskey (CONTROL * control, int key)
{
	CHECKBOX * checkbox = (CHECKBOX *) control;

	const char * ptr = checkbox->text;

	while (*ptr)
	{
		if (*ptr == '\b')
		{
			if (!key || toupper(ptr[1]) == toupper(key))
				return 1 ;
		}
		ptr++;
	}
	return 0;
}

/** Create a new checkbox control 
 *
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param text 	Label of the checkbox
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_checkbox_new (const char * text, int * var)
{
	CHECKBOX * checkbox ;

	/* Alloc memory for the struct */
	checkbox = (CHECKBOX *) malloc(sizeof(CHECKBOX));
	if (checkbox == NULL)
		return NULL;
	gui_control_init (&checkbox->control);

	/* Fill the control control struct data members */
	checkbox->control.bytes = sizeof(CHECKBOX);
	checkbox->control.width = BWIDTH + 4 + gr_text_width (0, text);
	checkbox->control.height = 12;

	/* Fill the control control struct member functions */
	checkbox->control.draw = gui_checkbox_draw;
	checkbox->control.mouseleave = gui_checkbox_mouseleave;
	checkbox->control.mousebutton = gui_checkbox_mousebutton;
	checkbox->control.enter = gui_checkbox_enter;
	checkbox->control.leave = gui_checkbox_leave;
	checkbox->control.key = gui_checkbox_key;
	checkbox->control.syskey = gui_checkbox_syskey;

	/* Fill the rest of data members */
	strncpy (checkbox->text, text, CHECKBOX_TEXT_MAX);
	checkbox->text[CHECKBOX_TEXT_MAX-1] = 0;
	checkbox->pressed = 0;
	checkbox->checked = var;
	checkbox->callback = 0;

	return &checkbox->control;
}

/** Consult or set the checkbox's state
 *
 *  @param c		Pointer to the checkbox.control member (at offset 0 of the button)
 *  @param action	Action to perform (CHECKBOX_READ, SET, UNSET or TOGGLE)
 *  @return			Returns the new value of the checkbox
 */

int gui_checkbox_state (CONTROL * c, int action)
{
	CHECKBOX * checkbox = (CHECKBOX *)c;

	if (action == CHECKBOX_SET)
		*checkbox->checked = 1;
	if (action == CHECKBOX_UNSET)
		*checkbox->checked = 0;
	if (action == CHECKBOX_TOGGLE)
		*checkbox->checked = !*checkbox->checked;

	return *checkbox->checked;
}

