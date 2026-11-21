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
 *  Contains the CONTROL:BUTTON class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Draw a button.
 *  This is a member function of the CONTROL:BUTTON class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_button_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	BUTTON * button   = (BUTTON *)c ;

	/* Draw the button */
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
	gr_setcolor (!button->pressed ? color_highlight : color_shadow);
	gr_hline (dest, clip, x+1, y+1, c->width-3);
	gr_vline (dest, clip, x+1, y+1, c->height-3);
	gr_setcolor (!button->pressed ? color_shadow : color_face);
	gr_hline (dest, clip, x+2, y+c->height-2, c->width-3);
	gr_vline (dest, clip, x+c->width-2, y+2, c->height-3);

	/* Draw the text */
	gr_setcolor (color_border);
	x += (c->width  - gui_text_width  (button->text)) / 2 + 1;
	y += (c->height - gui_text_height (button->text, 0)) / 2 ;
	if (button->pressed) x++, y++;
	gr_text_setcolor (color_text);
	gr_setcolor (color_text);
	gui_text_put (dest, clip, x, y, button->text, 0, 0) ;

	/* Draw the focus border */
	if (c->focused)
	{
		gr_line (dest, clip, x, y+gr_text_height(0, button->text), 
				gui_text_width(button->text), 0 /*, 0x55555555*/);
	}
}

/** Call a button's callback function, if present
 *
 *	@param button	Pointer to the button
 **/

void gui_button_action (BUTTON * button)
{
	button->was_pressed = 1;
	button->control.actionflags |= ACTION_CHANGE;

	if (button->callback == NULL)
		return;

	switch (button->callbackparamcount)
	{
		case 0:
			(*button->callback)();
			break;
		case 1:
			(*(void (*)(int))button->callback)
				(button->callbackparam[0]);
			break;
		case 2:
			(*(void (*)(int,int))button->callback)
				(button->callbackparam[0], button->callbackparam[1]);
			break;
	}
}

/** Change the button's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:BUTTON class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the button pressed or released
 *  @param pressed	1 if the button was pressed, 0 if it was released
 * */

int gui_button_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	BUTTON * button = (BUTTON *)c;

	if (button->pressed != pressed)
	{
		c->redraw = 1;
		if (button->pressed && !pressed)
		{
			button->pressed = 0;
			gui_button_action (button);
		}
		else
			button->pressed = pressed;
	}
	return 1;
}

/** The mouse leaves a button, change its pressed state.
 *  This is a member function of the CONTROL:BUTTON class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 * */

int gui_button_mouseleave (CONTROL *c)
{
	BUTTON * button = (BUTTON *)c ;

	if (button->pressed)
	{
		button->pressed = 0;
		c->redraw = 1;
	}
	return 1;
}

/** The user moves the focus to the button
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @return			1 if the control is capable of focusing
 */

int gui_button_enter (CONTROL * c)
{
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 */

int gui_button_leave (CONTROL * c)
{
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the button.control member (at offset 0 of the button)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_button_key (CONTROL * c, int scancode, int character)
{
	BUTTON * button = (BUTTON *)c;

	if (scancode == KEY_SPACE || scancode == KEY_RETURN)
	{
		gui_button_action (button);
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

int gui_button_syskey (CONTROL * control, int key)
{
	BUTTON * button = (BUTTON *) control;

	const char * ptr = button->text;

	while (*ptr)
	{
		if (*ptr == '&')
		{
			if (!key || toupper(ptr[1]) == toupper(key))
			{
				if (key && button->callback)
					gui_button_action (button);
				return 1 ;
			}
		}
		ptr++;
	}
	return 0;
}

/** Initialize the data of a new button control 
 *
 *  @param button	Pointer to the button control
 *  @param text 	Label of the button
 **/

void gui_button_init (BUTTON * button, const char * text)
{
	gui_control_init (&button->control);

	/* Fill the control control struct data members */
	button->control.bytes = sizeof(BUTTON);
	button->control.width = 80;
	button->control.height = 22;
	button->control.innerdrag = 0;

	/* Fill the control control struct member functions */
	button->control.draw = gui_button_draw;
	button->control.mouseleave = gui_button_mouseleave;
	button->control.mousebutton = gui_button_mousebutton;
	button->control.enter = gui_button_enter;
	button->control.leave = gui_button_leave;
	button->control.key = gui_button_key;
	button->control.syskey = gui_button_syskey;

	/* Fill the rest of data members */
	strncpy (button->text, text, BUTTON_TEXT_MAX);
	button->text[BUTTON_TEXT_MAX-1] = 0;
	button->pressed = 0;
	button->callback = 0;
	button->was_pressed = 0;
}

/** Create a new button control 
 *
 *  @param text 	Label of the button
 *  @return 		Pointer to the control or 0 if not enough memory
 **/

CONTROL * gui_button_new (const char * text)
{
	BUTTON * button ;

	/* Alloc memory for the struct */
	button = (BUTTON *) malloc(sizeof(BUTTON));
	if (button == NULL)
		return NULL;
	gui_button_init (button, text);
	return &(button->control);
}

/** Set a callback function for a push button
 *
 *	@param control		Pointer to the button control
 *	@param callback		Pointer to the callback function or NULL to deactivate it
 **/

void gui_button_callback (CONTROL * control, void (*callback)())
{
	((BUTTON *)control)->callback = callback;
	((BUTTON *)control)->callbackparamcount = 0;
}

/** Set a callback function for a push button, with a parameter
 *
 *	@param control		Pointer to the button control
 *	@param callback		Pointer to the callback function or NULL to deactivate it
 **/

void gui_button_callbackp (CONTROL * control, void (*callback)(), void * p)
{
	((BUTTON *)control)->callback = callback;
	((BUTTON *)control)->callbackparam[0] = *(int *)&p;
	((BUTTON *)control)->callbackparamcount = 1;
}

/** Set a callback function for a push button, with a parameter
 *
 *	@param control		Pointer to the button control
 *	@param callback		Pointer to the callback function or NULL to deactivate it
 **/

void gui_button_callbacki (CONTROL * control, void (*callback)(), int p)
{
	((BUTTON *)control)->callback = callback;
	((BUTTON *)control)->callbackparam[0] = p;
	((BUTTON *)control)->callbackparamcount = 1;
}

/** Set a callback function for a push button, with two parameters
 *
 *	@param control		Pointer to the button control
 *	@param callback		Pointer to the callback function or NULL to deactivate it
 **/

void gui_button_callbackpi (CONTROL * control, void (*callback)(), void * p, int i)
{
	((BUTTON *)control)->callback = callback;
	((BUTTON *)control)->callbackparam[0] = (long)p;
	((BUTTON *)control)->callbackparam[1] = i;
	((BUTTON *)control)->callbackparamcount = 2;
}

/** Set the text of a button
 *
 *  @param control		Pointer to the button control
 *  @param text			New text to assign to the control
 **/

void gui_button_text (CONTROL * control, const char * text)
{
	BUTTON * button = (BUTTON *)control;
	
	if (text != NULL)
	{
		strncpy (button->text, text, BUTTON_TEXT_MAX);
		button->text[BUTTON_TEXT_MAX-1] = 0;
	}
}
