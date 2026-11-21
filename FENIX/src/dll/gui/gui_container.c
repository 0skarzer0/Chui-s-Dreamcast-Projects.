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
 *  Contains the basic CONTAINER struct functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Add a new control to the container
 *
 *	@param container		Pointer to the container object
 *	@param x				X internal coordinate of the control
 *	@param y				Y internal coordinate of the control
 *	@param control			Pointer to the control to add
 **/

void gui_container_add (CONTROL * c, int x, int y, CONTROL * add)
{
	CONTAINER * container = (CONTAINER *)c ;

	if (container->allocated == container->count)
	{
		CONTROL * * new_contents = (CONTROL * * ) realloc (container->contents,
			sizeof(CONTROL *) * (container->allocated + 16));
		if (new_contents == NULL)
			return;

		container->contents = new_contents;
		container->allocated += 16;
	}

	if (container->control.width  < x+add->width)
	    container->control.width  = x+add->width;
	if (container->control.height < y+add->height)
	    container->control.height = y+add->height;

	add->x = x;
	add->y = y;
	add->window = container->control.window;

	container->contents[container->count++] = add;
}

/** Remove a control from the container (does NOT destroy it)
 *
 *	@param container		Pointer to the container object
 *	@param control			Pointer to the object to remove
 **/

void gui_container_remove (CONTROL * c, CONTROL * control)
{
	CONTAINER * container = (CONTAINER *)c ;
	int i;

	for (i = 0 ; i < container->count ; i++)
	{
		if (container->contents[i] == control)
		{
			if (i < container->count-1)
				memmove (&container->contents[i], &container->contents[i+1],
					sizeof(CONTROL * *) * (container->count - i - 1));
			container->count--;
			return;
		}
	}
}

/** Change the position of a control inside the container
 *
 *	@param container		Pointer to the container object
 *	@param x				New x coordinate of the object
 *	@param y				New y coordinate of the object
 *	@param control			Pointer to the contained control
 **/

void gui_container_move (CONTROL * c, int x, int y, CONTROL * control)
{
	CONTAINER * container = (CONTAINER *)c ;
	int i;

	for (i = 0 ; i < container->count ; i++)
	{
		if (container->contents[i] == control)
		{
			control->x = x;
			control->y = y;
			return;
		}
	}
}

void gui_container_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	CONTAINER * container = (CONTAINER *)c;
	int i;
	REGION region;

	/* Create a better clipping region */
	region.x = x;
	region.y = y;
	region.x2 = x + c->width-1;
	region.y2 = y + c->height-1;
	if (clip)
		region_union (&region, clip);
	else
	{
		if (region.x < 0)
			region.x = 0;
		if (region.y < 0)
			region.y = 0;
		if (region.x2 >= scr_width)
			region.x2 = scr_width-1;
		if (region.y2 >= scr_height)
			region.y2 = scr_height-1;
	}

	for (i = 0 ; i < container->count ; i++)
	{
		container->contents[i]->window = container->control.window;

		if (container->contents[i]->hidden == 0)
			(*container->contents[i]->draw)(container->contents[i], dest, 
				x + container->contents[i]->x,
				y + container->contents[i]->y, &region);
	}
}

/** Function called when the mouse enters the container
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 */

int gui_container_mouseenter (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;

	if (container->mouse)
	{
		container->mouse->highlight = 1;
		c->redraw = 1;
		return (*container->mouse->mouseenter)(container->mouse);
	}
	return (*container->mouseenter)(c);
}

/** Function called to assign a mouse pointer to a container
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 *  @param x			X Coordinate
 *  @param y			Y Coordinate
 */

int gui_container_mousepointer (CONTROL * control, int x, int y)
{
	CONTAINER * container = (CONTAINER *)control;
	int i;

	for (i = 0 ; i < container->count ; i++)
	{
		CONTROL * c = container->contents[i];

		if (c->hidden == 0 && c->x <= x && c->y <= y &&
			c->x + c->width > x && c->y + c->height > y)
			return (*c->mousepointer)(c, x - c->x, y - c->y);
	}
	return POINTER_ARROW;
}

/** Function called when the mouse moves inside (or dragging around) a container
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 *  @param x			X Coordinate
 *  @param y			Y Coordinate
 *	@param buttons		Mouse button state (1 bit for each button)
 */

int gui_container_mousemove (CONTROL * control, int x, int y, int buttons)
{
	CONTAINER * container = (CONTAINER *)control;
	int			i;

	/* Dragging a control? */
	if (container->drag)
	{
		(*container->drag->mousemove)(container->drag, 
			x - container->drag->x, y - container->drag->y, buttons);
		return 1;
	}

	/* Propagate the event */
	for (i = 0 ; i < container->count ; i++)
	{
		CONTROL * c = container->contents[i];

		if (c->hidden == 0 && c->x <= x && c->y <= y &&
			c->x + c->width > x && c->y + c->height > y)
		{
			/* Propagate mouseenter/mouseleave events if necessary */
			if (container->mouse != c)
			{
				if (container->mouse)
				{
					(*container->mouse->leave)(container->mouse);
					(*container->mouse->mouseleave)(container->mouse);
					container->mouse->highlight = 0;
					container->mouse->focused = 0;
					container->mouse = 0;
				}
				(*c->mouseenter)(c);
				if (c->tooltip)
					gui_desktop_tooltip (string_get(c->tooltip));
				else
					gui_desktop_tooltip (0);
				c->highlight = 1;
				c->focused = 1;
				c->redraw = 1;
				(*c->enter)(c);
				container->mouse = c;
			}

			/* Propagate the mousemove event */
			(*c->mousemove)(c, x - c->x, y - c->y, buttons);
			return 1;
		}
	}

	/* Mouse is not inside a control: propagate a mouseleave event */
	if (container->mouse)
	{
		(*container->mouse->leave)(container->mouse);
		(*container->mouse->mouseleave)(container->mouse);
		container->mouse->highlight = 0;
		container->mouse->focused = 0;
		container->mouse = NULL;
		control->redraw = 1;
	}

	return (*container->mousemove)(control, x, y, buttons);
}

/** Function called when a mouse button is pressed or released in a container
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 *  @param x			X Coordinate
 *  @param y			Y Coordinate
 *	@param b			Mouse button (0 left, 1 right, 2 middle, 3 wheel up, 4 wheel down)
 *	@param pressed		1 if mouse button was pressed, 0 if released
 */

int gui_container_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	CONTAINER * container = (CONTAINER *)c;
	int			i;

	gui_desktop_tooltip(0);

	/* Disable inner dragging (it needs to be enabled again by the internal control) */
	if (c->draggable)
		c->draggable = 2;

	/* Dragging a control? */
	if (container->drag)
	{
		(*container->drag->mousebutton)(container->drag, 
			x - container->drag->x, y - container->drag->y, b, pressed);
		c->redraw |= container->drag->redraw;
		if (!pressed) container->drag = NULL;
		return 1;
	}

	/* Propagate the event */
	for (i = 0 ; i < container->count ; i++)
	{
		CONTROL * s = container->contents[i];

		if (s->hidden == 0 && s->x <= x && s->y <= y &&
			s->x + s->width > x && s->y + s->height > y)
		{
			container->drag = (b == 0 && pressed && s->innerdrag ? s : 0);
			(*s->mousebutton)(s, x - s->x, y - s->y, b, pressed);
			c->redraw |= s->redraw;
			return 1;
		}
	}

	return (*container->mousebutton)(c, x, y, b, pressed);
}

int gui_container_mouseleave (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;

	gui_desktop_tooltip(0);
	if (container->mouse)
	{
		int r = (*container->mouse->mouseleave)(container->mouse);
		(*container->mouse->leave)(container->mouse);
		container->mouse->highlight = 0;
		container->mouse->focused = 0;
		container->mouse = 0;
		c->redraw = 1;
		return r;
	}
	return (*container->mouseleave)(c);
}

int gui_container_key (CONTROL * c, int scancode, int key)
{
	CONTAINER * container = (CONTAINER *)c;
	if (container->mouse)
		return (*container->mouse->key)(container->mouse, scancode, key);
	return (*container->key)(c, scancode, key);
}

int gui_container_enter (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;

	(*container->enter)(c);
	if (container->mouse)
	{
		(*container->mouse->leave)(container->mouse);
		container->mouse->focused = 0;
		container->mouse->highlight = 0;
		container->mouse = 0;
	}
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

int gui_container_leave (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;
	(*container->leave)(c);
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

int gui_container_syskey (CONTROL * c, int key)
{
	CONTAINER * container = (CONTAINER *)c;
	int i;

	i = (*container->syskey)(c, key);
	if (i) return i;

	for (i = 0 ; i < container->count ; i++)
	{
		if (!container->contents[i]->hidden &&
			(*container->contents[i]->syskey)(container->contents[i], key))
		{
			return 1;
		}
	}
	return 0;
}

void gui_container_destructor (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;

	if (container->contents != NULL)
		free (container->contents);
	(*container->destructor)(c);
	gui_control_destructor(c);
}

/* ---- Stub functions ---- */

static void gui_stub_draw (CONTROL * c, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
}
static int gui_stub_mouseenter (CONTROL * c)
{
	return 0;
}
static int gui_stub_mousemove (CONTROL * c, int x, int y, int buttons)
{
	return 0;
}
static int gui_stub_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	return 0;
}
static int gui_stub_mouseleave (CONTROL * c)
{
	return 0;
}
static int gui_stub_key (CONTROL * c, int scancode, int key)
{
	return 0;
}
static int gui_stub_enter (CONTROL * c)
{
	return 0;
}
static int gui_stub_leave (CONTROL * c)
{
	return 0;
}
static int gui_stub_syskey (CONTROL * c, int key)
{
	return 0;
}
static void gui_stub_destructor (CONTROL * c)
{
}

/** Initialize the components of a container struct.
 *  This is an internal function that is called by all class constructors
 *  to initialize the CONTAINER member of the child class. All virtual functions
 *  are initialized to container member functions that do nothing.
 *  
 *  @param container	Pointer to the container base class to initialize
 *  @param width		Width in pixels of the container control
 *  @param height		Height in pixels of the container control
 */

void gui_container_init (CONTAINER * container, int width, int height)
{
	gui_control_init (&container->control);

	/* Initialize parent data members */
	container->control.bytes = sizeof(CONTAINER);
	container->control.width = width;
	container->control.height = height;
	container->control.innerdrag = 1;

	/* Initialize data members */
	container->count = 0;
	container->allocated = 0;
	container->contents = 0;
	container->drag = NULL;
	container->mouse = NULL;

	/* Initialize parent virtual functions */
	container->control.draw = gui_container_draw;
	container->control.mouseenter = gui_container_mouseenter;
	container->control.mousemove = gui_container_mousemove;
	container->control.mousebutton = gui_container_mousebutton;
	container->control.mouseleave = gui_container_mouseleave;
	container->control.key = gui_container_key;
	container->control.enter = gui_container_enter;
	container->control.leave = gui_container_leave;
	container->control.syskey = gui_container_syskey;
	container->control.destructor = gui_container_destructor;
	container->control.mousepointer = gui_container_mousepointer;

	/* Initialize stub virtual funcions */
	container->draw = gui_stub_draw;
	container->mouseenter = gui_stub_mouseenter;
	container->mousemove = gui_stub_mousemove;
	container->mousebutton = gui_stub_mousebutton;
	container->mouseleave = gui_stub_mouseleave;
	container->key = gui_stub_key;
	container->enter = gui_stub_enter;
	container->leave = gui_stub_leave;
	container->syskey = gui_stub_syskey;
	container->destructor = gui_stub_destructor;
}

CONTROL * gui_container_new (int width, int height)
{
	CONTAINER * container ;

	/* Alloc memory for the struct */
	container = (CONTAINER *) malloc(sizeof(CONTAINER));
	if (container == NULL)
		return NULL;

	gui_container_init (container, width, height);
	return &container->control;
}


