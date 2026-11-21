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
 *  Contains the basic CONTROL struct functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Draw the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or NULL if none
 **/

static void gui_control_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
}

/** Function called when the mouse enters the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 **/

static int gui_control_mouseenter (CONTROL * c)
{
	return 0;
}

/** Function called when the mouse is moving inside the control
 *  (or outside if the control supports inner dragging and a mouse button was pressed inside)
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param buttons	Flag with all mouse buttons state (bit 0 = 1 if left button pressed, etc)
 **/

static int gui_control_mousemove (CONTROL * c, int x, int y, int buttons)
{
	return 0;
}

/** Function called when a mouse button is pressed or released inside the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the button pressed or released
 *  @param pressed	1 if the button was pressed, 0 if it was released
 **/

static int gui_control_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	return 0;
}

/** Function called when the mouse leaves the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 **/

static int gui_control_mouseleave (CONTROL * c)
{
	return 0;
}

/** Function called when the user presses a key and the control has the keyboard focus
 *
 *  @param c			Pointer to the control member (at offset 0 of the struct)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

static int gui_control_key (CONTROL * c, int scancode, int key)
{
	return 0;
}

/** Function called when the user moves (or tries to move) the keyboard focus to the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @return			1 if the control is capable of focusing
 */

static int gui_control_enter (CONTROL * c)
{
	return 0;
}

/** Function called when the keyboard focus is lost 
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @return			1 if the control is capable of focusing (unused)
 */

static int gui_control_leave (CONTROL * c)
{
	c->focused = 0;
	return 0;
}

/** Function called to check if the control supports the given acces key (or any at all)
 *
 *  @param control	Pointer to the control
 *	@param key		Character pressed or 0 to check if any is present
 *  @return			1 if the key is a shortcut key, 0 otherwise
 **/

static int gui_control_syskey (CONTROL * c, int key)
{
	return 0;
}

/** Function called when the control is resized (usually by the user)
 *
 *  @param control	Pointer to the control
 */

static void gui_control_resized (CONTROL * c)
{
}

/** Function called to check what mouse cursor to use when the mouse is
 *  inside the control, at the given coordinates
 *
 *  @param control	Pointer to the control
 *  @param x		Left coordinate (local to the control)
 *	@param y		Top coordinate (local to the control)
 */

static int gui_control_mousepointer (CONTROL * c, int x, int y)
{
	return c->pointer;
}

/** Function called to destroy the control. You must call the parent's
 *  destructor just after you destroy your own control.
 *
 *  @param control	Pointer to the control
 */

void gui_control_destructor (CONTROL * c)
{
	if (c->tooltip)
		string_discard (c->tooltip);
	if (c->drstring)
		string_discard (c->drstring);
}

/** Initialize the components of a control struct.
 *  This is an internal function that is called by all class constructors
 *  to initialize the CONTROL member of the child class. All virtual functions
 *  are initialized to control member functions that do nothing.
 *  
 *  @param control	Pointer to the control base class to initialize
 */

void gui_control_init (CONTROL * control)
{
	/* Initialize data members */
	control->bytes = sizeof(CONTROL);
	control->x = 0;
	control->y = 0;
	control->width = 0;
	control->height = 0;
	control->redraw = 0;
	control->focused = 0;
	control->highlight = 0;
	control->innerdrag = 0;
	control->hidden = 0;
	control->window = NULL;
	control->hresizable = 0;
	control->vresizable = 0;
	control->max_height = -1;
	control->max_width = -1;
	control->min_height = 1;
	control->min_width = 1;
	control->width_step = 1;
	control->height_step = 1;
	control->pointer = 0;
	control->bgmode = BG_WINDOW;
	control->tooltip = 0;
	control->parent = 0;
	control->actionflags = 0;
	control->draggable = 0;
	control->drgraph = NULL;
	control->drstring = 0;
	control->allkeys = 0;

	/* Initialize virtual functions */
	control->draw = gui_control_draw;
	control->mouseenter = gui_control_mouseenter;
	control->mousemove = gui_control_mousemove;
	control->mousebutton = gui_control_mousebutton;
	control->mouseleave = gui_control_mouseleave;
	control->key = gui_control_key;
	control->enter = gui_control_enter;
	control->leave = gui_control_leave;
	control->syskey = gui_control_syskey;
	control->destructor = gui_control_destructor;
	control->resized = gui_control_resized;
	control->mousepointer = gui_control_mousepointer;
}

