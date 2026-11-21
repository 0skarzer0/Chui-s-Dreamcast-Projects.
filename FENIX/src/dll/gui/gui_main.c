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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** @file
 *
 *  Main DLL functions and desktop management
 */

static int default_pointer = POINTER_ARROW;
static int buttons = 0;
static int last_clickx = -1;
static int last_clicky = -1;
static int last_clickbutton = -1;
static int last_x = -1;
static int last_y = -1;
static int last_clicktime = -1;
static int dragdrop = 0;
static int dropped = 0;
static int dragdrop_string = 0;
static GRAPH * dragdrop_graph = NULL;
static CONTROL * dragdrop_destination = NULL;
static int dragdrop_atdesktop = 0;
static int dragdrop_accepted = 0;
static int shadow_region = 0;

static const char * tooltip = 0;
static long tooltip_time = 0;

#define DOUBLECLICK_THRESOLD 500
#define TOOLTIP_TIME 500

int color_highlight;
int color_shadow;
int color_tablebg;
int color_input;
int color_face;
int color_highlightface;
int color_text;
int color_border;
int color_window;
int color_selection;
int color_captionfg;
int color_captionbg;

static WINDOW * * 	window = 0;
static int			window_allocated = 0;
static int			window_count = 0;
static WINDOW *     lastclick_window = 0;

/* Array of windows where the user pressed each button */
static WINDOW *     clicked_window[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static WINDOW *     mouse_captured_window = 0;

typedef struct _popup
{
	CONTROL *		control;
	WINDOW *		window;
	int				x, y;
	struct _popup * next;
	struct _popup * prev;
}
POPUP;

static POPUP *		popup = 0;

static POPUP * gui_desktop_findpopup (int x, int y);

/** Calculate the maximize window area (that is, the area of the full screen,
 *	minus any docked window that are visible). This function also recalculates
 *  any docked window's position.
 *
 *	@param region	Pointer to a region that will be filled with the output values
 */

void gui_desktop_visibleregion (REGION * r)
{
	int i;

	r->x  = 0;
	r->x2 = scr_width-1;
	r->y  = 0;
	r->y2 = scr_height-1;

	for (i = 0 ; i < window_count ; i++)
	{
		switch (window[i]->dock)
		{
			case STYLE_DOCK_NORTH:
				window[i]->y = r->y;
				window[i]->x = r->x;
				gui_window_resize (window[i], r->x2 - r->x + 1, window[i]->height);
				window[i]->width = r->x2 - r->x + 1;
				r->y += window[i]->height;
				break;

			case STYLE_DOCK_SOUTH:
				window[i]->y = r->y2 - window[i]->height + 1;
				window[i]->x = r->x;
				gui_window_resize (window[i], r->x2 - r->x + 1, window[i]->height);
				window[i]->width = r->x2 - r->x + 1;
				r->y2 -= window[i]->height;
				break;

			case STYLE_DOCK_WEST:
				window[i]->x = r->x;
				window[i]->y = r->y;
				window[i]->height = r->y2 - r->y + 1;
				r->x += window[i]->width;
				break;

			case STYLE_DOCK_EAST:
				window[i]->x = r->x2 - window[i]->width;
				window[i]->y = r->y;
				window[i]->height = r->y2 - r->y + 1;
				r->x2 -= window[i]->width;
				break;
		}
	}
}

/** Recalculate any docked window's position and adjust the dimensions of the
 *  maximized window (if any).
 */

void gui_desktop_adjustdock()
{
	int i;
	REGION visible;

	gui_desktop_visibleregion(&visible);

	for (i = 0 ; i < window_count ; i++)
	{
		if (window[i]->maximized && window[i]->fullscreen)
		{
			for (i = 0 ; i < window_count ; i++)
			{
				if (window[i]->dock)
					window[i]->visible = 0;
			}
			break;
		}
		if (window[i]->dock)
			window[i]->visible = 1;
		if (window[i]->maximized)
		{
			int visible_width = visible.x2 - visible.x + 1;
			int visible_height = visible.y2 - visible.y + 1;
			gui_window_resize (window[i], visible_width, visible_height);
			gui_window_moveto (window[i], 
				visible_width - window[i]->width + visible.x, 
				visible.y - window[i]->caption);
		}
	}
}

/** Count how many windows have a portion in the given region
 *
 *  @param region		Region in screen pixel coordinates
 *  @returns			Sum of the number of pixels ocupped by every window in the area
 */

int gui_desktop_countwindows (REGION * region)
{
	int i;
	int count = 0;
	int border_x = (region->x2 - region->x)/15;
	int border_y = (region->y2 - region->y)/15;

	for (i = 0 ; i < window_count ; i++)
	{
		if (window[i]->dock == 0 && window[i]->visible && window[i]->x != -1)
		{
			if (window[i]->x <= region->x2 - border_x &&
				window[i]->y <= region->y2 - border_y &&
				window[i]->x + window[i]->width-1 >= region->x  + border_x &&
				window[i]->y + window[i]->height + window[i]->caption-1 >= region->y + border_y)
			{
				REGION inner;

				inner.x = window[i]->x;
				inner.y = window[i]->y;
				inner.x2 = inner.x + window[i]->width;
				inner.y2 = inner.y + window[i]->height + window[i]->caption;
				region_union (&inner, region);

				count += (inner.x2 - inner.x + 1) * (inner.y2 - inner.y + 1);
			}
		}
	}
	return count;
}

/** Adjust the window position so it does not caught out of the given region
 */

void gui_desktop_fitwindow (WINDOW * window, REGION * region)
{
	if (window->x + window->width > region->x2)
		window->x = region->x2 - window->width;
	if (window->y + window->height + window->caption > region->y2)
		window->y = region->y2 - window->height - window->caption;
	if (window->x < region->x)
		window->x = region->x;
	if (window->y < region->y)
		window->y = region->y;
}

/** Find a suitable screen position for a new window
 */

void gui_desktop_allocatewindow (WINDOW * win)
{
	REGION visible;
	REGION square[4];
	int count[4];
	int width, height;
	int border_x, border_y;

	gui_desktop_visibleregion(&visible);
	width = (visible.x2 - visible.x + 1)/2;
	height = (visible.y2 - visible.y + 1)/2;

	square[0].x = visible.x;
	square[0].y = visible.y;
	square[0].x2 = visible.x + width;
	square[0].y2 = visible.y + height;
	count [0] = gui_desktop_countwindows (&square[0]);

	square[1].x = visible.x + width;
	square[1].y = visible.y;
	square[1].x2 = visible.x2;
	square[1].y2 = visible.y + height;
	count [1] = gui_desktop_countwindows (&square[1]);

	square[2].x = visible.x;
	square[2].y = visible.y + height;
	square[2].x2 = visible.x + width;
	square[2].y2 = visible.y2;
	count [2] = gui_desktop_countwindows (&square[2]);

	square[3].x = visible.x + width;
	square[3].y = visible.y + height;
	square[3].x2 = visible.x2;
	square[3].y2 = visible.y2;
	count [3] = gui_desktop_countwindows (&square[3]);

	if (!count[0] && !count[1] && !count[2] && !count[3])
	{
		win->x = (scr_width - win->width)/2;
		win->y = (scr_height - win->height - win->caption)/2;
	}
	else if (count[0] <= count[1] && count[0] <= count[2] && count[0] <= count[3])
	{
		win->x = square[0].x + (square[0].x2 - square[0].x - win->width) / 2;
		win->y = square[0].y + (square[0].y2 - square[0].y - win->height - win->caption) / 2;
	}
	else if (count[3] <= count[1] && count[3] <= count[2] && count[3] <= count[0])
	{
		win->x = square[3].x + (square[3].x2 - square[3].x - win->width) / 2;
		win->y = square[3].y + (square[3].y2 - square[3].y - win->height - win->caption) / 2;
	}
	else if (count[1] <= count[0] && count[1] <= count[2] && count[1] <= count[3])
	{
		win->x = square[1].x + (square[1].x2 - square[1].x - win->width) / 2;
		win->y = square[1].y + (square[1].y2 - square[1].y - win->height - win->caption) / 2;
	}
	else if (count[2] <= count[1] && count[2] <= count[0] && count[2] <= count[3])
	{
		win->x = square[2].x + (square[2].x2 - square[2].x - win->width) / 2;
		win->y = square[2].y + (square[2].y2 - square[2].y - win->height - win->caption) / 2;
	}
	else
	{
		win->x = (scr_width - win->width)/2;
		win->y = (scr_height - win->height - win->caption)/2;
	}

	border_x = (visible.x2 - visible.x)/64;
	border_y = (visible.y2 - visible.y)/48;
	visible.x += border_x;
	visible.y += border_y;
	visible.x2 -= border_x;
	visible.y2 -= border_y;
	gui_desktop_fitwindow (win, &visible);
}

/** Add a new window to the desktop (make the window visible)
 *
 *  @param window	Pointer to the WINDOW object
 */

void gui_desktop_addwindow (WINDOW * win)
{
	int i;

	for (i = 0 ; i < window_count ; i++)
	{
		if (window[i] == win)
		{
			win->visible = 1;
			win->redraw = 1;
			gui_desktop_bringtofront(win);
			return;
		}
	}

	gui_full_redraw = 1;

	/* Allocate memory for the window if needed */
	if (window_allocated == window_count)
	{
		WINDOW * * new_window = (WINDOW * *) realloc(window, 
				sizeof(WINDOW * *) * (window_allocated + 16));
		if (new_window == NULL)
			return;
		window = new_window;
		window_allocated += 16;
	}

	/* Add the window to the list */
	window[window_count++] = win;

	/* Set the window position */
	if (win->dock)
	{
		gui_desktop_adjustdock();
	}
	else if (win->x == -1 && win->y == -1)
	{
		/* Modals windows are centered; else, use the allocation algorithm */

		if (win->modal)
		{
			win->x = (scr_width - win->width)/2;
			win->y = (scr_height - win->height - win->caption)/2;
		}
		else
			gui_desktop_allocatewindow (win);
	}

	if (win->dontfocus)
	{
		if (window_count > 1)
			gui_desktop_bringtofront (window[window_count-2]);
	}
	else
	{
		/* If there was any window here, it has lost the focus */
		if (window_count > 1)
			gui_window_leave (window[window_count-2]);
	
		/* The new window gets the focus */
		gui_window_enter (window[window_count-1]);
	}
}

/** Delete a window from the desktop, if it is there
 *
 *  @param window	Pointer to the window object
 */

void gui_desktop_removewindow (WINDOW * win)
{
	int i;
	POPUP * p;

	if (lastclick_window == win)
		lastclick_window = 0;

	gr_mark_rect (win->x, win->y, win->width, win->height + win->caption);

	/* Remove the window from the internal list */
	for (i = 0 ; i < window_count ; i++)
	{
		if (window[i] == win)
		{
			if (i == window_count-1)
				gui_window_leave (win);
			else 
			{
				memmove (&window[i], &window[i+1],
						sizeof(WINDOW * *) * (window_count-i-1));
			}
			gui_full_redraw = 1;
			window_count--;
			if (i == window_count)
				gui_window_enter (window[window_count-1]);
			break;
		}
	}

	/* Remove any popup if it is from the window */
	for (p = popup ; p ; )
	{
		if (p->window == win)
		{
			CONTROL * control = p->control;
			p = p->next;
			gui_desktop_removepopup(control);
		}
		else
			p = p->next;
	}
}

/** Calculates if a window intersects (touches) a screen region
 *
 *	@param w			Window
 *	@param region		Screen rectangle (in screen coordinates)
 *	@returns			1 if the windows has at least 1 pixel inside the rectangle
 */

int gui_window_intersects (WINDOW * w, REGION * region)
{
	int shadow_size = (w->shadow ? 3 : 0);

	if (w->x <= region->x2 && w->y <= region->y2 &&
		w->x + w->width + shadow_size > region->x &&
		w->y + w->caption + w->height + shadow_size > region->y)
	{
		if (w->maximized && w->y + w->caption > region->y2)
			return 0;
		return 1;
	}
	return 0;
}

/** Draws a window and, recursively, all the windows around it. Draws only windows 
 *  below the given one, if provided, or draw any window otherwise. This feature
 *  is used to draw the desktop under a transparent window, but this same function
 *  is also used in normal drawing procedures.
 *
 *  @param w			Window to draw (required)
 *	@param dest			Destination bitmap
 *	@param clip			Clipping region (required, but may be the whole screen)
 *	@param below_this	Only draw windows below this one (optional, 0 to ignore)
 */

void gui_desktop_draw_around (WINDOW * w, GRAPH * dest, REGION * clip, WINDOW * below_this)
{
	int shadow_size = (w->shadow ? 3 : 0);
	REGION zone;

	/* If the window is transparent, draw the desktop below first, and
	 * then draw the window contents on top */

	if (w->transparent || w->alpha != 255)
	{
		zone.x = w->x;
		zone.y = w->y;
		zone.x2 = w->x + w->width - 1;
		zone.y2 = w->y + w->height + w->caption - 1;
		region_union (&zone, clip);
		if (!region_is_empty(&zone))
			gui_desktop_draw_region_below (dest, &zone, w);
	}

	/* Draw the window */

	gui_window_draw (w, dest, clip);

	/* Draw the window shadow, if any */

	if (shadow_size)
	{
		/* Shadow right */
		zone.x = w->x + w->width;
		zone.x2 = zone.x + 2;
		zone.y = w->y;
		zone.y2 = w->y + w->height + w->caption + 2;
		region_union (&zone, clip);
		if (!region_is_empty (&zone))
			gui_desktop_draw_region_below (dest, &zone, w);

		/* Shadow bottom */
		zone.x = w->x;
		zone.x2 = w->x + w->width - 1;
		zone.y = w->y + w->height + w->caption;
		zone.y2 = zone.y + 2;
		region_union (&zone, clip);
		if (!region_is_empty (&zone))
			gui_desktop_draw_region_below (dest, &zone, w);

		/* Shadow */
		gui_window_draw_shadow (w, dest, clip);
	}

	/* Left Zone */
	zone.x = 0;
	zone.x2 = w->x-1;
	zone.y = 0;
	zone.y2 = scrbitmap->height - 1;
	region_union (&zone, clip);
	if (!region_is_empty (&zone))
	{
		if (below_this)
			gui_desktop_draw_region_below (dest, &zone, below_this);
		else
			gui_desktop_draw_region (dest, &zone);
	}

	/* Right zone */
	zone.x = w->x + w->width + shadow_size;
	zone.x2 = scrbitmap->width - 1;
	zone.y = 0;
	zone.y2 = scrbitmap->height - 1;
	region_union (&zone, clip);
	if (!region_is_empty (&zone))
	{
		if (below_this)
			gui_desktop_draw_region_below (dest, &zone, below_this);
		else
			gui_desktop_draw_region (dest, &zone);
	}

	/* Top zone */
	zone.x = w->x;
	zone.x2 = w->x + w->width - 1 + shadow_size;
	zone.y = 0;
	zone.y2 = w->y - 1;
	if (w->maximized)
		zone.y2 += w->caption;
	region_union (&zone, clip);
	if (!region_is_empty (&zone))
	{
		if (below_this)
			gui_desktop_draw_region_below (dest, &zone, below_this);
		else
			gui_desktop_draw_region (dest, &zone);
	}

	/* Bottom zone */
	zone.x = w->x;
	zone.x2 = w->x + w->width - 1 + shadow_size;
	zone.y = w->y + w->height + w->caption + shadow_size;
	zone.y2 = scrbitmap->height - 1;
	region_union (&zone, clip);
	if (!region_is_empty (&zone))
	{
		if (below_this)
			gui_desktop_draw_region_below (dest, &zone, below_this);
		else
			gui_desktop_draw_region (dest, &zone);
	}

	/* Modal windows shadow the entire desktop */
	if (w->modal && gui_desktop_lastwindow() == w && !w->maximized)
	{
		gr_setalpha (64);
		gr_setcolor (0);
		gr_box (dest, clip, 0, 0, scrbitmap->width, w->y);
		gr_box (dest, clip, 0, w->y, w->x, w->height+w->caption);
		gr_box (dest, clip, w->x+w->width, w->y, scrbitmap->width, w->height+w->caption);
		gr_box (dest, clip, 0, w->y+w->caption+w->height, scrbitmap->width, scrbitmap->height-1);
		gr_setalpha(255);
	}
}

/** Draw all the windows below the given one, limited to a given region.
 *  This is used to draw transparent windows and shadows.
 *
 *  @param dest		Destination graphic (usually the background or screen bitmap)
 *	@param region	Clipping region (required, but may be the whole screen)
 *	@param win		Only draw windows below this one
 */

void gui_desktop_draw_region_below (GRAPH * dest, REGION * region, WINDOW * win)
{
	int i;
	int found = 0;

	if (win->stayontop)
	for (i = window_count-1 ; i >= 0 ; i--)
	{
		assert (window[i] != NULL);
		if (win == window[i])
		{
			found = 1;
			continue;
		}
		if (!found)
			continue;

		if (!window[i]->visible)
			continue;
		if (window[i]->stayontop)
		{
			if (!gui_window_intersects (window[i], region))
				continue;

			//gui_window_draw (window[i], dest, region);
			gui_desktop_draw_around (window[i], dest, region, win);
			break;
		}
	}

	for (i = window_count-1 ; i >= 0 ; i--)
	{
		if (win == window[i])
		{
			found = 1;
			continue;
		}
		if (!found)
			continue;
		if (!window[i]->visible)
			continue;
		if (!window[i]->stayontop)
		{
			if (!gui_window_intersects (window[i], region))
				continue;

			gui_desktop_draw_around (window[i], dest, region, win);
			break;
		}
	}
}

/** Draw the desktop (all the windows, in correct order), but limits drawing
 *  operations to the clipping region
 *
 *  @param dest		Destination graphic (usually the background or screen bitmap)
 */

void gui_desktop_draw_region (GRAPH * dest, REGION * region)
{
	int i, j;
	int shadow_region_old;
	int done = 0;

	/* Find stay-on-top windows first. Note that this function only searches
	 * the top-most window touching the given region, and uses gui_desktop_draw_around
	 * to do all the dirty work */

	for (i = window_count-1 ; i >= 0 ; i--)
	{
		assert (window[i] != NULL);
		if (!window[i]->visible)
			continue;
		if (window[i]->stayontop)
		{
			if (!gui_window_intersects (window[i], region))
			{
				if (!window[i]->maximized && window[i]->modal && gui_desktop_lastwindow() == window[i])
					shadow_region = 1;
				continue;
			}

			shadow_region_old = shadow_region;
			gui_desktop_draw_around (window[i], dest, region, 0);
			shadow_region = shadow_region_old;

			if (window[i]->dock == STYLE_DOCK_NORTH && window[i]->y <= 0)
			{
				/* Hack: show maximize/close buttons of the first maximized
				   visible window through the menu bar or equivalent window */

				for (j = window_count-1 ; j >= 0 ; j--)
				{
					if (window[j]->maximized && window[j]->visible)
					{
						WINDOW * w = window[j];

						if (w->close)
						{
							w->close->focused = 0;
							w->close->y = -w->y-w->caption+3;
							w->close->x = w->width - w->close->width - 4;
							(*w->close->draw) (w->close, dest, w->x + w->close->x,
								w->y + w->close->y + w->caption, 0);
						}
						if (w->maximize)
						{
							w->maximize->focused = 0;
							w->maximize->y = -w->y-w->caption+3;
							w->maximize->x = w->close->x - w->maximize->width - 6;
							(*w->maximize->draw) (w->maximize, dest, w->x + w->maximize->x,
								w->y + w->maximize->y + w->caption, 0);
						}
						if (w->minimize)
						{
							w->minimize->focused = 0;
							w->minimize->y = -w->y-w->caption+3;
							w->minimize->x = (w->maximize ? w->maximize->x : w->close->x - 6) 
								- w->minimize->width;
							(*w->minimize->draw) (w->minimize, dest, w->x + w->minimize->x,
								w->y + w->minimize->y + w->caption, 0);
						}

					} /* if (MAXIMIZED && VISIBLE) */
				} /* for (...) */
			} /* if (DOCK) */
				
			done = 1;
			break;
		}
	}

	/* Find normal windows next */

	if (!done)
	{
		for (i = window_count-1 ; i >= 0 ; i--)
		{
			if (!window[i]->visible)
				continue;
			if (!window[i]->stayontop)
			{
				if (!gui_window_intersects (window[i], region))
				{
					if (!window[i]->maximized && window[i]->modal && gui_desktop_lastwindow() == window[i])
						shadow_region = 1;
					continue;
				}

				shadow_region_old = shadow_region;
				gui_desktop_draw_around (window[i], dest, region, 0);
				shadow_region = shadow_region_old;
				break;
			}
		}
	}
}

/** Draw the desktop (all the windows, respecting its order)
 *
 *  @param dest		Destination graphic (usually the background or screen bitmap)
 */

void gui_desktop_draw (GRAPH * dest, REGION * clip)
{
	REGION region;
	POPUP * p;

	region.x = 0;
	region.y = 0;
	region.x2 = dest->width-1;
	region.y2 = dest->height-1;
	if (clip)
		region_union (&region, clip);

	shadow_region = 0;

	gui_desktop_draw_region (dest, &region);

	/* A modal window was found outside the drawing region:
	   all of the region should be shadowed */

	if (shadow_region)
	{
		gr_setalpha (64);
		gr_setcolor (0);
		gr_box (dest, &region, 0, 0, scrbitmap->width, scrbitmap->height);
		gr_setalpha(255);
	}

	/* Show popups */
	p = popup;
	while (p && p->next)
		p = p->next;
	while (p)
	{
		(*p->control->draw)(p->control, dest, p->x, p->y, clip);
		p = p->prev;
	}

	/* Show the tooltip */
	if (tooltip && tooltip_time < gr_timer() - TOOLTIP_TIME)
	{
		int width = gui_text_width (tooltip);
		int height = gui_text_height (tooltip, 2);
		int tx = last_x + 8;
		int ty = last_y + 20;

		if (tx + width/2 > scrbitmap->width - 4)
			tx = scrbitmap->width - 4 - width/2;
		if (ty + height > scrbitmap->height - 2)
			ty = scrbitmap->height - 2 - height;
		if (tx < width/2 + 2)
			tx = width/2 + 2;
		if (ty < 1)
			ty = 1;
		
		gr_setalpha (64);
		gr_setcolor (0);
		gr_box (dest, clip, tx - width/2 - 2, ty - 2, width + 7, height + 6);
		gr_setalpha (255);
		gr_setcolor (color_highlight);
		gr_box (dest, clip, tx - width/2 - 4, ty - 4, width + 7, height + 6);
		gr_setcolor (color_border);
		gr_rectangle (dest, clip, tx - width/2 - 4, ty - 4, width + 7, height + 6);
		gui_text_put (dest, clip, tx, ty-1, tooltip, 1, 2);
	}

	/* Show dragging icon */
	if (dragdrop)
	{
		GRAPH * graph = dragdrop_graph;
		if (!graph) 
			graph = cursors[POINTER_DRAGOBJECT];
		if (!dragdrop_accepted)
			graph = cursors[POINTER_BADDROP];
		if (graph)  
			gui_scale_image (dest, last_x-32, last_y-32, last_x+32, last_y+32, 4, clip, graph);
		dragdrop_accepted = 0;
	}

	/* Drag/drop mode inactive, but drop flag on... Drop the flag safely  now */
	if (!dragdrop && dropped)
		dropped--;
}

/** Find the window under a given pixel coordinate
 *
 *  @param x		X coordinate
 *  @param y		Y coordinate
 *  @return			Pointer to the window in this location or NULL if there is none
 */

WINDOW * gui_desktop_findwindow (int x, int y)
{
	int i ;

	for (i = window_count - 1 ; i >= 0 ; i--)
	{
		assert (window[i] != NULL);

		if (!window[i]->visible)
			continue;
		if (window[i]->stayontop && 
			(!window[i]->dock || x < scr_width - 120) &&
			window[i]->x <= x && window[i]->y <= y &&
			window[i]->width + window[i]->x - 1 >= x &&
			window[i]->height + window[i]->caption + window[i]->y - 1 >= y)
			return window[i];
	}

	for (i = window_count - 1 ; i >= 0 ; i--)
	{
		assert (window[i] != NULL);

		if (!window[i]->visible)
			continue;
		if (window[i]->x <= x && window[i]->y <= y &&
			window[i]->width + window[i]->x - 1 >= x &&
			window[i]->height + window[i]->caption + window[i]->y - 1 >= y)
			return window[i];
	}
	return NULL;
}

/** Move a window to the top
 *
 *  @param window	Pointer to the window object
 */

void gui_desktop_bringtofront (WINDOW * w)
{
	int i;

	w->visible = 1;
	w->redraw = 1;

	if (window_count < 1)
		return;
	if (window[window_count-1] == w)
	{
		gui_window_enter(w);
		return;
	}
	if (w->dontfocus)
		return;

	if (window_count > 0)
		gui_window_leave (window[window_count-1]);

	for (i = 0 ; i < window_count - 1 ; i++)
	{
		if (window[i] == w)
		{
			memmove (&window[i], &window[i+1], sizeof(WINDOW*) * (window_count-i-1)) ;
			window[window_count-1] = w;
			break;
		}
	}

	if (window_count > 0)
		gui_window_enter (window[window_count-1]);
}

/** Drop a window to the background
 *
 *  @param window	Pointer to the window object
 */

void gui_desktop_sendtoback (WINDOW * w)
{
	int i;

	for (i = 1 ; i < window_count ; i++)
	{
		if (window[i] == w)
		{
			if (window_count == i+1)
				gui_window_leave (w);
			memmove (&window[1], &window[0], sizeof(WINDOW *) * i);
			window[0] = w;
 			if (window_count == i+1)
			{
				for (; i >= 0 ; i--)
					if (!window[i]->dontfocus && window[i]->visible)
						break;
				if (i >= 0)
					gui_desktop_bringtofront(window[i]);
			}
			break;
		}
	}
}

/** Propagate a mouse move event
 *
 *  @param x		Mouse X coordinate
 *  @param y		Mouse Y coordinate
 */

void gui_desktop_mousemove (int x, int y)
{
	POPUP  * this_p = NULL;
	POPUP  * last_p = NULL;
	WINDOW * last_one = NULL;
	WINDOW * this_one = NULL;
	int      i, j;

	/* Mouse is captured? */
	if (mouse_captured_window)
	{
		for (i = 0 ; i < window_count ; i++)
		{
			if (window[i] == mouse_captured_window)
			{
				gui_window_mousemove (window[i], x - window[i]->x, y - window[i]->y, buttons);
				last_x = x;
				last_y = y;
				return;
			}
		}
		mouse_captured_window = 0;
	}

	/* Drag/drop object mode */
	if (dragdrop)
	{
		if (lastclick_window)
		{
			gui_window_mouseleave (lastclick_window);
			lastclick_window = 0;
		}
		last_x = x;
		last_y = y;
		dragdrop_atdesktop = 0;
		dragdrop_destination = NULL;
		this_one = gui_desktop_findwindow (x, y);
		if (this_one)
			dragdrop_destination = gui_window_findcontrol (
				this_one, x - this_one->x, y - this_one->y);
		else
			dragdrop_atdesktop = 1;
		return;
	}

	/* Deactivate double click detection if too far from origin */
	if (last_clicktime != -1 && (abs(x - last_clickx) > 3 || abs(y - last_clicky) > 3))
		last_clicktime = -1;

	/* A window the user is dragging automatically captures the mouse */
	for (i = 0 ; i < 8 ; i++)
	{
		if ((buttons & (1 << i)) && clicked_window[i])
		{
			for (j = 0 ; j < window_count ; j++)
			{
				if (window[j] == clicked_window[i])
				{
					gui_window_mousemove (window[j], x - window[j]->x, y - window[j]->y, buttons) ;
					last_x = x;
					last_y = y;
					return;
				}
			}
		}
	}
	if ((buttons & 1) && lastclick_window && (lastclick_window->innerdrag || lastclick_window->drag))
	{
		gui_window_mousemove (lastclick_window, x - lastclick_window->x, y - lastclick_window->y, buttons) ;
		last_x = x;
		last_y = y;
		return;
	}

	/* Was the mouse inside a window the last time? */
	if (last_x != -1)
		last_one = gui_desktop_findwindow (last_x, last_y) ;

	if (last_one && (last_one->innerdrag || (last_one->drag & DRAG_RESIZEHV)))
	{
		gui_window_mousemove (last_one, x - last_one->x, y - last_one->y, buttons) ;
		return;
	}
	if (last_one && last_one->drag)
	{
		gui_window_mousemove (last_one, x - last_one->x, y - last_one->y, buttons) ;
		return;
	}

	/* Any popups? */
	if (last_x != -1)
		last_p = gui_desktop_findpopup (last_x, last_y);
	this_p = gui_desktop_findpopup (x, y);
	if (this_p != NULL)
	{
		if (last_p != NULL)
			(*last_p->control->mouseleave)(last_p->control);
		else if (last_one)
			gui_window_mouseleave(last_one);
		if (last_p != this_p)
			(*this_p->control->mouseenter)(this_p->control);
		if ((*this_p->control->mousemove)(this_p->control, x-this_p->x, y-this_p->y, buttons))
		{
			last_x = x;
			last_y = y;
		}
		return;
	}
	if (last_p != NULL)
		(*last_p->control->mouseleave)(last_p->control);

	last_x = x;
	last_y = y;

	/* Is the mouse inside a window now? */
	this_one = gui_desktop_findwindow (x, y) ;
	if (this_one != window[window_count-1] && window[window_count-1]->modal != 0)
	{
		if (last_one)
			gui_window_mouseleave(last_one);
		last_one = 0;
		return;
	}

	if (last_one != this_one)
	{
		if (last_one)
			gui_window_mouseleave(last_one);
		if (this_one)
			gui_window_mouseenter(last_one);
	}

	if (this_one)
		gui_window_mousemove (this_one, x - this_one->x, y - this_one->y, buttons) ;
}

/** Propagate a mouse button event
 *
 *  @param x		Mouse X coordinate
 *  @param y		Mouse Y coordinate
 *  @param b		Mouse button
 *  @param pressed	1 if pressed, 0 if released
 */

void gui_desktop_mousebutton (int x, int y, int b, int pressed)
{
	WINDOW * last_one = NULL;
	WINDOW * win;
	POPUP  * p;
	int      i;

	/* This global variable holds the current mouse button state */
	if (pressed == 1)
		buttons |= (1 << b);
	if (pressed == 0)
		buttons &= ~(1 << b);

	/* Mouse is captured? */
	if (mouse_captured_window)
	{
		for (i = 0 ; i < window_count ; i++)
		{
			if (window[i] == mouse_captured_window)
			{
				gui_window_mousebutton (window[i], x - window[i]->x, y - window[i]->y, b, pressed);
				clicked_window[b] = 0;
				return;
			}
		}
		mouse_captured_window = 0;
	}

	if (dragdrop)
	{
		gui_desktop_enddrag();
		if (pressed != 0)
		{
			default_pointer = 0;
			dropped = 0;
		}
	}

	if (b > 7 || b < 0)
		return;

	/* The user clicked on a window and she is releasing the mouse button now */
	if (pressed == 0 && clicked_window[b])
	{
		for (i = 0 ; i < window_count ; i++)
		{
			if (window[i] == clicked_window[b])
			{
				/* The window is still visible */
				gui_window_mousebutton (window[i], x - window[i]->x, y - window[i]->y, b, pressed);
				clicked_window[b] = 0;
				return;
			}
		}
		clicked_window[b] = 0;
	}
	
	/* lastclick_window holds a pointer to the windows the user is dragging */
	if (pressed == 0 && b == 0 && lastclick_window && (lastclick_window->innerdrag || lastclick_window->drag))
	{
		gui_window_mousebutton (lastclick_window, x - lastclick_window->x, 
				y - lastclick_window->y, b, pressed);
		if (!buttons) lastclick_window = 0;
		return;
	}

	/* Detect double clicks */
	if (pressed == 1 && abs(x - last_clickx) < 4 && abs(y - last_clicky) < 4)
	{
		if (last_clicktime > gr_timer() - DOUBLECLICK_THRESOLD)
			if (last_clickbutton == b)
				pressed++;
	}
	if (pressed)
	{
		/* Store the click position in order to detect double clicks */
		last_clickx = x;
		last_clicky = y;
		last_clickbutton = b;
		last_clicktime = gr_timer();
	}

	/* Any popups? */
	p = gui_desktop_findpopup (x, y);
	if (p != 0)
	{
		(*p->control->mousebutton)(p->control, x-p->x, y-p->y, b, pressed);
		return;
	}
	if (last_x != -1)
	{
		p = gui_desktop_findpopup (last_x, last_y);
		if (p != NULL && p->control->innerdrag)
		{
			(*p->control->mousebutton)(p->control, x-p->x, y-p->y, b, pressed);
			return;
		}
	}

	/* Click outside any popup control: remove all popups */
	if (popup && pressed)
		gui_desktop_removepopup(0);

	/* Find the window at last mouse coordinates */
	if (last_x != -1)
		last_one = gui_desktop_findwindow (last_x, last_y) ;
	if (last_one && !pressed && (last_one->innerdrag || (last_one->drag & DRAG_RESIZEHV)))
	{
		gui_window_mousebutton (last_one, x - last_one->x, y - last_one->y, b, pressed) ;
		return;
	}

	/* Find the window at current coordinates */
	win = gui_desktop_findwindow(x, y);
	if (win)
	{
		if (pressed)
		{
			if (win != window[window_count-1] && window[window_count-1]->modal != 0)
				return;
			gui_desktop_bringtofront (win);
		}
		if (buttons && pressed)
		{
			clicked_window[b] = win;
			if (b == 0)
				lastclick_window = win;
		}
		gui_window_mousebutton (win, x - win->x, y - win->y, b, pressed);
	}
}

/** Propagate a key event
 *
 *  @param scancode	Keyboard scan code of the key
 *  @param key		ASCII code of the character
 */

void gui_desktop_key (int scancode, int key)
{
	int i;

	if (dragdrop)
		gui_desktop_enddrag();

	if (popup)
	{
		if ((*popup->control->key)(popup->control, scancode, key))
			return;
		gui_desktop_removepopup(0);
	}

	/* Window navigation keys */
	if (scancode == KEYMOD_CONTROL + KEY_TAB && window_count > 1 &&
		window[window_count-1]->modal == 0)
	{
		for (i = 0 ; i < window_count ; i++)
			if (!window[i]->dontfocus)
				break;
		if (i < window_count)
			gui_desktop_bringtofront (window[i]);
		return;
	}

	/* System keys */
	if (scancode == KEY_F5 && window_count > 0)
	{
		gui_action_maximizewindow (window[window_count-1]);
		return;
	}

	/* Global menu keys (ALT+N): docked windows have precedence */
	if ((scancode & KEYMOD_ALT) && ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z'))
		&& window_count > 1 && window[window_count-1]->modal == 0)
	{
		int i;

		for (i = 0 ; i < window_count ; i++)
		{
			if (window[i]->dock)
			{
				if (gui_window_key (window[i], scancode, key))
					return;
			}
		}
	}

	if (window_count > 0)
	{
		WINDOW * win = window[window_count-1];

		assert (win != NULL);
		gui_window_key (win, scancode, key);
	}
}

/** Add a new pop-up control to the desktop
 *
 *  @param window		Window that requests the pop-up or 0 if none
 *	@param control		Pointer to the control
 *	@param x			X Coordinate (relative to window)
 *	@param y			Y Coordinate (relative to window)
 *	@return				1 if succeded, 0 otherwise
 **/

int gui_desktop_addpopup (WINDOW * window, CONTROL * control, int x, int y)
{
	POPUP * p;
	int     i, j;

	/* Showing a popup resets the automatic click-capture
	   so the user can drag the mouse into the popup control and the
	   mouse button release can be received by the pop-up */

	for (i = 0 ; i < 8 ; i++)
	{
		if (clicked_window[i])
		{
			for (j = 0 ; j < 7 ; j++)
				clicked_window[i]->clicked_control[j] = 0;
			clicked_window[i] = 0;
		}
	}

	p = (POPUP *)malloc(sizeof(POPUP));
	if (p == 0)
		return 0;

	p->window = window;
	p->control = control;
	p->control->window = window;
	if (window)
	{
		p->x = x - window->border - window->padding;
		p->y = y - window->border - window->padding;
	}
	else
	{
		p->x = x;
		p->y = y;
	}
	p->control->x = x;
	p->control->y = y;
	p->next = popup;
	p->prev = 0;
	if (popup) 
	{
		popup->control->focused = 0;
		popup->prev = p;
	}
	popup = p;
	popup->control->focused = 1;

	if (window)
	{
		p->x += window->x + window->border + 2;
		p->y += window->y + window->caption + window->border + 2;
	}

	p->control->redraw = 1;

	return 1;
}

/** Remove a popup control from the desktop and destroy it
 *
 *  @param control		Pointer to the control to remove or 0 to remove all of them
 **/

void gui_desktop_removepopup (CONTROL * control)
{
	POPUP * p;

	if (control == 0)
	{
		/* Remove all controls */
		while (popup)
		{
			p = popup;
			popup = p->next;
			if (popup) popup->prev = NULL;
			gr_mark_rect (p->x, p->y, p->control->width, p->control->height);
			(*p->control->destructor)(p->control);
			free (p->control);
			free (p);
		}
		popup = 0;
	}
	else
	{
		/* Remove one specific control */
		for (p = popup ; p ; p = p->next)
		{
			if (p->control == control)
			{
				gr_mark_rect (p->x, p->y, p->control->width, p->control->height);
				if (p->prev)
					p->prev->next = p->next;
				else
					popup = p->next;
				if (p->next)
					p->next->prev = p->prev;
				(*p->control->destructor)(p->control);
				free (p->control);
				free (p);
				return;
			}
		}
		if (popup)
			popup->control->focused = 1;
	}
}

/** Remove a popup control from the desktop only if the parent is the given one
 *
 *  @param control		Pointer to the control to remove or 0 to remove all of them
 *  @param parent		Pointer to parent control
 **/

void gui_desktop_removepopup_p (CONTROL * control, CONTROL * parent)
{
	POPUP * p;

	/* Remove one specific control */
	for (p = popup ; p ; p = p->next)
	{
		if ((p->control == control || !control) && p->control->parent == parent)
		{
			if (p->prev)
				p->prev->next = p->next;
			else
				popup = p->next;
			if (p->next)
				p->next->prev = p->prev;
			(*p->control->destructor)(p->control);
			free (p->control);
			free (p);
			return;
		}
	}
	if (popup)
		popup->control->focused = 1;
}

/** Search for a popup control in a given screen coordinate
 *  
 *  @param x		X Coordinate in screen
 *	@param y		Y Coordinate in screen
 *	@return			Pointer to the popup struct or 0 if none found
 **/

static POPUP * gui_desktop_findpopup (int x, int y)
{
	POPUP * p = popup;

	while (p)
	{
		if (p->x <= x && p->y <= y 
			&& p->control->width  + p->x >= x
			&& p->control->height + p->y >= y)
			break;
		p = p->next;
	}
	return p;
}

/** Search for a popup control, return 1 if the popup exists
 *  
 *  @param control	Pointer to the control object
 *	@return			1 if the popup exists, 0 otherwise
 **/

int gui_desktop_ispopup (CONTROL * control)
{
	POPUP * p = popup;

	while (p)
	{
		if (p->control == control)
			return 1;
		p = p->next;
	}
	return 0;
}

/** Returns the current mouse pointer
 *
 *	@return		One of the POINTER_XXX constants
 */

int gui_desktop_mousepointer()
{
	WINDOW * w;
	POPUP * p;
	int result = 0;

	p = gui_desktop_findpopup (last_x, last_y);
	if (p)
	{
		if (p->control)
		{
			if (p->window)
				result = (*p->control->mousepointer)(p->control, last_x - p->window->x - p->control->x, last_y - p->window->y - p->control->y);
			else
				result = (*p->control->mousepointer)(p->control, last_x - p->control->x, last_y - p->control->y);
		}
	}
	else
	{
		if (buttons && lastclick_window)
			w = lastclick_window;
		else
			w = gui_desktop_findwindow (last_x, last_y);
		if (w)
		{
			result = gui_window_mousepointer (w, last_x - w->x, last_y - w->y);
		}
	}

	return result ? result : default_pointer;
}

/** Returns the first window in the desktop (at the background)
 */

WINDOW * gui_desktop_firstwindow()
{
	return window_count > 0 ? window[0] : NULL;
}

/** Returns the last window in the desktop (at foreground)
 */

WINDOW * gui_desktop_lastwindow()
{
	return window_count > 0 ? window[window_count-1] : NULL;
}

/** Given a window pointer, returns a pointer to the next window 
 *
 *	@param window		Pointer (not null) to a window
 *	@returns			Pointer to the next window or NULL if it is the top-most
 */

WINDOW * gui_desktop_nextwindow (WINDOW * w)
{
	int i;

	for (i = 0 ; i < window_count-1 ; i++)
		if (window[i] == w)
			return window[i+1];
	return NULL;
}

/** Return 1 if a window is present at the desktop
 *
 *  @param window		Pointer to a window
 *	@returns			1 if the window exists
 */

int gui_desktop_present (WINDOW * win)
{
	int i;

	for (i = 0 ; i < window_count ; i++)
	{
		if (window[i] == win)
			return 1;
	}
	return 0;
}

/** Set of reset the current tooltip text
 *
 *	@param tooltip		Tooltip text or NULL to hide the current tooltip
 */

void gui_desktop_tooltip (const char * text)
{
	if (text && *text == 0)
		text = 0;
	tooltip = text;
	if (tooltip)
		tooltip_time = gr_timer();
}

/** Start an object drag/drop operation
 *
 *	@param string		String code with object description
 *	@param graph		Pointer to dragable object graphic (optional)
 */

void gui_desktop_dragobject (int string, GRAPH * object)
{
	if (!dragdrop || (dragdrop_string != string || dragdrop_graph != object))
	{
		if (dragdrop_string != string)
		{
			if (dragdrop_string)
				string_discard(dragdrop_string);
			dragdrop_string = string;
			string_use (string);
		}
		dragdrop = 1;
		dragdrop_atdesktop = 0;
		dragdrop_destination = NULL;
		dragdrop_graph = object;
		dragdrop_accepted = 0;
	}
}

/** Ends an object drag/drop operation
 */

void gui_desktop_enddrag()
{
	if (dragdrop)
	{
		dragdrop = 0;
		dropped = 1;
		dragdrop_graph = NULL;
		last_x = last_y = -1;
	}
}

/** Returns DRAG_ACTIVE, DRAG_INACTIVE or DRAG_DROPPED
 */

int gui_desktop_dragging()
{
	if (!dragdrop && dropped)
		return DRAG_DROPPED;
	if (dragdrop)
		return DRAG_ACTIVE;
	return DRAG_INACTIVE;
}

/** Returns the control that was the destination of the last dragging operation */
CONTROL * gui_desktop_dragcontrol()
{
	if (dragdrop_atdesktop)
		return (CONTROL *) DRAG_ATDESKTOP;
	return dragdrop_destination;
}

/** Returns the string associated with the last dragging operation */
int gui_desktop_dragtype()
{
	return dragdrop_string;
}

/** Sets the default mouse pointer */
void gui_desktop_setpointer (int p)
{
	default_pointer = p;
}

/** Gets the default mouse pointer */
int gui_desktop_getpointer()
{
	return default_pointer;
}

/** Set the drag/drop accepted flag */
void gui_desktop_acceptdrag (int accepted)
{
	dragdrop_accepted = accepted;
}

/** Enable mouse capturing by a window */
void gui_desktop_capturemouse (WINDOW * window)
{
	mouse_captured_window = window;
}

/** Release the mouse captured by a window */
void gui_desktop_releasemouse (WINDOW * window)
{
	if (mouse_captured_window == window)
		mouse_captured_window = 0;
}

/** Mark all popups that need redrawing */
void gui_desktop_markpopups ()
{
	POPUP * p = popup;
	
	while (p)
	{
		gr_mark_rect (p->x, p->y, p->control->width, p->control->height);
		p = p->next;
	}
}