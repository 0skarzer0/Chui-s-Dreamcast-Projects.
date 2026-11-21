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
 *  Contains the CONTROL:SCROLLBAR class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"
#define SCROLLBAR_WIDTH		12
#define SCROLLBAR_HEIGHT	12

#define DECORATION_LEFT		1
#define DECORATION_RIGHT	2
#define DECORATION_UP  		3
#define DECORATION_DOWN 	4
#define DECORATION_THUMB	5

/** Help function to draw a scrollbar button
 *
 *	@param dest			Destination graphic
 *	@param clip			Destination clipping region
 *	@param pressed		1 to draw a pressed button, 0 to draw a raised one
 *	@param x			X coordinate
 *	@param y			Y coordinate
 *	@param width		Width of the button
 *	@param height		Height of the button
 *	@param decoration	One of the DECORATION_X constants, or 0
 *  @param highlight	1 to highlight the button
 *  @param orientation  0 if the scrollbar is horizontal, 1 if vertical
 **/

static void drawbutton (GRAPH * dest, REGION * clip, int pressed,
		int x, int y, int width, int height, int decoration, 
		int highlight, int fixed, int orientation)
{
	if (pressed == 2 && !fixed)
	{
		gr_setcolor (highlight ? color_face: color_window);
		gr_box (dest, clip, x, y, width, height);
		gr_setcolor (color_shadow);
		gr_hline (dest, clip, x, y, width);
		gr_vline (dest, clip, x, y, height);
		gr_setcolor (color_face);
		gr_hline (dest, clip, x, y+height-1, width);
		gr_vline (dest, clip, x+width-1, y, height);
	}
	else if (pressed == 2 && fixed)
	{
		if (width > height)
		{
			gr_setcolor (color_highlight);
			gr_hline (dest, clip, x, y+height/2, width);
			gr_setcolor (color_shadow);
			gr_hline (dest, clip, x, y+height/2-1, width);
		}
		else
		{
			gr_setcolor (color_highlight);
			gr_vline (dest, clip, x+width/2, y, height);
			gr_setcolor (color_shadow);
			gr_vline (dest, clip, x+width/2-1, y, height);
		}
	}
	else if (!fixed || decoration != DECORATION_THUMB)
	{
		gr_setcolor (color_border) ;
		gr_hline (dest, clip, x, y+height-1, width);
		gr_vline (dest, clip, x+width-1, y, height);
		gr_setcolor (highlight ? color_highlightface : color_face);
		gr_box (dest, clip, x+1, y+1, width-2, height-2);
		gr_setcolor (!pressed ? color_highlight : color_shadow);
		gr_hline (dest, clip, x, y, width-1);
		gr_vline (dest, clip, x, y, height-1);
		gr_setcolor (!pressed ? color_shadow : color_face);
		gr_hline (dest, clip, x+1, y+height-2, width-2);
		gr_vline (dest, clip, x+width-2, y+1, height-2);
	}

	if (!pressed) x--, y--;

	if (decoration == DECORATION_UP)
	{
		x += width/2;
		y += height/2;
		gr_setcolor (color_border);
		gr_line (dest, clip, x-2, y+2,  2, -4);
		gr_line (dest, clip, x  , y-2,  2,  4);
		gr_line (dest, clip, x+2, y+2, -4,  0);
		gr_line (dest, clip, x-1, y+1,  1, -2);
		gr_line (dest, clip, x+1, y+1, -1, -2);
		gr_line (dest, clip, x  , y+1,  0, -2);
	}
	if (decoration == DECORATION_LEFT)
	{
		x += width/2;
		y += height/2;
		gr_setcolor (color_shadow);
		gr_setcolor (color_border);
		gr_line (dest, clip, x+2, y-2, -4,  2);
		gr_line (dest, clip, x+1, y-1, -2,  1);
		gr_line (dest, clip, x+1, y+1, -2, -1);
		gr_line (dest, clip, x+1, y  , -2,  0);
		gr_line (dest, clip, x-2, y  ,  4,  2);
		gr_line (dest, clip, x+2, y+2,  0, -4);
	}
	if (decoration == DECORATION_DOWN)
	{
		x += width/2;
		y += height/2;
		gr_setcolor (color_border);
		gr_line (dest, clip, x-2, y-2,  2,  4);
		gr_line (dest, clip, x-1, y-1,  1,  2);
		gr_line (dest, clip, x+1, y-1, -1,  2);
		gr_line (dest, clip, x  , y-1,  0,  2);
		gr_line (dest, clip, x  , y+2,  2, -4);
		gr_line (dest, clip, x+2, y-2, -4,  0);
	}
	if (decoration == DECORATION_RIGHT)
	{
		x += width/2;
		y += height/2;
		gr_setcolor (color_shadow);
		gr_setcolor (color_border);
		gr_line (dest, clip, x-2, y-2,  4,  2);
		gr_line (dest, clip, x-1, y-1,  2,  1);
		gr_line (dest, clip, x-1, y+1,  2, -1);
		gr_line (dest, clip, x-1, y  ,  2,  0);
		gr_line (dest, clip, x+2, y  , -4,  2);
		gr_line (dest, clip, x-2, y+2,  0, -4);
	}
	if (decoration == DECORATION_THUMB)
	{
		if (fixed && orientation)
		{
			if (!pressed) x++, y++;
			gr_setcolor (color_window);
			gr_box (dest, clip, x+1, y+1, width-2, height-2);
			gr_setcolor (color_highlight);
			gr_hline (dest, clip, x, y, width);
			gr_vline (dest, clip, x, y, height/2);
			gr_line (dest, clip, x+1, y+height/2, width/2, height/2);
			gr_setcolor (color_shadow);
			gr_line (dest, clip, x, y+height/2, width/2, height/2);
			gr_line (dest, clip, x+width/2-1, y+height-1, width/2, -height/2);
			gr_vline (dest, clip, x+width-2, y, height/2);
			gr_setcolor (color_border);
			gr_line (dest, clip, x+width/2, y+height-1, width/2, -height/2);
			gr_vline (dest, clip, x+width-1, y, height/2+1);
			drawbutton (dest, clip, 0, x+width/2-2, y+height/3-1, 3, 3, 0, highlight, fixed, width > height);
		}
		else if (fixed)
		{
			if (!pressed) x++, y++;
			gr_setcolor (color_window);
			gr_box (dest, clip, x+1, y+1, width-2, height-2);
			gr_setcolor (color_highlight);
			gr_hline (dest, clip, x+width/2, y, width/2);
			gr_line (dest, clip, x, y+height/2, width/2, -height/2);
			gr_setcolor (color_shadow);
			gr_line (dest, clip, x, y+height/2, width/2, height/2);
			gr_hline (dest, clip, x+width/2, y+height-2, width/2-1);
			gr_vline (dest, clip, x+width-2, y, height);
			gr_setcolor (color_border);
			gr_vline (dest, clip, x+width-1, y, height);
			gr_hline (dest, clip, x+width/2, y+height-1, width/2-1);
			drawbutton (dest, clip, 0, x+width/2-1, y+height/2-1, 3, 3, 0, highlight, fixed, width > height);
		}
		else
			drawbutton (dest, clip, 0, x+width/2-1, y+height/2-1, 3, 3, 0, highlight, fixed, width > height);
	}
}

/** Draw a scrollbar.
 *  This is a member function of the CONTROL:SCROLLBAR class.
 *  It also updates scroll positions.
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_scrollbar_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	SCROLLBAR * scrollbar   = (SCROLLBAR *)c ;
	SCROLLPOS * pos         = scrollbar->scroll;
	int			thumbsize;

	struct _scrollbarelement * e = scrollbar->element;

	/* Update the scroll position if holding the left/right buttons */
	if (scrollbar->pressed == 0 && scrollbar->pressedtime < gr_timer()-50)
	{
		pos->pos -= pos->step;
		if (pos->pos < pos->min)
			pos->pos = pos->min;
		scrollbar->pressedtime = gr_timer();
	}
	if (scrollbar->pressed == 1 && scrollbar->pressedtime < gr_timer()-50)
	{
		pos->pos += pos->step;
		if (pos->pos > pos->max)
			pos->pos = pos->max;
		scrollbar->pressedtime = gr_timer();
	}
	if (scrollbar->pressed == 2 && scrollbar->pressedtime < gr_timer()-100)
	{
		if ((c->width >  c->height && scrollbar->last_mousex < e[3].x) ||
			(c->width <= c->height && scrollbar->last_mousey < e[3].y))
		{
			if (scrollbar->pressedsection == 0) scrollbar->pressed = -1;
			else scrollbar->scroll->pos -= scrollbar->scroll->pagestep;
			if (scrollbar->scroll->pos < scrollbar->scroll->min)
				scrollbar->scroll->pos = scrollbar->scroll->min ;
		}
		else
		{
			if (scrollbar->pressedsection == 1) scrollbar->pressed = -1;
			else scrollbar->scroll->pos += scrollbar->scroll->pagestep;
			if (scrollbar->scroll->pos > scrollbar->scroll->max)
				scrollbar->scroll->pos = scrollbar->scroll->max ;
		}
		scrollbar->pressedtime = gr_timer();
	}

	/* Update the elements size */
	if (scrollbar->style & SCROLLBAR_BUTTONS)
	{
		if (c->width > c->height)
		{
			e[1].x = c->width - SCROLLBAR_WIDTH;
			e[2].width = c->width - SCROLLBAR_WIDTH*2;
		}
		else
		{
			e[1].y = c->height - SCROLLBAR_HEIGHT;
			e[2].height = c->height - SCROLLBAR_HEIGHT*2;
		}
	}
	else
	{
		e[2].x = e[2].y = 0;
		e[2].width = c->width;
		e[2].height = c->height;
	}

	/* Draw the scrollbar */
	gr_setcolor (color_border) ;
	if (c->focused)
	{
		gr_hline (dest, clip, x-1, y-1, c->width+1);
		gr_vline (dest, clip, x-1, y-1, c->height+1);
	}

	/* Calculate the thumb position */
	e[3].x = e[2].x;
	e[3].y = e[2].y;
	e[3].width = SCROLLBAR_WIDTH;
	e[3].height = SCROLLBAR_HEIGHT;
	if (pos->max <= pos->min)
		pos->max = pos->min+1;
	if (c->width > c->height)
	{
		thumbsize = e[2].width * pos->pagestep / (pos->max + pos->pagestep - pos->min);
		if (thumbsize < SCROLLBAR_WIDTH)
		    thumbsize = SCROLLBAR_WIDTH;
		e[3].width = thumbsize;
		e[3].x += (e[2].width  - thumbsize)
			* pos->pos / (pos->max - pos->min) ;
	}
	else 
	{
		thumbsize = e[2].height * pos->pagestep / (pos->max + pos->pagestep - pos->min);
		if (thumbsize < SCROLLBAR_HEIGHT)
		    thumbsize = SCROLLBAR_HEIGHT;
		e[3].height = thumbsize;
		e[3].y += (e[2].height - thumbsize) 
			* pos->pos / (pos->max - pos->min) ;
	}

	/* Draw the elements */
	if (scrollbar->style & SCROLLBAR_BUTTONS)
	{
		drawbutton (dest, clip, scrollbar->pressed == 0,
				x+e[0].x, y+e[0].y, e[0].width, e[0].height,
				c->width > c->height ? DECORATION_LEFT : DECORATION_UP,
				c->highlight == 1, scrollbar->style & SCROLLBAR_FIXED, 
				c->width > c->height);
		drawbutton (dest, clip, scrollbar->pressed == 1,
				x+e[1].x, y+e[1].y, e[1].width, e[1].height,
				c->width > c->height ? DECORATION_RIGHT : DECORATION_DOWN,
				c->highlight == 2, scrollbar->style & SCROLLBAR_FIXED, 
				c->width > c->height);
	}
	if (scrollbar->style & SCROLLBAR_FRAME)
	{
		drawbutton (dest, clip, 2, x+e[2].x, y+e[2].y, 
				e[2].width, e[2].height, 0, 0, scrollbar->style & SCROLLBAR_FIXED, 
				c->width > c->height);
		if (scrollbar->pressed == 2 && scrollbar->pressedsection == 0)
		{
			drawbutton (dest, clip, 2, x+e[3].x, y+e[3].y, 
				e[2].width -  (e[3].x - e[2].x),  
				e[2].height - (e[3].y - e[2].y), 0, 1, scrollbar->style & SCROLLBAR_FIXED, 
				c->width > c->height);
		}
		if (scrollbar->pressed == 2 && scrollbar->pressedsection == 1)
		{
			drawbutton (dest, clip, 2, x+e[2].x, y+e[2].y, 
				e[2].x < e[3].x ? (e[3].x - e[2].x) : e[2].width,  
				e[2].y < e[3].y ? (e[3].y - e[2].y) : e[2].height, 0, 1, 
				scrollbar->style & SCROLLBAR_FIXED, c->width > c->height);
		}
	}

	if (scrollbar->style & SCROLLBAR_FIXED)
	{
		if (c->width > c->height)
		{
			gr_setcolor (color_face);
			gr_hline (dest, clip, x, y+c->height/2-3, c->width);
			gr_setcolor (color_shadow);
			gr_hline (dest, clip, x, y+c->height/2-2, c->width);
		}
		else
		{
			gr_setcolor (color_face);
			gr_vline (dest, clip, x+c->width/2+2, y, c->height);
			gr_setcolor (color_shadow);
			gr_vline (dest, clip, x+c->width/2+3, y, c->height);
		}
	}

	drawbutton (dest, clip, scrollbar->pressed == 3,
			x+e[3].x, y+e[3].y, 
			e[3].width, e[3].height, 
			DECORATION_THUMB, c->highlight == 4, 
			scrollbar->style & SCROLLBAR_FIXED, c->width > c->height);
}

/** Response to mouse move events
 *  This is a member function of the CONTROL:SCROLLBAR class
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param buttons	Button state (1 bit for button)
 * */

int gui_scrollbar_mousemove (CONTROL * c, int x, int y, int buttons)
{
	int i;
	SCROLLBAR * scrollbar = (SCROLLBAR *)c;
	SCROLLPOS * pos = scrollbar->scroll;

	for (i = 3 ; i >= 0 ; i--)
	{
		if (scrollbar->element[i].x <= x &&
		    scrollbar->element[i].y <= y &&
			scrollbar->element[i].x + scrollbar->element[i].width >= x &&
			scrollbar->element[i].y + scrollbar->element[i].height >= y)
			break;
	}
	if (c->highlight && i > -1)
		c->highlight = i+1;
	else
		c->highlight = 0;

	if (scrollbar->pressed == 3)
	{
		int incx = x - scrollbar->last_mousex;
		int incy = y - scrollbar->last_mousey;

		if (c->width > c->height)
		{
			pos->pos = scrollbar->last_pos + incx * (pos->max + pos->min) /
				(scrollbar->element[2].width - scrollbar->element[3].width);
		}
		else
		{
			pos->pos = scrollbar->last_pos + incy * (pos->max + pos->min) /
				(scrollbar->element[2].height - scrollbar->element[3].height);
		}
		if (scrollbar->style & SCROLLBAR_FIXED)
		{
			pos->pos += pos->step/2;
			pos->pos -= (pos->pos % pos->step) ;
		}
		if (scrollbar->scroll->pos > scrollbar->scroll->max)
		    scrollbar->scroll->pos = scrollbar->scroll->max;
		if (scrollbar->scroll->pos < scrollbar->scroll->min)
		    scrollbar->scroll->pos = scrollbar->scroll->min;
		return 1;
	}
	return 0;
}

/** Change the scrollbar's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:SCROLLBAR class
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the scrollbar pressed or released
 *  @param pressed	1 if the scrollbar was pressed, 0 if it was released
 * */

int gui_scrollbar_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	int i;
	SCROLLBAR * scrollbar = (SCROLLBAR *)c;

	for (i = 3 ; i >= 0 ; i--)
	{
		if (scrollbar->element[i].x <= x &&
		    scrollbar->element[i].y <= y &&
			scrollbar->element[i].x + scrollbar->element[i].width >= x &&
			scrollbar->element[i].y + scrollbar->element[i].height >= y)
			break;
	}

	if (!pressed)
	{
		if (i == 0)
		{
			scrollbar->scroll->pos -= scrollbar->scroll->step;
			if (scrollbar->scroll->pos < scrollbar->scroll->min)
				scrollbar->scroll->pos = scrollbar->scroll->min ;
		}
		else if (i == 1)
		{
			scrollbar->scroll->pos += scrollbar->scroll->step;
			if (scrollbar->scroll->pos > scrollbar->scroll->max)
				scrollbar->scroll->pos = scrollbar->scroll->max ;
		}
		else if (i == 2 && scrollbar->pressed == 2)
		{
			if ((c->width >  c->height && x < scrollbar->element[3].x) ||
			    (c->width <= c->height && y < scrollbar->element[3].y))
			{
				scrollbar->scroll->pos -= scrollbar->scroll->pagestep;
				if (scrollbar->scroll->pos < scrollbar->scroll->min)
					scrollbar->scroll->pos = scrollbar->scroll->min ;
			}
			else
			{
				scrollbar->scroll->pos += scrollbar->scroll->pagestep;
				if (scrollbar->scroll->pos > scrollbar->scroll->max)
					scrollbar->scroll->pos = scrollbar->scroll->max ;
			}
		}
		scrollbar->pressed = -1 ;
		c->redraw = 1;
	}
	else if (scrollbar->pressed != i)
	{
		scrollbar->last_mousex = x;
		scrollbar->last_mousey = y;
		scrollbar->last_pos = scrollbar->scroll->pos;
		scrollbar->pressed = i;
		if (i == 2)
		{
			scrollbar->pressedsection =
				((c->width >  c->height && x < scrollbar->element[3].x) ||
			     (c->width <= c->height && y < scrollbar->element[3].y)) ;
		}

		scrollbar->pressedtime = gr_timer() + 250;
		c->redraw = 1;
	}
	return 1;
}

/** The mouse leaves a scrollbar, change its pressed state.
 *  This is a member function of the CONTROL:SCROLLBAR class
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 * */

int gui_scrollbar_mouseleave (CONTROL *c)
{
	SCROLLBAR * scrollbar = (SCROLLBAR *)c ;

	if (scrollbar->pressed >= 0)
	{
		scrollbar->pressed = -1;
		c->redraw = 1;
	}
	return 1;
}

/** The user moves the focus to the scrollbar
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @return			1 if the control is capable of focusing
 */

int gui_scrollbar_enter (CONTROL * c)
{
	if (((SCROLLBAR *)c)->style & SCROLLBAR_FOCUSABLE)
	{
		c->focused = 1;
		c->redraw = 1;
		return 1;
	}
	return 0;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 */

int gui_scrollbar_leave (CONTROL * c)
{
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the scrollbar.control member (at offset 0 of the scrollbar)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_scrollbar_key (CONTROL * c, int scancode, int character)
{
	SCROLLBAR * scrollbar = (SCROLLBAR *)c;

	if (scancode == KEY_HOME || scancode == KEY_HOME+KEYMOD_CONTROL)
	{
		scrollbar->scroll->pos = scrollbar->scroll->min;
		c->redraw = 1;
		return 1;
	}
	if (scancode == KEY_END || scancode == KEY_END+KEYMOD_CONTROL)
	{
		scrollbar->scroll->pos = scrollbar->scroll->max;
		c->redraw = 1;
		return 1;
	}
	if (scancode == KEY_UP || scancode == KEY_LEFT)
	{
		scrollbar->scroll->pos -= scrollbar->scroll->step;
		if (scrollbar->scroll->pos < scrollbar->scroll->min)
			scrollbar->scroll->pos = scrollbar->scroll->min ;
		c->redraw = 1;
		return 1;
	}
	if (scancode == KEY_UP+KEYMOD_CONTROL || scancode == KEY_LEFT+KEYMOD_CONTROL || scancode == KEY_PAGEUP)
	{
		scrollbar->scroll->pos -= scrollbar->scroll->pagestep;
		if (scrollbar->scroll->pos < scrollbar->scroll->min)
			scrollbar->scroll->pos = scrollbar->scroll->min ;
		c->redraw = 1;
		return 1;
	}
	if (scancode == KEY_DOWN+KEYMOD_CONTROL || scancode == KEY_RIGHT+KEYMOD_CONTROL || scancode == KEY_PAGEDOWN)
	{
		scrollbar->scroll->pos += scrollbar->scroll->pagestep;
		if (scrollbar->scroll->pos > scrollbar->scroll->max)
			scrollbar->scroll->pos = scrollbar->scroll->max ;
		c->redraw = 1;
		return 1;
	}
	if (scancode == KEY_DOWN || scancode == KEY_RIGHT)
	{
		scrollbar->scroll->pos += scrollbar->scroll->step;
		if (scrollbar->scroll->pos > scrollbar->scroll->max)
			scrollbar->scroll->pos = scrollbar->scroll->max ;
		c->redraw = 1;
		return 1;
	}
	return 0;
}

/** Initialize a new horizontal scrollbar control 
 *
 *  @param scrollbar	Pointer to memory allocated for the object
 *  @param width		Width of the control in pixels
 *  @param data			Pointer to scroll data struct
 * */

void gui_hscrollbar_init (SCROLLBAR * scrollbar, int width, SCROLLPOS * data)
{
	gui_control_init (&scrollbar->control);

	/* Fill the control control struct data members */
	scrollbar->control.bytes = sizeof(SCROLLBAR);
	scrollbar->control.width = width;
	scrollbar->control.height = SCROLLBAR_HEIGHT;
	scrollbar->control.innerdrag = 1;

	/* Fill the control control struct member functions */
	scrollbar->control.draw = gui_scrollbar_draw;
	scrollbar->control.mouseleave = gui_scrollbar_mouseleave;
	scrollbar->control.mousebutton = gui_scrollbar_mousebutton;
	scrollbar->control.mousemove = gui_scrollbar_mousemove;
	scrollbar->control.enter = gui_scrollbar_enter;
	scrollbar->control.leave = gui_scrollbar_leave;
	scrollbar->control.key = gui_scrollbar_key;

	/* Left button */
	scrollbar->element[0].x = 0;
	scrollbar->element[0].y = 0;
	scrollbar->element[0].width = SCROLLBAR_WIDTH;
	scrollbar->element[0].height = SCROLLBAR_HEIGHT;

	/* Right button */
	scrollbar->element[1].x = width - SCROLLBAR_WIDTH;
	scrollbar->element[1].y = 0;
	scrollbar->element[1].width = SCROLLBAR_WIDTH;
	scrollbar->element[1].height = SCROLLBAR_HEIGHT;

	/* Frame */
	scrollbar->element[2].x = SCROLLBAR_WIDTH;
	scrollbar->element[2].y = 0;
	scrollbar->element[2].width = width - SCROLLBAR_WIDTH*2;
	scrollbar->element[2].height = SCROLLBAR_HEIGHT;

	scrollbar->style = SCROLLBAR_BUTTONS + SCROLLBAR_FRAME;
	scrollbar->scroll = data;
	scrollbar->pressed = -1;
	scrollbar->last_mousex = -1;
	scrollbar->last_mousey = -1;
}

/** Create a new horizontal scrollbar control 
 *
 *  @param width	Width of the control in pixels
 *  @param data		Pointer to scroll data struct
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_hscrollbar_new (int height, SCROLLPOS * data)
{
	SCROLLBAR * scrollbar ;

	/* Alloc memory for the struct */
	scrollbar = (SCROLLBAR *) malloc(sizeof(SCROLLBAR));
	if (scrollbar == NULL)
		return NULL;

	gui_hscrollbar_init (scrollbar, height, data);
	return &scrollbar->control;
}

/** Create a new vertical scrollbar control 
 *
 *  @param scrollbar	Pointer to memory allocated for the object
 *  @param height		Height of the control in pixels
 *  @param data			Pointer to scroll data struct
 * */

void gui_vscrollbar_init (SCROLLBAR * scrollbar, int height, SCROLLPOS * data)
{
	gui_control_init (&scrollbar->control);

	/* Fill the control control struct data members */
	scrollbar->control.bytes = sizeof(SCROLLBAR);
	scrollbar->control.width = SCROLLBAR_WIDTH;
	scrollbar->control.height = height;
	scrollbar->control.innerdrag = 1;

	/* Fill the control control struct member functions */
	scrollbar->control.draw = gui_scrollbar_draw;
	scrollbar->control.mouseleave = gui_scrollbar_mouseleave;
	scrollbar->control.mousebutton = gui_scrollbar_mousebutton;
	scrollbar->control.mousemove = gui_scrollbar_mousemove;
	scrollbar->control.enter = gui_scrollbar_enter;
	scrollbar->control.leave = gui_scrollbar_leave;
	scrollbar->control.key = gui_scrollbar_key;

	/* Left button */
	scrollbar->element[0].x = 0;
	scrollbar->element[0].y = 0;
	scrollbar->element[0].width = SCROLLBAR_WIDTH;
	scrollbar->element[0].height = SCROLLBAR_HEIGHT;

	/* Right button */
	scrollbar->element[1].x = 0;
	scrollbar->element[1].y = height - SCROLLBAR_HEIGHT;
	scrollbar->element[1].width = SCROLLBAR_WIDTH;
	scrollbar->element[1].height = SCROLLBAR_HEIGHT;

	/* Frame */
	scrollbar->element[2].x = 0;
	scrollbar->element[2].y = SCROLLBAR_HEIGHT;
	scrollbar->element[2].width = SCROLLBAR_WIDTH;
	scrollbar->element[2].height = height - SCROLLBAR_HEIGHT*2;

	scrollbar->style = SCROLLBAR_FRAME + SCROLLBAR_BUTTONS;

	scrollbar->scroll = data;
	scrollbar->pressedsection = -1;
	scrollbar->pressed = -1;
	scrollbar->last_mousex = -1;
	scrollbar->last_mousey = -1;
	scrollbar->pressedtime = 0;
}

/** Create a new vertical scrollbar control 
 *
 *  @param height	Height of the control in pixels
 *  @param data		Pointer to scroll data struct
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_vscrollbar_new (int height, SCROLLPOS * data)
{
	SCROLLBAR * scrollbar ;

	/* Alloc memory for the struct */
	scrollbar = (SCROLLBAR *) malloc(sizeof(SCROLLBAR));
	if (scrollbar == NULL)
		return NULL;

	gui_vscrollbar_init (scrollbar, height, data);
	return &scrollbar->control;
}

/** Change the style of an scroll bar
 *
 *  @param	control		Pointer to the scroll bar control
 *  @param	style		New style to apply
 */

void gui_scrollbar_style (CONTROL * control, int style)
{
	((SCROLLBAR *)control)->style = style;
}


