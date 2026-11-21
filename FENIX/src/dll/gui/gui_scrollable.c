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
 *  Contains the basic SCROLLABLE struct functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Draw an scrollable area with an optional border.
 *  This is a member function of the CONTROL:SCROLLABLE class. It also
 *  recalculates scrollbar visibility.
 *
 *  @param c		Pointer to the control object
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 **/

static void gui_scrollable_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	int x0, y0, x1, y1;
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	int width = c->width;
	int height = c->height;
	REGION * inner_region;

	scrollable->scrollx = scrollable->hpos.pos;
	scrollable->scrolly = scrollable->vpos.pos;

	/* Set the initial coordinates */
	x0 = scrollable->scrollx;
	y0 = scrollable->scrolly;
	if (scrollable->autocenter)
	{
		scrollable->offsetx = 0;
		scrollable->offsety = 0;

		if (scrollable->areawidth < c->width + scrollable->border)
		{
			scrollable->offsetx = (c->width - scrollable->border - scrollable->areawidth)/2;
			x0 -= scrollable->offsetx;
		}
		if (scrollable->areaheight < c->height + scrollable->border)
		{
			scrollable->offsety = (c->height - scrollable->border - scrollable->areaheight)/2;
			y0 -= scrollable->offsety;
		}
	}
	x1 = x0+width-1-scrollable->titlex;
	y1 = y0+height-1-scrollable->titley;

	/* Draw the border, if any */
	if (scrollable->border)
	{
		int b = scrollable->border;
		while (b-- > 1)
		{
			gr_setcolor (color_highlight);
			gr_hline (dest, clip, x, y, width);
			gr_vline (dest, clip, x, y, height);
			gr_setcolor (color_shadow);
			gr_hline (dest, clip, x, y+height-1, width);
			gr_vline (dest, clip, x+width-1, y, height);
			x++;
			y++;
			x1-=2;
			y1-=2;
			width-=2;
			height-=2;
		}
		gr_setcolor (c->focused ? color_border : color_shadow);
		gr_hline (dest, clip, x, y, width);
		gr_vline (dest, clip, x, y, height);
		gr_setcolor (color_highlight);
		gr_hline (dest, clip, x, y+height-1, width);
		gr_vline (dest, clip, x+width-1, y, height);

		x++;
		y++;
		x1-=2;
		y1-=2;
		width-=2;
		height-=2;
	}

	/* gui_scrollable_resizearea calculates the scroll bar size */
	gui_scrollable_resizearea (c, scrollable->areawidth, scrollable->areaheight);
	
	if (scrollable->hscroll.control.hidden == 0)
		y1 -= scrollable->hscroll.control.height;
	if (scrollable->vscroll.control.hidden == 0)
		x1 -= scrollable->vscroll.control.width;

	/* Draw the scroll bars */
	if (scrollable->hscroll.control.hidden == 0)
	{
		scrollable->hscroll.control.x = scrollable->border;
		scrollable->hscroll.control.y = scrollable->border + scrollable->titley + (y1-y0+1);
		(*scrollable->hscroll.control.draw)(&scrollable->hscroll.control, 
			dest, x, y + scrollable->titley + (y1-y0+1), clip);
	}
	if (scrollable->vscroll.control.hidden == 0)
	{
		scrollable->vscroll.control.x = scrollable->border + scrollable->titlex + (x1-x0+1);
		scrollable->vscroll.control.y = scrollable->border ;
		(*scrollable->vscroll.control.draw)(&scrollable->vscroll.control, 
			dest, x + scrollable->titlex + (x1-x0+1), y, clip);
	}
	scrollable->scrollx = scrollable->hpos.pos;
	scrollable->scrolly = scrollable->vpos.pos;

	/* Limit the coordinates to the inner area limits */
	if (scrollable->limit_area)
	{
		if (x0 > scrollable->areawidth - 1)
			x0 = scrollable->areawidth - 1;
		if (y0 > scrollable->areaheight - 1)
			y0 = scrollable->areaheight - 1;
		if (x1 > scrollable->areawidth - 1)
			x1 = scrollable->areawidth - 1;
		if (y1 > scrollable->areaheight - 1)
			y1 = scrollable->areaheight - 1;
	}

	/* Draw the inner area */
	inner_region = region_new (x, y, 
		scrollable->titlex + x1-x0+1, scrollable->titley + y1-y0+1);
	if (clip) region_union (inner_region, clip);
	if (!region_is_empty(inner_region))
		(*scrollable->draw)(c, dest, x, y, x0, y0, x1, y1, inner_region);
	region_destroy (inner_region);
}

/** Responde to a mouse enter. Sets the focus to itself.
 *
 *	@param c			Pointer to the SCROLLABLE control
 *	@return				1 if the event is processed, 0 if it is ignored
 **/

static int gui_scrollable_mouseenter (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	scrollable->focusbar = c;
	c->redraw = 1;
	return (*scrollable->mouseenter)(c);
}

static int gui_scrollable_mousepointer (CONTROL * c, int x, int y)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	SCROLLBAR * bar;

	if (c->window && c->window->captured_mouse == c)
		scrollable->innerdrag = 1;
	if (!scrollable->innerdrag)
	{
		for (bar = &scrollable->hscroll ; bar <= &scrollable->vscroll ; bar++)
		{
			CONTROL * barc = &bar->control;

			if (barc->hidden == 0 && barc->x <= x && barc->y <= y &&
				barc->x + barc->width >= x && barc->y + barc->height >= y)
				return (*barc->mousepointer)(barc, x, y);
		}
	}

	if (x <= 0 || x > scrollable->titlex)
		x += scrollable->hpos.pos;
	if (y <= 0 || y > scrollable->titley)
		y += scrollable->vpos.pos;
	x -= scrollable->offsetx;
	y -= scrollable->offsety;
	return (*scrollable->mousepointer)(c, x, y);
}

/** Response to a mouse move. Enter or leaves an scrollbar
 *  and propagates its event. If the mouse is not over an
 *  scrollbar, propagate the event to the child class.
 *
 *	@param c			Pointer to the SCROLLABLE control
 *	@param x			Local X coordinate of the mouse
 *	@param y			Local Y coordinate of the mouse
 *	@param buttons		Bit mask with the state of each mouse button
 *	@return				1 if the event is processed, 0 if it is ignored
 **/

static int gui_scrollable_mousemove (CONTROL * c, int x, int y, int buttons)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	SCROLLBAR * bar ;

	/* Dragging a scroll bar? */
	if (scrollable->dragbar)
	{
		CONTROL * barc = &scrollable->dragbar->control;
		(*barc->mousemove)(barc, x - barc->x, y - barc->y, buttons);
		scrollable->scrollx = scrollable->hpos.pos;
		scrollable->scrolly = scrollable->vpos.pos;
		c->redraw = 1;
		return 1;
	}

	/* If any scroll bar is visible, propagate the event */
	if (c->window && c->window->captured_mouse == c)
		scrollable->innerdrag = 1;
	if (!scrollable->innerdrag)
	{
		for (bar = &scrollable->hscroll ; bar <= &scrollable->vscroll ; bar++)
		{
			CONTROL * barc = &bar->control;

			if (barc->hidden == 0 && barc->x <= x && barc->y <= y &&
				barc->x + barc->width >= x && barc->y + barc->height >= y)
			{
				/* Propagate mouseenter/mouseleave events if necessary */
				if (scrollable->focusbar != barc)
				{
					if (scrollable->focusbar == c)
					{
						(*scrollable->mouseleave)(&scrollable->control);
						scrollable->control.highlight = 0;
					}
					else if (scrollable->focusbar)
					{
						(*scrollable->focusbar->mouseleave)(scrollable->focusbar);
						scrollable->focusbar->highlight = 0;
					}
					(*bar->control.mouseenter)(&bar->control);
					bar->control.highlight = 1;
				}
				scrollable->focusbar = barc;

				(*barc->mousemove)(barc, x - barc->x, y - barc->y, buttons);
				scrollable->scrollx = scrollable->hpos.pos;
				scrollable->scrolly = scrollable->vpos.pos;
				return 1;
			}
		}
	}

	/* Propagate mouseleave */
	if (scrollable->focusbar != NULL && scrollable->focusbar != c)
	{
		(*scrollable->focusbar->mouseleave)(scrollable->focusbar);
		scrollable->focusbar->highlight = 0;
		scrollable->focusbar = c;
		(*scrollable->mouseenter)(c);
		scrollable->control.highlight = 1;
	}

	if (x <= 0 || x > scrollable->titlex)
		x += scrollable->hpos.pos;
	if (y <= 0 || y > scrollable->titley)
		y += scrollable->vpos.pos;
	x -= scrollable->offsetx;
	y -= scrollable->offsety;
	return (*scrollable->mousemove)(c, x, y, buttons);
}

/** Response to a mouse button action. Propagates the event to an
 *	scroll bar if the user is dragging it or over it,
 *	or to the child class provided function.
 *
 *	@param c			Pointer to the SCROLLABLE control
 *	@param x			Local X coordinate of the mouse
 *	@param b			Index of the button
 *	@param pressed		0 = Released; 1 = Pressed; 2+ = Double click
 *	@return				1 if the event is processed, 0 if it is ignored
 **/

static int gui_scrollable_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	SCROLLBAR * bar ;

	/* Disable inner dragging (it needs to be enabled again by the internal control) */
	if (c->draggable)
		c->draggable = 2;

	scrollable->innerdrag = 0;

	/* Dragging a scroll bar? */
	if (scrollable->dragbar)
	{
		CONTROL * barc = &scrollable->dragbar->control;
		(*barc->mousebutton)(barc, x - barc->x, y - barc->y, b, pressed);
		scrollable->scrollx = scrollable->hpos.pos;
		scrollable->scrolly = scrollable->vpos.pos;
		if (!pressed) scrollable->dragbar = NULL;
		return 1;
	}

	/* If any scroll bar is visible, propagate the event */
	for (bar = &scrollable->hscroll ; bar <= &scrollable->vscroll ; bar++)
	{
		CONTROL * barc = &bar->control;

		if (barc->hidden == 0 && barc->x <= x && barc->y <= y &&
			barc->x + barc->width >= x && barc->y + barc->height >= y)
		{
			(*barc->mousebutton)(barc, x - barc->x, y - barc->y, b, pressed);
			scrollable->scrollx = scrollable->hpos.pos;
			scrollable->scrolly = scrollable->vpos.pos;
			scrollable->dragbar = (b == 0 && pressed ? bar : 0);
			return 1;
		}
	}

	if (pressed)
		scrollable->innerdrag = 1;

	if (x <= 0 || x >= scrollable->titlex)
		x += scrollable->hpos.pos;
	if (y <= 0 || y >= scrollable->titley)
		y += scrollable->vpos.pos;
	x -= scrollable->offsetx;
	y -= scrollable->offsety;

	return (*scrollable->mousebutton)(c, x, y, b, pressed);
}

static int gui_scrollable_mouseleave (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;

	if (scrollable->focusbar)
	{
		scrollable->focusbar->highlight = 0;
		scrollable->focusbar = 0;
		c->redraw = 1;
	}
	return (*scrollable->mouseleave)(c);
}

static int gui_scrollable_key (CONTROL * c, int scancode, int key)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	return (*scrollable->key)(c, scancode, key);
}

static int gui_scrollable_enter (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	if ((*scrollable->enter)(c))
	{
		c->focused = 1;
		c->redraw = 1;
		return 1;
	}
	return 0;
}

static int gui_scrollable_leave (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	(*scrollable->leave)(c);
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

static int gui_scrollable_syskey (CONTROL * c, int key)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	return (*scrollable->syskey)(c, key);
}

static void gui_scrollable_destructor (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	(*scrollable->destructor)(c);
}

/* ---- Stub functions ---- */

static void gui_stub_draw (CONTROL * c, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	int width = x1-x0+1;
	int height = y1-y0+1;
	char buffer[64];

	sprintf (buffer, "(%d,%d)-(%d,%d)", x0, y0, x1, y1);
	gr_text_put (dest, clip, 0, x+width/2, y+height/2, buffer);
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
	return 1;
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
static void gui_stub_resized (CONTROL * c)
{
}
static int gui_stub_vresize (CONTROL * c, int newheight)
{
	return newheight;
}
static int gui_stub_mousepointer (CONTROL * c, int x, int y)
{
	return 0;
}

/** Act after a control resize operation
 *
 *	@param c			Scrollable control object
 */

void gui_scrollable_resized (CONTROL * c)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;

	(*scrollable->resized)(c);
	gui_scrollable_resizearea (c, scrollable->areawidth, scrollable->areaheight);
}

/** Initialize the components of a scrollable struct.
 *  This is an internal function that is called by all class constructors
 *  to initialize the SCROLLABLE member of the child class. All virtual functions
 *  are initialized to scrollable member functions that do nothing.
 *  
 *  @param scrollable	Pointer to the scrollable control class to initialize
 *  @param width		Width in pixels of the scrollable control
 *  @param height		Height in pixels of the scrollable control
 */

void gui_scrollable_init (SCROLLABLE * scrollable, int width, int height)
{
	gui_control_init (&scrollable->control);

	/* Initialize parent data members */
	scrollable->control.bytes = sizeof(SCROLLABLE);
	scrollable->control.width = width;
	scrollable->control.height = height;
	scrollable->control.innerdrag = 1;
	scrollable->maxwidth = 0;
	scrollable->maxheight = 0;
	scrollable->showbars = 1;

	/* Initialize data members */
	scrollable->areaheight = 0;
	scrollable->areawidth = 0;
	scrollable->border = 1;
	scrollable->scrollx = 0;
	scrollable->scrolly = 0;
	scrollable->dragbar = 0;
	scrollable->focusbar = 0;
	scrollable->hpos.step = 1;
	scrollable->hpos.pos = 0;
	scrollable->vpos.pos = 0;
	scrollable->vpos.step = 1;
	scrollable->titlex = 0;
	scrollable->titley = 0;
	scrollable->limit_area = 0;
	scrollable->autocenter = 0;
	scrollable->offsetx = 0;
	scrollable->offsety = 0;
	scrollable->innerdrag = 0;

	/* Initialize parent virtual functions */
	scrollable->control.draw = gui_scrollable_draw;
	scrollable->control.mouseenter = gui_scrollable_mouseenter;
	scrollable->control.mousemove = gui_scrollable_mousemove;
	scrollable->control.mousebutton = gui_scrollable_mousebutton;
	scrollable->control.mouseleave = gui_scrollable_mouseleave;
	scrollable->control.key = gui_scrollable_key;
	scrollable->control.enter = gui_scrollable_enter;
	scrollable->control.leave = gui_scrollable_leave;
	scrollable->control.syskey = gui_scrollable_syskey;
	scrollable->control.resized = gui_scrollable_resized;
	scrollable->control.destructor = gui_scrollable_destructor;
	scrollable->control.mousepointer = gui_scrollable_mousepointer;

	/* Initialize stub virtual funcions */
	scrollable->draw = gui_stub_draw;
	scrollable->mouseenter = gui_stub_mouseenter;
	scrollable->mousemove = gui_stub_mousemove;
	scrollable->mousebutton = gui_stub_mousebutton;
	scrollable->mouseleave = gui_stub_mouseleave;
	scrollable->key = gui_stub_key;
	scrollable->enter = gui_stub_enter;
	scrollable->leave = gui_stub_leave;
	scrollable->syskey = gui_stub_syskey;
	scrollable->resized = gui_stub_resized;
	scrollable->destructor = gui_stub_destructor;
	scrollable->mousepointer = gui_stub_mousepointer;

	/* Initialize member objects */
	gui_hscrollbar_init (&scrollable->hscroll, width-2,  &scrollable->hpos);
	gui_vscrollbar_init (&scrollable->vscroll, height-2, &scrollable->vpos);

	scrollable->control.min_height = 56;
	scrollable->control.min_width = 56;
}

CONTROL * gui_scrollable_new (int width, int height)
{
	SCROLLABLE * scrollable ;

	/* Alloc memory for the struct */
	scrollable = (SCROLLABLE *) malloc(sizeof(SCROLLABLE));
	if (scrollable == NULL)
		return NULL;

	gui_scrollable_init (scrollable, width, height);
	return &scrollable->control;
}

void gui_scrollable_resizearea (CONTROL * c, int areawidth, int areaheight)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;
	int x0, x1, y0, y1;
	int width = c->width;
	int height = c->height;

	scrollable->areawidth = areawidth;
	scrollable->areaheight = areaheight;

	/* Set the initial visible area coordinates */
	x0 = scrollable->scrollx;
	y0 = scrollable->scrolly;
	x1 = x0+width-1-scrollable->titlex;
	y1 = y0+height-1-scrollable->titley;

	/* If there is a border, reduce the visible area */
	if (scrollable->border)
	{
		x1-=2;
		y1-=2;
		width-=2;
		height-=2;
	}

	/* Check if any or both of the scroll bars should be visible */
	scrollable->hscroll.control.hidden = 1;
	scrollable->vscroll.control.hidden = 1;

	if (scrollable->showbars)
	{
		if (scrollable->areawidth > x1-x0+1)
		{
			y1 -= scrollable->hscroll.control.height;
			scrollable->hscroll.control.hidden = 0;
		}
		if (scrollable->areaheight > y1-y0+1)
		{
			x1 -= scrollable->vscroll.control.width;
			scrollable->vscroll.control.hidden = 0;
		}
		if (scrollable->areawidth > x1-x0+1 && scrollable->hscroll.control.hidden == 1)
		{
			y1 -= scrollable->hscroll.control.height;
			scrollable->hscroll.control.hidden = 0;
		}
	}

	/* Set the size of the scroll bars, if visible */
	if (scrollable->vscroll.control.hidden == 1 &&
	    scrollable->hscroll.control.hidden == 0)
	{
		scrollable->hscroll.control.width = width;
	}
	if (scrollable->vscroll.control.hidden == 0 &&
	    scrollable->hscroll.control.hidden == 1)
	{
		scrollable->vscroll.control.height = height;
	}
	if (scrollable->vscroll.control.hidden == 0 &&
	    scrollable->hscroll.control.hidden == 0)
	{
		scrollable->hscroll.control.width  = width  - scrollable->vscroll.control.width;
		scrollable->vscroll.control.height = height - scrollable->hscroll.control.height;
	}

	/* Set the scroll position values */
	scrollable->hpos.min = 0;
	scrollable->hpos.max = scrollable->areawidth - (x1-x0+1);
	scrollable->hpos.pos = scrollable->scrollx;
	scrollable->hpos.pagestep = x1-x0+1;
	scrollable->vpos.min = 0;
	scrollable->vpos.max = scrollable->areaheight - (y1-y0+1);
	scrollable->vpos.pos = scrollable->scrolly;
	scrollable->vpos.pagestep = y1-y0+1;

	/* Adjust the resulting position between limits */
	if (scrollable->vpos.pos > scrollable->vpos.max)
		scrollable->vpos.pos = scrollable->vpos.max;
	if (scrollable->hpos.pos > scrollable->hpos.max)
		scrollable->hpos.pos = scrollable->hpos.max;
	if (scrollable->vpos.pos < scrollable->vpos.min)
		scrollable->vpos.pos = scrollable->vpos.min;
	if (scrollable->hpos.pos < scrollable->hpos.min)
		scrollable->hpos.pos = scrollable->hpos.min;
}	

void gui_scrollable_scrollto (CONTROL * c, int x, int y)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)c;

	if (x > scrollable->hpos.max)
		x = scrollable->hpos.max;
	if (x < scrollable->hpos.min)
		x = scrollable->hpos.min;
	if (y > scrollable->vpos.max)
	    y = scrollable->vpos.max;
	if (y < scrollable->vpos.min)
	    y = scrollable->vpos.min;

	scrollable->scrollx = x;
	scrollable->scrolly = y;
	scrollable->hpos.pos = x;
	scrollable->vpos.pos = y;
}

/** Set the maximum size of a scrollable control
 *
 *  @param control	Pointer to the scrollable object
 *	@param neww		Maximum width,  -1 = unlimited
 *	@param newh		Maximum height, -1 = unlimited
 */

void gui_scrollable_maxsize (CONTROL * control, int neww, int newh)
{
	SCROLLABLE * scrollable = (SCROLLABLE *)control;
	scrollable->maxheight = newh;
	scrollable->maxwidth  = neww;
}
