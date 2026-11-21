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
 *  Window-related functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

static GRAPH * close_mark = 0;
static GRAPH * maximize_mark = 0;
static GRAPH * minimize_mark = 0;
static GRAPH * restore_mark = 0;

void gui_action_closewindow (WINDOW * w)
{
	gui_desktop_removewindow (w);
}
void gui_action_maximizewindow (WINDOW * w)
{
	int menu_bar_sep = 0;

	if (!w->maximize)
		return;

	w->redraw = 1;
	w->maximize->highlight = 0;

	if (w->maximized)
	{
		gui_window_resize (w, w->restorewidth, w->restoreheight);
		gui_window_moveto (w, w->restorex, w->restorey);
		((TOOL *)w->maximize)->graph = maximize_mark;
		w->maximized = 0;
		gui_desktop_adjustdock();
	}
	else
	{
		w->restorewidth = w->width;
		w->restoreheight = w->height;
		w->restorex = w->x;
		w->restorey = w->y;
		if (w->fullscreen)
		{
			gui_window_moveto (w, -w->border-w->padding - 1, -w->border-w->caption-w->padding - 1);
			gui_window_resize (w, scr_width + 2*w->border + 2*w->padding + 2, 
				scr_height + 2*w->border + 2*w->padding + 2);
		}
		else
		{
			gui_window_resize (w, scr_width, 
				scr_height - w->caption - menu_bar_sep);
			gui_window_moveto (w, scr_width - w->width, menu_bar_sep);
		}
		((TOOL *)w->maximize)->graph = restore_mark;
		w->maximized = 1;
		gui_desktop_adjustdock();
	}
}
void gui_action_minimizewindow (WINDOW * w)
{
	if (w->minimize)
	{
		gui_desktop_sendtoback(w);
		gr_mark_rect (w->x, w->y, w->width, w->height + w->caption);
		w->visible = 0;
	}
}

/** Draw a window frame (the window's background)
 */

void gui_window_draw_frame (WINDOW * w, GRAPH * dest, REGION * clip)
{
	int i;

	/* Search transparent or solid controls and draw the background
	 * around them to minimize drawing operations */

	for (i = 0 ; i < w->control_count ; i++)
	{
		CONTROL * c = w->control[i];

		if (c->x+w->x <= clip->x2 && 
			c->y+w->caption+w->y <= clip->y2 &&
			c->x+w->x + c->width > clip->x && 
			c->y+w->caption+w->y + c->height > clip->y)
		{
			if (c->bgmode != BG_WINDOW)
			{
				REGION zone;

				/* Top frame */
				zone.x = 0; zone.x2 = scrbitmap->width-1;
				zone.y = 0; zone.y2 = w->y+w->caption+c->y-1;
				region_union (&zone, clip);
				if (!region_is_empty(&zone))
					gui_window_draw_frame (w, dest, &zone);

				/* Left frame */
				zone.x = 0; zone.x2 = w->x+c->x-1;
				zone.y = w->y+w->caption+c->y; zone.y2 = zone.y+c->height-1;
				region_union (&zone, clip);
				if (!region_is_empty(&zone))
					gui_window_draw_frame (w, dest, &zone);

				/* Right frame */
				zone.x = w->x+c->x+c->width; zone.x2 = scrbitmap->width-1;
				zone.y = w->y+w->caption+c->y; zone.y2 = zone.y+c->height-1;
				region_union (&zone, clip);
				if (!region_is_empty(&zone))
					gui_window_draw_frame (w, dest, &zone);

				/* Bottom frame */
				zone.x = 0; zone.x2 = scrbitmap->width-1;
				zone.y = w->y+w->caption+c->y+c->height; zone.y2 = scrbitmap->height-1;
				region_union (&zone, clip);
				if (!region_is_empty(&zone))
					gui_window_draw_frame (w, dest, &zone);

				return;
			}
		}
	}

	gr_setcolor (color_window);
	gr_box      (dest, clip, clip->x, clip->y, 
		clip->x2 - clip->x + 1, clip->y2 - clip->y + 1);
}

/** Draw a window.
 *
 *  @param window	Pointer to the affected window object
 *  @param w		Pointer to the window object
 *  @param dest		Destination bitmap
 */

void gui_window_draw (WINDOW * w, GRAPH * dest, REGION * clip)
{
	REGION * 	region;
	REGION *    sysregion;
	REGION      zone;
	int			i;
   
	/* Create a region for the window limits */
	region = region_new (
			w->x + w->border, 
			w->y + w->border + w->caption, 
			w->width - 2*w->border, 
			w->height - 2*w->border);
	sysregion = region_new (
			w->x,
			w->y, 
			w->width, 
			w->caption);
	
	if (clip)
	{
		region_union (region, clip);
		region_union (sysregion, clip);
	}

	gr_setalpha(w->alpha);

	/* Draw the window frame */
	zone.x = w->x;
	zone.y = w->y + w->caption;
	zone.x2 = zone.x + w->width-1;
	zone.y2 = zone.y + w->height-1;
	region_union (&zone, clip);
	if (!region_is_empty(&zone))
		gui_window_draw_frame (w, dest, &zone);

	/* Draw the window caption */
	if (w->caption > 0 && w->maximized == 0)
	{
		const char * text = w->text;
		if (*text == '*') text++;

		gr_setcolor (w->focused ? color_captionfg : color_captionbg);
		gr_box (dest, sysregion, w->x, w->y, w->width, w->caption);
		gr_setcolor (color_highlight);
		gr_rectangle (dest, sysregion, w->x, w->y, w->width, w->caption);
		gr_setcolor (color_shadow);
		gr_hline (dest, sysregion, w->x, w->y+w->caption-1, w->width);
		gr_vline (dest, sysregion, w->x+w->width-1, w->y, w->caption);
		gr_text_put (dest, sysregion, 0, 
				w->x + 8, 
				w->y + (w->caption - gr_text_height(0,text))/2, 
				text);
		if (w->changed)
		{
			gr_text_put (dest, sysregion, 0, w->x + 12 + gr_text_width(0,text), 
				w->y + (w->caption - gr_text_height(0,text))/2, "(*)");
		}

		/* Create graphics for the system buttons (only once) */
		if (close_mark == NULL)
		{
			close_mark = bitmap_new (0, 8, 6, 1);
			((char *)close_mark->data)[0 * close_mark->pitch] = 0x63;
			((char *)close_mark->data)[1 * close_mark->pitch] = 0x36;
			((char *)close_mark->data)[2 * close_mark->pitch] = 0x1C;
			((char *)close_mark->data)[3 * close_mark->pitch] = 0x1C;
			((char *)close_mark->data)[4 * close_mark->pitch] = 0x36;
			((char *)close_mark->data)[5 * close_mark->pitch] = 0x63;
		}
		if (maximize_mark == NULL)
		{
			maximize_mark = bitmap_new (0, 8, 7, 1);
			((char *)maximize_mark->data)[0 * maximize_mark->pitch] = 0xFF;
			((char *)maximize_mark->data)[1 * maximize_mark->pitch] = 0xFF;
			((char *)maximize_mark->data)[2 * maximize_mark->pitch] = 0x81;
			((char *)maximize_mark->data)[3 * maximize_mark->pitch] = 0x81;
			((char *)maximize_mark->data)[4 * maximize_mark->pitch] = 0x81;
			((char *)maximize_mark->data)[5 * maximize_mark->pitch] = 0x81;
			((char *)maximize_mark->data)[6 * maximize_mark->pitch] = 0xFF;
		}
		if (minimize_mark == NULL)
		{
			minimize_mark = bitmap_new (0, 8, 6, 1);
			((char *)minimize_mark->data)[0 * minimize_mark->pitch] = 0x00;
			((char *)minimize_mark->data)[1 * minimize_mark->pitch] = 0x00;
			((char *)minimize_mark->data)[2 * minimize_mark->pitch] = 0x00;
			((char *)minimize_mark->data)[3 * minimize_mark->pitch] = 0x00;
			((char *)minimize_mark->data)[4 * minimize_mark->pitch] = 0xFF;
			((char *)minimize_mark->data)[5 * minimize_mark->pitch] = 0xFF;
		}
		if (restore_mark == NULL)
		{
			restore_mark = bitmap_new (0, 8, 6, 1);
			((char *)restore_mark->data)[0 * restore_mark->pitch] = 0x00;
			((char *)restore_mark->data)[1 * restore_mark->pitch] = 0x7E;
			((char *)restore_mark->data)[2 * restore_mark->pitch] = 0x7E;
			((char *)restore_mark->data)[3 * restore_mark->pitch] = 0x42;
			((char *)restore_mark->data)[4 * restore_mark->pitch] = 0x42;
			((char *)restore_mark->data)[5 * restore_mark->pitch] = 0x7E;
		}

		/* Create the windows system buttons, if not present already */
		if (w->close == NULL)
		{
			w->close = gui_tool_new (close_mark);
			gui_button_callbackp (w->close, gui_action_closewindow, w);
			((TOOL *)w->close)->frame = 1;
			gui_window_addcontrol (w, 0, 0, w->close);
			w->close->width = w->caption-3;
			w->close->height = w->caption-5;
			w->close->hidden = 1;
		}
		if (w->maximize == NULL && w->resizable)
		{
			w->maximize = gui_tool_new (maximize_mark);
			gui_button_callbackp (w->maximize, gui_action_maximizewindow, w);
			((TOOL *)w->maximize)->frame = 1;
			gui_window_addcontrol (w, 0, 0, w->maximize);
			w->maximize->width = w->caption-2;
			w->maximize->height = w->caption-5;
			w->maximize->hidden = 1;
		}
		if (w->minimize == NULL && w->minimizable)
		{
			w->minimize = gui_tool_new (minimize_mark);
			gui_button_callbackp (w->minimize, gui_action_minimizewindow, w);
			((TOOL *)w->minimize)->frame = 1;
			gui_window_addcontrol (w, 0, 0, w->minimize);
			w->minimize->width = w->caption-2;
			w->minimize->height = w->caption-5;
			w->minimize->hidden = 1;
		}

		/* Draw the system buttons */
		if (w->close)
		{
			w->close->focused = 0;
			w->close->y = -w->caption+2;
			w->close->x = w->width - w->close->width - 4;
			(*w->close->draw) (w->close, dest, w->x + w->close->x,
					w->y + w->close->y + w->caption, sysregion);
		}
		if (w->maximize)
		{
			w->maximize->focused = 0;
			w->maximize->y = -w->caption+2;
			w->maximize->x = w->close->x - w->maximize->width - 6;
			(*w->maximize->draw) (w->maximize, dest, w->x + w->maximize->x,
					w->y + w->maximize->y + w->caption, sysregion);
		}
		if (w->minimize)
		{
			w->minimize->focused = 0;
			w->minimize->y = -w->caption+2;
			w->minimize->x = (w->maximize ? w->maximize->x : w->close->x - 6) 
				- w->minimize->width;
			(*w->minimize->draw) (w->minimize, dest, w->x + w->minimize->x,
					w->y + w->minimize->y + w->caption, sysregion);
		}
	}

	/* Draw the window border */
	if (w->border > 0)
	{
		gr_setcolor (color_highlight);
		gr_hline    (dest, clip, w->x, w->y + w->caption, w->width);
		gr_vline    (dest, clip, w->x, w->y + w->caption, w->height);
		gr_setcolor (color_shadow);
		gr_hline    (dest, clip, w->x, w->y + w->caption + w->height - 1, w->width);
		gr_vline    (dest, clip, w->x + w->width - 1, w->y + w->caption, w->height);

		if (w->border > 1 && w->resizable && !w->maximized)
		{
			int b = w->border / 2;
			gr_setcolor (color_shadow);
			gr_hline (dest, clip, w->x + b, w->y + w->caption + b, w->width - w->border);
			gr_vline (dest, clip, w->x + b, w->y + w->caption + b, w->height - b*2);
			gr_setcolor (color_highlight);
			gr_hline (dest, clip, w->x + b, w->y + w->caption + w->height - b - 1, w->width - b*2);
			gr_vline (dest, clip, w->x + w->width - b - 1, w->y + w->caption + b, w->height - b*2);
		}
	}

	/* Draw the controls */
	for (i = 0 ; i < w->control_count ; i++)
	{
		CONTROL * control = w->control[i];

		assert (control != NULL);

		if (!control->hidden)
		{
			(*control->draw) (control, dest, w->x + control->x,
					w->y + control->y + w->caption, region);
		}
	}

	region_destroy(region);
	region_destroy(sysregion);
}

/** Find a window control
 *
 *  @param x		X coordinate
 *  @param y		Y coordinate
 *  @return			Pointer to the control or NULL if none there
 */

CONTROL * gui_window_findcontrol (WINDOW * window, int x, int y)
{
	int i ;

	y -= window->caption;

	for (i = window->control_count-1 ; i >= 0 ; i--)
	{
		assert (window->control[i] != NULL);

		if ((window->control[i]->hidden == 0 || window->control[i]->y < 0) &&
			window->control[i]->x <= x &&
		    window->control[i]->y <= y &&
		    window->control[i]->x + window->control[i]->width - 1 >= x &&
		    window->control[i]->y + window->control[i]->height - 1 >= y)
			return window->control[i];
	}
	return NULL;
}

/** Propagate a mouse move event
 *
 *  @param window	Pointer to the affected window object
 *  @param x		Mouse X coordinate
 *  @param y		Mouse Y coordinate
 */

void gui_window_mousemove (WINDOW * window, int x, int y, int buttons)
{
	CONTROL * last_one = NULL;
	CONTROL * this_one = NULL;
	int       i;

	/* Mouse capture */
	if (window->captured_mouse)
	{
		CONTROL * c = window->captured_mouse;
		(*c->mousemove)(c, x - c->x, y - c->y - window->caption, buttons);
		c->actionflags |= ACTION_MOUSEMOVE;
		return;
	}
		
	/* Window dragging mode */
	if (window->drag == DRAG_MOVE)
	{
		gr_mark_rect (window->x, window->y, window->width+3, window->caption + window->height+3);
		window->x += x - window->last_mousex;
		window->y += y - window->last_mousey;
		window->redraw = 1;
		return;
	}
	if (window->drag & DRAG_RESIZEHV)
	{
		int newwidth = window->width;
		int newheight = window->height;
		if (window->drag & DRAG_RESIZEH)
			newwidth = window->lastwidth + x - window->last_mousex;
		if (window->drag & DRAG_RESIZEV)
			newheight = window->lastheight + y - window->last_mousey;
		gui_window_resize (window, newwidth, newheight);
		window->redraw = 1;
		return;
	}

	/* Hack: adjust mouse positions to ignore the top-left window borders */
	if (x <= window->border && x >= 0) 
		x = window->border+1;
	if (y <= window->border && y >= 0) 
		y = window->border+1;

	/* Idem for bottom-right borders */
	if (window->maximized || !window->resizable)
	{
		if (x >= window->width - window->border && x < window->width) 
			x = window->width - window->border - 3;
		if (y >= window->height + window->caption - window->border - 1 && y <= window->height + window->caption)
			y = window->caption + window->height - window->border - 2;
	}

	/* Inner dragging in a control */
	if (buttons && window->control_drag)
	{
		(*window->control_drag->mousemove)(window->control_drag, x - window->control_drag->x, 
					y - window->caption - window->control_drag->y, buttons);
		window->control_drag->actionflags |= ACTION_MOUSEMOVE;
		if (window->control_drag->draggable == 1 && (buttons & 0x01))
		{
			gui_desktop_dragobject (window->control_drag->drstring, window->control_drag->drgraph);
			window->innerdrag = 0;
			return;
		}
	
		return;
	}
	else if (!buttons && window->control_drag)
	{
		window->control_drag = NULL;
		window->innerdrag = 0;
	}
	for (i = 0 ; i < 8 ; i++)
	{
		if ((buttons & (1 << i)) && window->clicked_control[i])
		{
			CONTROL * c = window->clicked_control[i];
			if ((*c->mousemove)(c, x - c->x, y - c->y - window->caption, buttons))
			{
				c->actionflags |= ACTION_MOUSEMOVE;
				return;
			}
		}
	}

	if (window->last_mousex != -1)
		last_one = gui_window_findcontrol (window, window->last_mousex, window->last_mousey);

	if (window->innerdrag && last_one)
	{
		(*last_one->mousemove)(last_one, x - last_one->x, 
							   y - window->caption - last_one->y, buttons);
		last_one->actionflags |= ACTION_MOUSEMOVE;
		return;
	}

	window->innerdrag = 0;
	window->last_mousex = x;
	window->last_mousey = y;

	this_one = gui_window_findcontrol (window, x, y);

	if (this_one != last_one)
	{
		if (last_one)
		{
			(*last_one->mouseleave)(last_one);
			if (last_one->highlight)
			{
				last_one->highlight = 0;
				last_one->redraw = 1;
			}
			gui_desktop_tooltip(0);
		}
		if (this_one)
		{
			(*this_one->mouseenter)(this_one);
			if (!this_one->highlight)
			{
				this_one->highlight = 1;
				this_one->redraw = 1;
			}
			if (this_one->tooltip)
				gui_desktop_tooltip(string_get(this_one->tooltip));
		}
	}
	if (this_one)
	{
		(*this_one->mousemove)(this_one, x - this_one->x, 
							   y - window->caption - this_one->y, buttons);
		this_one->actionflags |= ACTION_MOUSEMOVE;
	}
}

/** Propagate a mouse button event
 *
 *  @param window	Pointer to the affected window object
 *  @param x		Mouse X coordinate
 *  @param y		Mouse Y coordinate
 */

void gui_window_mousebutton (WINDOW * window, int x, int y, int b, int p)
{
	int i, should_return = 0;
	CONTROL * this_one;

	/* Mouse capture */
	if (window->captured_mouse)
	{
		CONTROL * c = window->captured_mouse;
		(*c->mousebutton)(c, x - c->x, y - c->y - window->caption, b, p);
		c->actionflags |= (1 << b);
		return;
	}

	if (window->drag && !p)
	{
		window->drag = 0;
		return;
	}
	
	/* Hack: adjust mouse positions to ignore the top-left window borders */
	if (x <= window->border && x >= 0) x = window->border+1;
	if (y <= window->border && y >= 0) y = window->border+1;

	/* Idem for bottom-right borders */
	if (window->maximized || !window->resizable)
	{
		if (x >= window->width - window->border && x < window->width) 
			x = window->width - window->border - 3;
		if (y >= window->height + window->caption - window->border - 1 && y <= window->height + window->caption)
			y = window->caption + window->height - window->border - 2;
	}

	/* Find the control at mouse position */

	if (p == 0 && window->clicked_control[b])
	{
		this_one = window->clicked_control[b];
		window->clicked_control[b] = 0;
	}
	else
		this_one = gui_window_findcontrol (window, x, y);

	/* ALT+LMB: drag the window around */
	if (b == 0 && p == 1 && gr_key(KEY_LALT) && !gr_key(KEY_LCTRL) && !window->drag &&
		(!this_one || !this_one->allkeys))
	{
		window->last_mousex = x;
		window->last_mousey = y;
		window->drag = DRAG_MOVE;
		return;
	}

	if (this_one)
	{
		window->control_drag = 0;
		if (!p && this_one->innerdrag)
		{
			window->innerdrag = 0;
			window->control_drag = 0;
		}

		if (p == 1 && b < 5)
		{
			this_one->actionflags |= (1 << b);
			window->clicked_control[b] = this_one;
		}
		if (p == 2 && b == 0)
			this_one->actionflags |= ACTION_DOUBLECLICK;

		if ((*this_one->mousebutton)(this_one, x - this_one->x, 
				 y - window->caption - this_one->y, b, p))
		{
			if (p && this_one->innerdrag)
			{
				window->innerdrag = 1;
				window->control_drag = this_one;
			}
			should_return = 1;
		}
		else if (this_one->draggable == 1)
		{
			window->innerdrag = 1;
			window->control_drag = this_one;
			should_return = 1;
		}
		else
			window->innerdrag = 0;

		/* The window may have been destroyed at this point */
		if (!gui_desktop_present(window))
			return;

		/* Try to give the focus to the new control */
		if (this_one != window->focus && (*this_one->enter)(this_one))
		{
			this_one->actionflags |= ACTION_ENTER;

			if (window->focus)
			{
				(*window->focus->leave)(window->focus);
				window->focus->actionflags |= ACTION_LEAVE;
				window->focus->focused = 0;
			}
			window->focus = this_one;
			this_one->focused = 1;
		}

		if (should_return)
			return;

		/* The user clicks on an inactive control, such as a label.
		   Move the focus to the next control, if any accepts it */

		for (i = 0 ; i < window->control_count ; i++)
		{
			if (window->control[i] == this_one)
			{
				for (; i < window->control_count ; i++)
				{
					this_one = window->control[i];
					if ((*this_one->enter)(this_one))
					{
						this_one->actionflags |= ACTION_ENTER;

						if (window->focus)
						{
							(*window->focus->leave)(window->focus);
							window->focus->actionflags |= ACTION_LEAVE;
							window->focus->focused = 0;
						}
						window->focus = this_one;
						this_one->focused = 1;
						break;
					}
				}
				break;
			}
		}
	}

	/* Dragging mode */

	window->last_mousex = x;
	window->last_mousey = y;
	if (x > window->width - window->border - 2 &&
		y > window->height + window->caption - window->border - 2 &&
		window->resizable && !window->maximized)
	{
		window->lastwidth = window->width;
		window->lastheight = window->height;

		if (p) window->drag = DRAG_RESIZEHV;
	}
	else if (x > window->width - window->border - 2
		     && window->resizable && !window->maximized)
	{
		window->lastwidth = window->width;
		if (p) window->drag = DRAG_RESIZEH;
	}
	else if (y > window->height + window->caption - window->border - 2
		     && window->resizable && !window->maximized)
	{
		window->lastheight = window->height;
		if (p) window->drag = DRAG_RESIZEV;
	}
	else if (!window->fixed && !window->maximized && window->moveable)
	{
		if (p) window->drag = DRAG_MOVE;
	}

}

/** Propagate a mouse leave event
 *
 *  @param window	Pointer to the affected window object
 */

void gui_window_mouseleave (WINDOW * window)
{
	if (window->last_mousex != -1)
	{
		CONTROL * last_one = gui_window_findcontrol (window, 
				window->last_mousex, window->last_mousey);
		if (last_one)
		{
			(*last_one->mouseleave)(last_one);
			if (last_one->highlight)
			{
				last_one->highlight = 0;
				last_one->redraw = 1;
			}
		}
	}

	window->last_mousex = -1;
	window->last_mousey = -1;
}

/** Propagate a mouse enter event
 *
 *  @param window	Pointer to the affected window object
 */

void gui_window_mouseenter (WINDOW * window)
{
}

/** Propagate a keyboard press event
 *
 *  @param window	Pointer to the affected window object
 *  @param scancode	Keyboard scan code of the key
 *  @param key		ASCII value of the character
 */

int gui_window_key (WINDOW * window, int scancode, int key)
{
	/* TAB navigation */

	if (window->focused && (scancode == KEY_TAB || scancode == KEY_TAB+KEYMOD_SHIFT))
	{
		int focus_n = -1, i;

		/* Find the control with the focus */
		for (i = 0 ; i < window->control_count ; i++)
		{
			if (window->control[i] == window->focus)
				focus_n = i;
		}

		if (focus_n == -1)
		{
			/* No control with focus - assign it to the first one */
			for (i = 0 ; i < window->control_count ; i++)
			{
				if ((*window->control[i]->enter)(window->control[i]))
				{
					window->control[i]->actionflags |= ACTION_ENTER;
					window->focus = window->control[i];
					window->focus->focused = 1;
					return 1;
				}
			}
		}
		else
		{
			/* Search the next or previous control */
			for (i = focus_n ; ;)
			{
				if (scancode == KEY_TAB)
				{
					if (++i == window->control_count) 
						i = 0;
				}
				else
				{
					if (--i < 0)
						i = window->control_count-1;
				}

				if (i == focus_n) break;
				if ((*window->control[i]->enter)(window->control[i]))
				{
					window->control[i]->actionflags |= ACTION_ENTER;
					(*window->focus->leave)(window->focus);
					window->focus->actionflags |= ACTION_LEAVE;
					window->focus->focused = 0;
					window->focus = window->control[i];
					window->focus->focused = 1;
					return 1;
				}
			}
		}
	}

	/* Direct access with ALT+key */

	if ((window->dock || window->focused) && 
		(scancode & KEYMOD_ALT) && key && window->control_count)
	{
		int focus_n = -1, i, accept_any = 0;
		CONTROL * last = window->focus;

		if (!last) last = window->control[window->control_count-1];

		/* Find the control with the focus */
		for (i = 0 ; ; i++)
		{
			if (i == window->control_count)
				i = 0;

			if (focus_n != -1)
			{
				/* The control matches the key? */
				if (accept_any || 
				   (*window->control[i]->syskey)(window->control[i], key))
				{
					accept_any = 1;

					/* Try to move the focus to the control */
					if ((*window->control[i]->enter)(window->control[i]))
					{
						/* The control accepts the focus */
						window->control[i]->actionflags |= ACTION_ENTER;
						(*window->focus->leave)(window->focus);
						window->focus->actionflags |= ACTION_LEAVE;
						window->focus->focused = 0;
						window->focus = window->control[i];
						window->focus->focused = 1;
						return 1;
					}

					/* The control does not accept the focus, but any control
					   after it will be accepted as a valid destination if accepts
					   (accept_any is 1) */
				}
			}

			/* Don't do anything until we find the current focus */
			if (window->control[i] == last)
			{
				if (focus_n == i)
					break;
				focus_n = i;
			}
			
		}
	}

	/* We should not propagate this it not focused... (?) */
	if (window->focus != NULL)
	{
		window->focus->actionflags |= ACTION_KEY;
		return (*window->focus->key)(window->focus, scancode, key);
	}

	return 0;
}

/** The window has got the focus
 *
 *	@param window		Pointer to the window object
 **/

void gui_window_enter (WINDOW * window)
{
	if (window->focus)
	{
		(*window->focus->enter)(window->focus);
		window->focus->actionflags |= ACTION_ENTER;
	}
	window->focused = 1;
}

/** The window loses the focus
 *
 *	@param window		Pointer to the window object
 **/

void gui_window_leave (WINDOW * window)
{
	if (window->focus)
	{
		(*window->focus->leave)(window->focus);
		window->focus->actionflags |= ACTION_LEAVE;
	}
	window->focused = 0;
	gui_desktop_tooltip(0);
}


/** Add a control to a window.
 *  You shold not free the control object, it will be automatically destroyed
 *  at the window destruction point by gui_window_destroy.
 *
 *  @param window	Pointer to the window object
 *  @param control	Pointer to the CONTROL object
 */

void gui_window_addcontrol (WINDOW * window, int x, int y, CONTROL * control)
{
	/* Alloc more memory if the available is already used */
	if (window->control_allocated == window->control_count)
	{
		CONTROL * * control = (CONTROL * *) realloc (window->control,
				sizeof(CONTROL * *) * (window->control_allocated + 16));
		if (control == NULL)
			return;
		window->control = control;
		window->control_allocated += 16;
	}

	control->x += x + window->padding + window->border;
	control->y += y + window->padding + window->border;
	control->window = window;

	window->control[window->control_count++] = control;

	/* Resize the window to allow space for the control, if needed */
	if (window->width < control->x + control->width + window->border + window->padding)
		window->width = control->x + control->width + window->border + window->padding;
	if (window->height < control->y + control->height + window->border + window->padding)
		window->height = control->y + control->height + window->border + window->padding;

	/* Give the focus to the new control, if possible */
	if (window->focus == NULL && (*control->enter)(control))
	{
		window->focus = control;
		window->focus->actionflags |= ACTION_ENTER;
	}

	window->dirty = 1;

	if (control->bgmode == BG_TRANSPARENT)
		window->transparent = 1;
}

/** Find if there are resizable controls inside a given rect (even in part) of the window
 *
 *	@param window			Pointer to the window object
 *	@param x0				Left coordinate
 *	@param y0				Top coordinate
 *	@param x1				Right coordinate
 *	@param y1				Bottom coordinate
 *  @param direction		0 to ignore vertically non-resizable, 1 to ignore horizontally non-resizable
 *	@return					1 if there isn't at least one control touching the area
 **/

int gui_window_rectempty (WINDOW * window, int x0, int y0, int x1, int y1, int direction)
{
	int i;

	for (i = 0 ; i < window->control_count ; i++)
	{
		CONTROL * c = window->control[i];

		if (c->hidden) continue;
		if ( direction && !c->hresizable) continue;
		if (!direction && !c->vresizable) continue;
		if (c->x <= x1 && c->y <= y1 &&
			c->x + c->width > x0 &&
			c->y + c->height > y0)
			return 0;
	}
	return 1;
}

/** Move all controls inside a given rect 
 *
 *	@param window			Pointer to the window object
 *	@param x0				Left coordinate
 *	@param y0				Top coordinate
 *	@param x1				Right coordinate
 *	@param y1				Bottom coordinate
 *  @param incx				Increment to x coordinate
 *	@param incy				Increment to y coordinate
 */

void gui_window_movecontrols (WINDOW * window, int x0, int y0, int x1, int y1, int incx, int incy)
{
	int i;

	for (i = 0 ; i < window->control_count ; i++)
	{
		CONTROL * c = window->control[i];

		if (c->hidden) continue;
		if (c->x <= x1 && c->y <= y1 &&
			c->x + c->width > x0 &&
			c->y + c->height > y0)
		{
			c->x += incx;
			c->y += incy;
		}
	}
}

/** Move a given window to a new position in screen
 *
 *	@param window		Pointer to the window object
 *	@param x			New X coordinate (left)
 *	@param y			New Y coordinate (top)
 **/

void gui_window_moveto (WINDOW * w, int x, int y)
{
	gr_mark_rect (w->x, w->y, w->width+3, w->caption + w->height+3);
	w->x = x;
	w->y = y;
	w->redraw = 1;
}

/** Create a new window
 *
 *  @return Pointer to the new WINDOW object or NULL if not enough memory
 */

WINDOW * gui_window_new()
{
	static int next_id = 0;
	WINDOW * window;

	/* Alloc memory for the window */
	window = (WINDOW *)malloc(sizeof(WINDOW));
	if (window == NULL)
		return NULL;

	/* Alloc memory for the control array */
	window->control = (CONTROL * *)malloc(sizeof(CONTROL *) * 16);
	if (window->control == NULL)
	{
		free (window);
		return NULL;
	}

	window->text[0] = 0;

	/* Initialize data members */
	window->id = next_id++;
	window->control_allocated = 16;
	window->control_count = 0;
	window->x = -1;
	window->y = -1;
	window->width = 0;
	window->height = 0;
	window->border = 0;
	window->caption = 0;
	window->last_mousex = -1;
	window->last_mousey = -1;
	window->focus = NULL;
	window->drag = 0;
	window->innerdrag = 0;
	window->focused = 0;
	window->padding = 1;
	window->fixed = 0;
	window->moveable = 1;
	window->stayontop = 0;
	window->resizable = 0;
	window->minimizable = 0;
	window->close = NULL;
	window->maximize = NULL;
	window->minimize = NULL;
	window->maximized = 0;
	window->dontfocus = 0;
	window->vlines = NULL;
	window->hlines = NULL;
	window->vlines_count = 0;
	window->hlines_count = 0;
	window->vlines_allocated = 0;
	window->hlines_allocated = 0;
	window->dirty = 1;
	window->alpha = 255;
	window->visible = 1;
	window->control_drag = 0;
	window->transparent = 0;
	window->shadow = 1;
	window->modal = 0;
	window->userdata = 0;
	window->usertype = 0;
	window->userfile = 0;
	window->control_drag = 0;
	window->changed = 0;
	window->clicked_control[0] = 0;
	window->clicked_control[1] = 0;
	window->clicked_control[2] = 0;
	window->clicked_control[3] = 0;
	window->clicked_control[4] = 0;
	window->clicked_control[5] = 0;
	window->clicked_control[6] = 0;
	window->clicked_control[7] = 0;
	window->captured_mouse = 0;
	window->fullscreen = 0;
	window->redraw = 1;
	return window;
}

/** Create a new window, with a title
 *
 *  @return Pointer to the new WINDOW object or NULL if not enough memory
 */

WINDOW * gui_window_newt (const char * title)
{
	WINDOW * window = gui_window_new();

	if (window)
	{
		window->padding = 4;
		window->border = 2;
		gui_window_caption (window, title);
	}
	return window;
}

/** Destroy a window object and all its controls
 *
 *  @param window	Pointer to the window object
 */

void gui_window_destroy (WINDOW * window)
{
	int i;

	assert (window != NULL);

	/* Destroy tension lines */
	if (window->vlines != NULL)
		free (window->vlines);
	if (window->hlines != NULL)
		free (window->hlines);

	/* Destroy controls */
	if (window->control != NULL)
	{
		for (i = 0 ; i < window->control_count ; i++)
		{
			assert (window->control[i] != NULL);
			(*window->control[i]->destructor)(window->control[i]);
			free(window->control[i]);
		}
		free(window->control);
	}

	/* Release strings */
	if (window->usertype)
		string_discard (window->usertype);
	if (window->userdata)
		string_discard (window->userdata);
	if (window->userfile)
		string_discard (window->userfile);

	/* Destroy the window itself */
	free(window);
}

/** Set the caption text of a given window
 *
 *  @param window	Pointer to the window object
 *  @param caption	Pointer to the new caption text or NULL for no caption
 */

void gui_window_caption (WINDOW * window, const char * caption)
{
	window->caption = 0;

	if (caption != NULL)
	{
		strncpy (window->text, caption, WINDOW_TEXT_MAX);
		window->text[WINDOW_TEXT_MAX-1] = 0;
		if (window->text[0] != '*')
			window->caption = 17;
	}
}

/** Set the style of a window (a combination of borders, system buttons, etc)
 *
 *	@param window		Pointer to the window object
 *	@param style		One of the STYLE_ constants
 **/

void gui_window_style (WINDOW * window, int style)
{
	window->dock = 0;
	window->stayontop = 0;

	if (style == STYLE_FIXED)
	{
		window->resizable = 0;
		window->minimizable = 1;
		window->border = 4;
		window->moveable = 1;
	}
	if (style == STYLE_STATIC)
	{
		window->resizable = 0;
		window->minimizable = 0;
		window->border = 0;
		window->moveable = 1;
		window->padding = 0;
		window->innerdrag = 1;
		window->shadow = 0;
	}
	if (style == STYLE_DIALOG)
	{
		window->resizable = 0;
		window->minimizable = 0;
		window->border = 4;
		window->moveable = 1;
	}
	if (style == STYLE_RESIZABLE_DIALOG)
	{
		window->resizable = 1;
		window->minimizable = 0;
		window->border = 2;
		window->moveable = 1;
	}
	if (style == STYLE_NORMAL)
	{
		window->resizable = 1;
		window->minimizable = 1;
		window->border = 4;
		window->moveable = 1;
	}
	if (style >= STYLE_DOCK_NORTH)
	{
		window->resizable = 0;
		window->minimizable = 1;
		window->border = 1;
		window->moveable = 0;
		window->dock = style;
		window->stayontop = 1;
		window->dontfocus = 1;
		window->shadow = 0;
	}
}

/** Add a vertical tension line to a window
 *
 *  @param win			Pointer to the window object
 *	@param first		First control, -1 for window border
 *	@param second		Second control, -1 for window border
 *	@param size			Size of the tension line in pixels
 */

static void gui_window_add_vline (WINDOW * win, int first, int second, int size)
{
	if (win->vlines_allocated == win->vlines_count)
	{
		win->vlines_allocated += 16;
		win->vlines = (TENSION *) realloc(win->vlines, sizeof(TENSION) * win->vlines_allocated);
	}
	win->vlines[win->vlines_count].first = first;
	win->vlines[win->vlines_count].second = second;
	win->vlines[win->vlines_count].length = size;
	win->vlines_count ++;
}

/** Given a control, create vertical tension lines between the control
 *  and any control in the vertical proximity (on top). 
 *
 *	@param win   		Pointer to the window object
 *  @param c      		Number of control
 *  @returns			Number of tension lines created
 */

static int gui_window_create_vlines_for (WINDOW * win, int c)
{
	int line_count = 0;
	int i;
	int distance;
	REGION rect;

	rect.x  = win->control[c]->x;
	rect.y  = 0;
	rect.x2 = win->control[c]->x + win->control[c]->width - 1;
	rect.y2 = win->control[c]->y - 1;

	/* Find the nearest control in the area */

	for (i = 0 ; i < win->control_count ; i++)
	{
		if (i == c)
			continue;
		if (win->control[i]->hidden)
			continue;

		if (   win->control[i]->x + win->control[i]->width >= rect.x
			&& win->control[i]->x <= rect.x2
			&& win->control[i]->y + win->control[i]->height >= rect.y
			&& win->control[i]->y <= rect.y2 )
		{
			distance = rect.y2 - (win->control[i]->y + win->control[i]->height) + 1;
			if (distance <= 16)
			{
				gui_window_add_vline (win, i, c, distance);
				line_count++;
			}
		}
	}

	if (line_count == 0 && rect.y2 <= 16)
	{
		gui_window_add_vline (win, -1, c, rect.y);
		line_count++;
	}

	if (win->height - (win->control[c]->y + win->control[c]->height - 1) <= 16)
	{
		gui_window_add_vline (win, c, -1, win->height - (win->control[c]->y + win->control[c]->height - 1));
		line_count++;
	}

	return line_count;
}

/** Add an horizontal tension line to a window
 *
 *  @param win			Pointer to the window object
 *	@param first		First control, -1 for window border
 *	@param second		Second control, -1 for window border
 *	@param size			Size of the tension line in pixels
 */

static void gui_window_add_hline (WINDOW * win, int first, int second, int size)
{
	if (win->hlines_allocated == win->hlines_count)
	{
		win->hlines_allocated += 16;
		win->hlines = (TENSION *) realloc(win->hlines, sizeof(TENSION) * win->hlines_allocated);
	}
	win->hlines[win->hlines_count].first = first;
	win->hlines[win->hlines_count].second = second;
	win->hlines[win->hlines_count].length = size;
	win->hlines_count ++;
}

/** Given a control, create horizontal tension lines between the control
 *  and any control in the horizontal proximity (at left). 
 *
 *	@param win   		Pointer to the window object
 *  @param c      		Number of control
 *  @returns			Number of tension lines created
 */

static int gui_window_create_hlines_for (WINDOW * win, int c)
{
	int line_count = 0;
	int i;
	int distance;
	REGION rect;

	rect.y  = win->control[c]->y;
	rect.x  = 0;
	rect.y2 = win->control[c]->y + win->control[c]->height - 1;
	rect.x2 = win->control[c]->x - 1;

	/* Find the nearest control in the area */

	for (i = 0 ; i < win->control_count ; i++)
	{
		if (i == c)
			continue;
		if (win->control[i]->hidden)
			continue;

		if (   win->control[i]->x + win->control[i]->width >= rect.x
			&& win->control[i]->x <= rect.x2
			&& win->control[i]->y + win->control[i]->height >= rect.y
			&& win->control[i]->y <= rect.y2 )
		{
			distance = rect.x2 - (win->control[i]->x + win->control[i]->width) + 1;
			if (distance <= 32)
			{
				gui_window_add_hline (win, i, c, distance);
				line_count++;
			}
		}
	}

	if (line_count == 0 && rect.x2 <= 16)
	{
		gui_window_add_hline (win, -1, c, rect.x);
		line_count++;
	}

	if (win->width - (win->control[c]->x + win->control[c]->width - 1) <= 16)
	{
		gui_window_add_hline (win, c, -1, win->width - (win->control[c]->x + win->control[c]->width - 1));
		line_count++;
	}

	return line_count;
}

/** Create tension lines between controls
 *
 *	@param win   		Pointer to the window object
 **/

void gui_window_create_lines (WINDOW * win)
{
	int i;

	win->vlines_count = 0;
	for (i = 0 ; i < win->control_count ; i++)
	{
		win->control[i]->o_x = win->control[i]->x;
		win->control[i]->o_y = win->control[i]->y;
		win->control[i]->o_width = win->control[i]->width;
		win->control[i]->o_height = win->control[i]->height;
		if (!win->control[i]->hidden)
		{
			gui_window_create_vlines_for (win, i);
			gui_window_create_hlines_for (win, i);
		}
	}
	win->o_width = win->width;
	win->o_height = win->height;
}

/** Reset window and controls to the original size, before resizing
 *
 *	@param win			Pointer to the window object
 */

void gui_window_resetpos (WINDOW * win)
{
	int i;

	for (i = 0 ; i < win->control_count ; i++)
	{
		win->control[i]->x = win->control[i]->o_x;
		win->control[i]->y = win->control[i]->o_y;
		win->control[i]->width = win->control[i]->o_width;
		win->control[i]->height = win->control[i]->o_height;
		win->redraw = 1;
		(*win->control[i]->resized)(win->control[i]);
	}
	win->width = win->o_width;
	win->height = win->o_height;
}

/** Given a vertical tension line path, distributes the given displacement
 *  between the resizable controls in the path
 *
 *	@param win			Pointer to the window object
 *  @param size			Ammount of size to increment
 *	@param lines		Array of vertical line indexes
 *	@param line_count	Count of tension lines in the array
 *	@param solution		Array of size increments (1 int for each window control)
 *  @returns			1 if solution found, 0 otherwise
 */

int gui_window_resize_vline_path (WINDOW * win, int size, int * lines, int line_count, int * solution, int * adjustment)
{
	int resizable_count = 0;
	int i;
	int increment;
	int remainder;

	static int control_list[256] ;

	/* Create a list of resizable controls in the path. If any resizable control
	 * already has a solution, adjust the remaining size and ignore it */

	for (i = 0 ; i < line_count ; i++)
	{
		int c = win->vlines[lines[i]].first;
		if (c == -1)
			continue;
		if (win->control[c]->vresizable)
		{
			if (solution[c] != 0)
			{
				size -= solution[c];
				continue;
			}
			control_list[resizable_count] = c;
			resizable_count++;
			if (resizable_count == 256)
				return 0;
		}
	}

	/* There is already enought ammount distributed by previous solutions? */

	if (size == 0)
		return 1;

	/* There is no resizable controls? */

	if (resizable_count == 0)
		return 0;

	/* Create a valid solution */

	increment = size / resizable_count;
	remainder = size - (increment * resizable_count);

	/* Find controls that can't extend to the needed size */

	for (i = 0 ; i < resizable_count ; i++)
	{
		CONTROL * control = win->control[control_list[i]];

		if (control->max_height > 0 && control->max_height < control->height + increment + remainder)
		{
			/* The desired height exceeds the control's maximum */
			solution[control_list[i]] = control->max_height - control->height;
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = control->max_height - (control->height + increment + remainder);
		}
		else if (control->min_height > 0 && control->min_height > control->height + increment + remainder)
		{
			/* The desired height is less than the control's maximum */
			solution[control_list[i]] = -control->height + control->min_height;
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = control->min_height - (control->height + increment + remainder);
		}
		else if (control->height_step > 1)
		{
			/* The control needs a height multiple of some value */
			solution[control_list[i]] = increment - (increment % control->height_step);
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = -size;
		}
		else
			continue;

		/* The control is not suitable for resizing, or a suitable solution
		 * has been set. Throw it out of the resizable array list and calculate
		 * the remaining size. If there are remaining controls and a size to
		 * distribute, reset the loop and continue */

		if (i < resizable_count-1)
			memcpy (&control_list[i], &control_list[i+1],
			(resizable_count-i-1) * sizeof(int));
		resizable_count--;
		if (resizable_count == 0)
		{
			if (size == 0)
				return 1;
			return 0;
		}
		increment = size / resizable_count;
		remainder = size - (increment * resizable_count);
		i = -1;
	}

	/* Distribute the size */

	for (i = 0 ; i < resizable_count ; i++)
		solution[control_list[i]] = increment;
	solution[control_list[0]] += remainder;

	return 1;
}

/** Create or continue creating a tension line path tree
 *
 *  @returns			0 if the resizing is not possible
 */

int gui_window_vline_path (WINDOW * win, int * path, int * current, int vline, int size, int * solution, int * adjustment)
{
	int i;

	*current++ = vline;
	if (win->vlines[vline].first == -1)
		return gui_window_resize_vline_path (win, size, path, current-path, solution, adjustment);

	/* Find lines attaching to the same control */
	for (i = 0 ; i < win->vlines_count ; i++)
	{
		if (win->vlines[i].second == win->vlines[vline].first)
		{
			if (!gui_window_vline_path (win, path, current, i, size, solution, adjustment))
				return 0;
		}
	}

	/* The path "hangs" with no border contact */
	return 1;
}

/** Find a solution for a vertical resize action
 *
 *  @param win			Pointer to the window object
 *	@param size			Ammount to resize
 */

int gui_window_vresize (WINDOW * win, int size, int * adjustment)
{
	int * path = (int *)malloc(sizeof(int) * win->vlines_count);
	int * solution = (int *)malloc(sizeof(int) * win->control_count);
	int i;

	memset (solution, 0, sizeof(int) * win->control_count);
	*adjustment = 0;

	/* Find lines attaching to the bottom border */
	for (i = 0 ; i < win->vlines_count ; i++)
	{
		if (win->vlines[i].second == -1)
		{
			if (!gui_window_vline_path (win, path, path, i, size, solution, adjustment))
			{
				free (path);
				free (solution);
				return 0;
			}
		}
	}

	for (i = 0 ; i < win->control_count ; i++)
	{
		if (solution[i] != 0)
		{
			win->control[i]->height += solution[i];
			(*win->control[i]->resized)(win->control[i]);
		}
	}

	for (i = win->vlines_count-1 ; i >= 0 ; i--)
	{
		if (win->vlines[i].second == -1)
			win->control[win->vlines[i].first]->y = 
				win->height + size - 
				win->vlines[i].length -
				win->control[win->vlines[i].first]->height + 1;
		else if (win->vlines[i].first != -1)
			win->control[win->vlines[i].first]->y =
				win->control[win->vlines[i].second]->y -
				win->vlines[i].length -
				win->control[win->vlines[i].first]->height;
	}
	
	win->height += size;

	free (path);
	free (solution);
	return 1;
}

/** Given an horizontal tension line path, distributes the given displacement
 *  between the resizable controls in the path
 *
 *	@param win			Pointer to the window object
 *  @param size			Ammount of size to increment
 *	@param lines		Array of horizontal line indexes
 *	@param line_count	Count of tension lines in the array
 *	@param solution		Array of size increments (1 int for each window control)
 *  @returns			1 if solution found, 0 otherwise
 */

int gui_window_resize_hline_path (WINDOW * win, int size, int * lines, int line_count, int * solution, int * adjustment)
{
	int resizable_count = 0;
	int i;
	int increment;
	int remainder;

	static int control_list[256] ;

	/* Create a list of resizable controls in the path. If any resizable control
	 * already has a solution, adjust the remaining size and ignore it */

	for (i = 0 ; i < line_count ; i++)
	{
		int c = win->hlines[lines[i]].first;
		if (c == -1)
			continue;
		if (win->control[c]->hresizable)
		{
			if (solution[c] != 0)
			{
				size -= solution[c];
				continue;
			}			
			control_list[resizable_count] = c;
			resizable_count++;
			if (resizable_count == 256)
				return 0;
		}
	}

	/* There is already enought ammount distributed by previous solutions? */

	if (size == 0)
		return 1;

	/* There is no resizable controls? */

	if (resizable_count == 0)
		return 0;

	/* Create a valid solution */

	increment = size / resizable_count;
	remainder = size - (increment * resizable_count);

	/* Find controls that can't extend to the needed size */

	for (i = 0 ; i < resizable_count ; i++)
	{
		CONTROL * control = win->control[control_list[i]];

		if (control->max_width > 0 && control->max_width < control->width + increment + remainder)
		{
			/* The desired width exceeds the control's maximum */
			solution[control_list[i]] = control->max_width - control->width;
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = control->max_width - (control->width + increment + remainder);
		}
		else if (control->min_width > 0 && control->min_width > control->width + increment + remainder)
		{
			/* The desired height is less than the control's maximum */
			solution[control_list[i]] = -control->width + control->min_width;
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = control->min_width - (control->width + increment + remainder);
		}
		else if (control->width_step > 1)
		{
			/* The control needs a height multiple of some value */
			solution[control_list[i]] = increment - (increment % control->width_step);
			size -= solution[control_list[i]];
			if (resizable_count == 1 && size != 0)
				*adjustment = -size;
		}
		else
			continue;

		/* The control is not suitable for resizing, or a suitable solution
		 * has been set. Throw it out of the resizable array list and calculate
		 * the remaining size. If there are remaining controls and a size to
		 * distribute, reset the loop and continue */

		if (i < resizable_count-1)
			memcpy (&control_list[i], &control_list[i+1],
			(resizable_count-i-1) * sizeof(int));
		resizable_count--;
		if (resizable_count == 0)
		{
			if (size == 0)
				return 1;
			return 0;
		}
		increment = size / resizable_count;
		remainder = size - (increment * resizable_count);
		i = -1;
	}

	/* Distribute the size */

	for (i = 0 ; i < resizable_count ; i++)
		solution[control_list[i]] = increment;
	solution[control_list[0]] += remainder;

	return 1;
}

/** Create or continue creating a tension line path tree
 *
 *  @returns			0 if the resizing is not possible
 */

int gui_window_hline_path (WINDOW * win, int * path, int * current, int hline, int size, int * solution, int * adjustment)
{
	int i;

	*current++ = hline;
	if (win->hlines[hline].first == -1)
		return gui_window_resize_hline_path (win, size, path, current-path, solution, adjustment);

	/* Find lines attaching to the same control */
	for (i = 0 ; i < win->hlines_count ; i++)
	{
		if (win->hlines[i].second == win->hlines[hline].first)
		{
			if (!gui_window_hline_path (win, path, current, i, size, solution, adjustment))
				return 0;
		}
	}

	/* The path "hangs" with no border contact */
	return 1;
}

/** Find a solution for an horizontal resize action
 *
 *  @param win			Pointer to the window object
 *	@param size			Ammount to resize
 */

int gui_window_hresize (WINDOW * win, int size, int * adjustment)
{
	int * path = (int *)malloc(sizeof(int) * win->hlines_count);
	int * solution = (int *)malloc(sizeof(int) * win->control_count);
	int i;

	memset (solution, 0, sizeof(int) * win->control_count);
	*adjustment = 0;

	/* Find lines attaching to the bottom border */
	for (i = 0 ; i < win->hlines_count ; i++)
	{
		if (win->hlines[i].second == -1)
		{
			if (!gui_window_hline_path (win, path, path, i, size, solution, adjustment))
			{
				free (path);
				free (solution);
				return 0;
			}
		}
	}

	/* Adjust the controls */
	for (i = 0 ; i < win->control_count ; i++)
	{
		if (solution[i] != 0)
		{
			win->control[i]->width += solution[i];
			(*win->control[i]->resized)(win->control[i]);
		}
	}

	for (i = win->hlines_count-1 ; i >= 0 ; i--)
	{
		if (win->hlines[i].second == -1)
			win->control[win->hlines[i].first]->x = 
				win->width + size - 
				win->hlines[i].length -
				win->control[win->hlines[i].first]->width + 1;
		else if (win->hlines[i].first != -1)
			win->control[win->hlines[i].first]->x =
				win->control[win->hlines[i].second]->x -
				win->hlines[i].length -
				win->control[win->hlines[i].first]->width;
	}
	
	win->width += size;

	free (path);
	free (solution);
	return 1;
}

/** Resize a given window
 *
 *	@param window			Pointer to the window object
 *	@param width			New width of the window (borders included)
 *	@param height			New height of the window (borders and caption included)
 **/

void gui_window_resize (WINDOW * window, int width, int height)
{
	int adjustment = 0;

	gr_mark_rect (window->x, window->y, window->width+3, window->caption + window->height+3);
	window->redraw = 1;

	/* Clamp the values to acceptable minimums and maximums */
	if (width < window->border*2 + 8)
		width = window->border*2 + 8;
	if (height < window->border*2 + window->caption + 8)
	    height = window->border*2 + window->caption + 8;
	if (width > scr_width && window->x >= 0)
		width = scr_width;
	if (height > scr_height && window->y >= 0)
		height = scr_height;

	/* Study the control layout of the window */
	if (window->dirty)
	{
		gui_window_create_lines(window);
		window->dirty = 0;
	}

	/* Set the control positions to the moment of the previous study */
	gui_window_resetpos (window);

	/* Call the resizing functions. If the new dimensions are not valid,
	 * try again after some adjustment */

	if (height != window->height)
	{
		while (!gui_window_vresize (window, height - window->height, &adjustment))
		{
			if (adjustment == 0)
				break;
			height += adjustment;
		}
	}
	if (width != window->width)
	{
		while (!gui_window_hresize (window, width - window->width, &adjustment))
		{
			if (adjustment == 0)
				break;
			width += adjustment;
		}
	}

	return;
}

/** Guess which mouse cursor to use at a given position
 *
 *	@param window		Pointer to the window object
 *	@param x			Mouse pointer coordinate
 *	@param y			Mouse pointer coordinate
 *	@returns			One of the POINTER_XXX constants
 */

int gui_window_mousepointer (WINDOW * window, int x, int y)
{
	CONTROL * control;

	if (window->drag & DRAG_RESIZEH)
	{
		if (window->drag & DRAG_RESIZEV)
			return POINTER_RESIZENW;
		return POINTER_RESIZEH;
	}
	if (window->drag & DRAG_RESIZEV)
		return POINTER_RESIZEV;
	if (window->control_drag)
		control = window->control_drag;
	else
	{
		if (!gui_desktop_dragging())
		{
			if (window->resizable && !window->maximized && x > window->width - window->border - 2)
			{
				if (y > window->caption + window->height - window->border)
					return POINTER_RESIZENW;
				return POINTER_RESIZEH;
			}
			if (window->resizable && !window->maximized && y > window->caption + window->height - window->border)
				return POINTER_RESIZEV;
		}
		control = gui_window_findcontrol (window, x, y);
	}
	if (!control)
		return POINTER_ARROW;
	return (*control->mousepointer)
		(control, x - control->x, y - window->caption - control->y);
}

/** Draw the shadow of a window. The shadow is currently hard-coded as 3 pixels wide.
 *	Note that the shadow is traslucent: the desktop below the window MUST be drawn
 *	under the shadow area before calling this function.
 *
 *	@param w		Window to draw the shadow of
 *	@param dest		Destination bitmap
 *	@param clip		Clipping region (not required)
 */

void gui_window_draw_shadow (WINDOW * w, GRAPH * dest, REGION * clip)
{
	/* Draw a shadow */
	if (w->shadow && !w->maximized)
	{
		gr_setcolor (0);
		gr_setalpha (128);
		gr_hline    (dest, clip, w->x+6, w->y+w->height+w->caption,   w->width-6);
		gr_vline    (dest, clip, w->x+w->width,   w->y+6, w->height+w->caption-5);
		gr_setalpha (64);
		gr_hline    (dest, clip, w->x+5, w->y+w->height+w->caption,   1);
		gr_hline    (dest, clip, w->x+5, w->y+w->height+w->caption+1, w->width-4);
		gr_vline    (dest, clip, w->x+w->width,   w->y+5, 1);
		gr_vline    (dest, clip, w->x+w->width+1, w->y+5, w->height+w->caption-3);
		gr_setalpha (20);
		gr_hline    (dest, clip, w->x+4, w->y+w->height+w->caption,   1);
		gr_hline    (dest, clip, w->x+4, w->y+w->height+w->caption+1, 1);
		gr_hline    (dest, clip, w->x+4, w->y+w->height+w->caption+2, w->width-2);
		gr_vline    (dest, clip, w->x+w->width,   w->y+4, 1);
		gr_vline    (dest, clip, w->x+w->width+1, w->y+4, 1);
		gr_vline    (dest, clip, w->x+w->width+2, w->y+4, w->height+w->caption-1);
		gr_setalpha (255);
	}
}

/** Guive the focus to some control at the window
 *
 *	@param window		Pointer to the window object
 *	@param control		Pointer to the control object
 */

void gui_window_setfocus (WINDOW * window, CONTROL * control)
{
	if (control && window && control->window == window && window->focus != control)
	{
		if (window->focus)
		{
			(*window->focus->leave)(window->focus);
			window->focus->actionflags |= ACTION_LEAVE;
		}
		window->focus = control;
		(*control->enter)(control);
		control->actionflags |= ACTION_ENTER;
	}
}

/** Capture all mouse movement by a control 
 *
 *	@param window		Parent window, with the control
 *	@param control		Control
 */

void gui_window_capturemouse (WINDOW * window, CONTROL * control)
{
	gui_desktop_capturemouse (window);
	window->captured_mouse = control;
}

/** Release a mouse capture by a control
 *
 *	@param window		Parent window, with the control
 *	@param control		Control with the mouse captured
 */

void gui_window_releasemouse (WINDOW * window, CONTROL * control)
{
	if (window->captured_mouse == control)
	{
		gui_desktop_releasemouse (window);
		window->captured_mouse = 0;
	}
}
