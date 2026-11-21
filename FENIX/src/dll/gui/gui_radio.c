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
 *  Contains the CONTROL:RADIO class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

#define RSIZE	3

/** Draw a radio.
 *  This is a member function of the CONTROL:CHECBOX class
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the radio)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_radio_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	RADIO * radio   = (RADIO *)c ;

	x += 3;
	y++;

	/* Draw the radio */
	gr_setcolor (color_border) ;
	if (c->focused)
		gr_circle (dest, clip, x+RSIZE-1, y+RSIZE-1, RSIZE+1);
	gr_setcolor (!radio->pressed ? color_highlight : color_shadow);
	gr_circle (dest, clip, x+RSIZE, y+RSIZE, RSIZE);
	gr_setcolor (!radio->pressed ? color_highlight : color_shadow);
	gr_circle (dest, clip, x-1+RSIZE, y+RSIZE, RSIZE);
	gr_setcolor (!radio->pressed ? color_shadow : color_face);
	gr_circle (dest, clip, x+RSIZE, y+1+RSIZE, RSIZE);
	gr_setcolor (c->highlight ? color_highlightface : color_face);
	gr_fcircle (dest, clip, x+RSIZE, y+RSIZE, RSIZE);

	/* Draw the check mark */
	if (*radio->variable == radio->value)
	{
		int off = (radio->pressed ? 0:-1);

		gr_setcolor (color_border);
		gr_fcircle (dest, clip, x+1+off+RSIZE, y+1+off+RSIZE, 2);
	}

	/* Draw the text */
	gr_setcolor (color_border);
	x += RSIZE*2 + 9;
	y += 3 - gr_text_height (0, radio->text) / 2 ;
	gr_text_setcolor (color_text);
	gr_text_put (dest, clip, 0, x, y, radio->text) ;

	/* Draw the focus border */
	if (c->focused)
	{
		gr_line (dest, clip, x, y+gr_text_height(0, radio->text), 
				gr_text_width(0,radio->text), 0 /*, 0x55555555*/);
	}
}

/** Change the radio's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:RADIO class
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the radio)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the radio pressed or released
 *  @param pressed	1 if the radio was pressed, 0 if it was released
 * */

int gui_radio_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	RADIO * radio = (RADIO *)c;

	if (radio->pressed != pressed)
	{
		if (radio->pressed && !pressed)
		{
			*radio->variable = radio->value ;
			radio->control.actionflags |= ACTION_CHANGE;
			if (radio->callback)
				(*radio->callback)();
		}

		radio->pressed = pressed;
		c->redraw = 1;
	}
	return 1;
}

/** The mouse leaves a radio, change its pressed state.
 *  This is a member function of the CONTROL:RADIO class
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the radio)
 * */

int gui_radio_mouseleave (CONTROL *c)
{
	RADIO * radio = (RADIO *)c ;

	if (radio->pressed)
	{
		radio->pressed = 0;
		c->redraw = 1;
	}
	return 1;
}

/** The user moves the focus to the radio
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the radio)
 *  @return			1 if the control is capable of focusing
 */

int gui_radio_enter (CONTROL * c)
{
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the radio)
 */

int gui_radio_leave (CONTROL * c)
{
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the radio.control member (at offset 0 of the radio)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_radio_key (CONTROL * c, int scancode, int character)
{
	RADIO * radio = (RADIO *)c;

	if (scancode == KEY_SPACE || scancode == KEY_RETURN)
	{
		*radio->variable = radio->value;
		radio->control.actionflags |= ACTION_CHANGE;
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

int gui_radio_syskey (CONTROL * control, int key)
{
	RADIO * radio = (RADIO *) control;

	const char * ptr = radio->text;

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


/** Create a new radio button control 
 *
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param text 	Label of the radio button
 *  @param var		Pointer to the variable with the ID of the pressed radio
 *  @param value	ID of this radio button
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_radio_new (const char * text, int * var, int value)
{
	RADIO * radio ;

	/* Alloc memory for the struct */
	radio = (RADIO *) malloc(sizeof(RADIO));
	if (radio == NULL)
		return NULL;
	gui_control_init (&radio->control);

	/* Fill the control control struct data members */
	radio->control.bytes = sizeof(RADIO);
	radio->control.width = RSIZE*2 + 11 + gr_text_width (0, text);
	radio->control.height = 12;

	/* Fill the control control struct member functions */
	radio->control.draw = gui_radio_draw;
	radio->control.mouseleave = gui_radio_mouseleave;
	radio->control.mousebutton = gui_radio_mousebutton;
	radio->control.enter = gui_radio_enter;
	radio->control.leave = gui_radio_leave;
	radio->control.key = gui_radio_key;
	radio->control.syskey = gui_radio_syskey;

	/* Fill the rest of data members */
	strncpy (radio->text, text, RADIO_TEXT_MAX);
	radio->text[RADIO_TEXT_MAX-1] = 0;
	radio->pressed = 0;
	radio->variable = var;
	radio->value = value;
	radio->callback = 0;

	return &radio->control;
}

/** Consult or set the radio's state
 *
 *  @param c		Pointer to the radio.control member (at offset 0 of the button)
 *  @param action	Action to perform (RADIO_READ or RADIO_SET)
 *  @return			Returns the new value of the radio
 */

int gui_radio_state (CONTROL * c, int action)
{
	RADIO * radio = (RADIO *)c;

	if (action == RADIO_SET)
		*radio->variable = radio->value;
	return *radio->variable;
}

