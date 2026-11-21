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
 *  Contains the CONTROL:TABS class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

#define CAPTION_HEIGHT 15


/* ------------------------------------------------------------------------------
 *   Utility functions
 * ------------------------------------------------------------------------------ */


/** Draw a title tab
 *
 *	@param dest			Destination graphic
 *	@param clip			Clipping coordinates
 *	@param x			X Coordinate
 *	@param y			Y Coordinate
 *  @param w			Width of the tab
 *	@param text			Text of the tab
 *	@param highlight	1 to draw the tab as highlighted, 0 otherwise
 *  @param selected     1 if the tab is selected, 0 otherwise
 */

static void gui_tabs_drawtab (GRAPH * dest, REGION * clip, int x, int y, int w, 
							  const char * text, int highlight, int selected)
{
	int i, j;
	int face = color_face;

	if (selected)
		face = color_window;
	if (highlight)
		face = color_highlight;

	/* Draw the background */
	gr_setcolor (face);
	for (i = 0 ; i < 6 ; i++)
	{
		j = CAPTION_HEIGHT*i/6;
		gr_vline (dest, clip, x+i, y+CAPTION_HEIGHT-1-j, j);
		gr_vline (dest, clip, x+w-1-i, y+CAPTION_HEIGHT-1-j, j);
	}
	gr_box (dest, clip, x+i, y, w-2*i, CAPTION_HEIGHT-1);

	/* Draw the border */
	gr_setcolor (color_highlight);
	gr_line (dest, clip, x, y+CAPTION_HEIGHT-1, i, -CAPTION_HEIGHT);
	gr_hline (dest, clip, x+i, y, w-2*i);
	gr_setcolor (color_shadow);
	gr_line (dest, clip, x+w-1-i, y, i, CAPTION_HEIGHT);

	/* Draw the text */
	gr_setcolor (color_text);
	gr_text_setcolor (color_text);
	gui_text_put (dest, clip, x+w/2, y+3, text, 1, 0);
}

/** Add a new page to the given tabbed dialog
 *
 *  @param control		Pointer to the tabbed dialog control
 *	@param title		String identifier of the dialog title
 */

void gui_tabs_addpage (CONTROL * control, int title)
{
	TABS * tabs = (TABS *)control;
	int    i;

	/* Alloc more page slots if needed */

	if (tabs->pages_count == tabs->pages_allocated)
	{
		tabs->pages_allocated += 4;
		tabs->pages = (TABPAGE *) realloc(tabs->pages,
			tabs->pages_allocated * sizeof(TABPAGE));
	}

	i = tabs->pages_count++;
	tabs->pages[i].title = title;
	tabs->pages[i].title_x = tabs->tabs_width;
	tabs->pages[i].title_width = gui_text_width(string_get(title)) + 30;
	tabs->pages[i].container = (CONTAINER *) gui_container_new (
		control->width - tabs->border, control->height - CAPTION_HEIGHT - tabs->border);
	tabs->pages[i].container->control.parent = control;
	tabs->pages[i].container->control.window = control->window;
	string_use(title);

	tabs->current_page = i;
	tabs->tabs_width += tabs->pages[i].title_width;
	control->redraw = 1;
}

/** Add a new control to the current selected page of the tabbed dialog
 *
 *	@param control			Pointer to the tabbed dialog control
 *	@param x				X internal coordinate of the control
 *	@param y				Y internal coordinate of the control
 *	@param control			Pointer to the control to add
 **/

void gui_tabs_addcontrol (CONTROL * control, int x, int y, CONTROL * add)
{
	TABS * tabs = (TABS *)control;

	if (tabs->current_page >= 0)
	{
		CONTROL * container = (CONTROL *)tabs->pages[tabs->current_page].container;

		x += tabs->border;
		y += tabs->border;
		gui_container_add (container, x, y, add);
		if (control->width < container->width + tabs->border)
			control->width = container->width + tabs->border;
		if (control->height < container->height + CAPTION_HEIGHT + tabs->border)
		    control->height = container->height + CAPTION_HEIGHT + tabs->border;

		control->redraw = 1;
	}
}

/** Change the selected page in a tabbed dialog
 *
 *  @param control			Pointer to the tabbed control dialog
 *	@param page				New page (0 for leftmost)
 */

void gui_tabs_setpage (CONTROL * control, int page)
{
	TABS * tabs = (TABS *)control;

	if (page >= 0 && page < tabs->pages_count)
	{
		tabs->current_page = page;
		control->redraw = 1;
	}
}

/** Draw a tabbed dialog control
 *
 *	@param c				Pointer to the tabbed dialog control
 *	@param dest				Destination graphic
 *	@param x				Left X
 *	@param y				Top Y
 *	@param clip				Clipping region
 */

void gui_tabs_draw (CONTROL * control, GRAPH * dest, int x, int y, REGION * clip)
{
	int i, c;
	TABS * tabs = (TABS *)control;
	int nb_x = -1, nb_x2;

	/* Draw the tabs */

	for (i = 0 ; i < tabs->pages_count ; i++)
	{
		c = tabs->pages[i].title_x;
		gui_tabs_drawtab (dest, clip, x+c, y, tabs->pages[i].title_width,
			string_get(tabs->pages[i].title), tabs->highlight_page == i,
			tabs->current_page == i);

		if (tabs->current_page == i)
		{
			nb_x = x + c;
			nb_x2 = nb_x + tabs->pages[i].title_width;
		}
	}

	/* Draw the border */

	gr_setcolor (color_highlight);
	gr_vline (dest, clip, x, y+CAPTION_HEIGHT-1, control->height-CAPTION_HEIGHT);
	if (nb_x == -1)
		gr_hline (dest, clip, x, y+CAPTION_HEIGHT-1, control->width);
	else
	{
		gr_hline (dest, clip, x, y+CAPTION_HEIGHT-1, nb_x - x);
		gr_hline (dest, clip, nb_x2, y+CAPTION_HEIGHT-1, x + control->width - nb_x2);
	}
	gr_setcolor (color_shadow);
	gr_vline (dest, clip, x+control->width-1, y+CAPTION_HEIGHT-1, control->height-CAPTION_HEIGHT+1);
	gr_hline (dest, clip, x, y+control->height-1, control->width);
	
	/* Draw the contents */

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
		gui_container_draw (&tabs->pages[tabs->current_page].container->control, 
			dest, x, y + CAPTION_HEIGHT, clip);
}

/* ------------------------------------------------------------------------------
 *   Member functions
 * ------------------------------------------------------------------------------ */


/** Destroy a tabbed dialog control (destroys all the pages and allocated memory)
 *
 *  @param control		Pointer to the tabbed dialog control
 */

void gui_tabs_destructor (CONTROL * control)
{
	TABS * tabs = (TABS *)control;
	int    i;
	
	for (i = 0 ; i < tabs->pages_count ; i++)
		gui_container_destructor(&tabs->pages[i].container->control);
	if (tabs->pages)
		free(tabs->pages);
	gui_control_destructor(control);
}

/** Function called when the mouse enters the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 **/

int gui_tabs_mouseenter (CONTROL * c)
{
	TABS * tabs = (TABS *)c;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_mouseenter(container);
		c->redraw |= container->redraw;
		return result;
	}
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

int gui_tabs_mousemove (CONTROL * c, int x, int y, int buttons)
{
	TABS * tabs = (TABS *)c;

	/* Highlight the page under the mouse */

	if (y >= 0 && y < CAPTION_HEIGHT)
	{
		int i;

		for (i = 0 ; i < tabs->pages_count ; i++)
		{
			if (tabs->pages[i].title_x <= x && 
				tabs->pages[i].title_width + tabs->pages[i].title_x >= x)
			{
				break;
			}
		}

		if (i == tabs->pages_count)
			i = -1;
		if (tabs->highlight_page != i)
		{
			tabs->highlight_page = i;
			c->redraw = 1;
		}
	}
	else
	{
		if (tabs->highlight_page != -1)
		{
			tabs->highlight_page = -1;
			c->redraw = 1;
		}
	}

	/* Propagate the event */

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_mousemove (container, x, y - CAPTION_HEIGHT, buttons);
		c->redraw |= container->redraw;
		return result;
	}
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

int gui_tabs_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	TABS * tabs = (TABS *)c;

	/* Select the page under the mouse, if LMB pressed */

	if (y >= 0 && y < CAPTION_HEIGHT && pressed && b == 0)
	{
		int i;

		for (i = 0 ; i < tabs->pages_count ; i++)
		{
			if (tabs->pages[i].title_x <= x && 
				tabs->pages[i].title_width + tabs->pages[i].title_x >= x)
			{
				if (tabs->current_page != i)
				{
					if (c->focused && tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
					{
						gui_container_leave (&tabs->pages[tabs->current_page].container->control);
						gui_container_mouseleave (&tabs->pages[tabs->current_page].container->control);
					}
					tabs->current_page = i;
					c->redraw = 1;
					return 1;
				}
				break;
			}
		}
	}

	/* Propagate the event */

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_mousebutton (container, x, y - CAPTION_HEIGHT, b, pressed);
		c->redraw |= container->redraw;
		return result;
	}
	return 0;
}

/** Function called when the mouse leaves the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 **/

int gui_tabs_mouseleave (CONTROL * c)
{
	TABS * tabs = (TABS *)c;

	if (tabs->highlight_page >= 0)
	{
		tabs->highlight_page = -1;
		c->redraw = 1;
	}
	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_mouseleave (container);
		c->redraw |= container->redraw;
		return result;
	}
	return 0;
}

/** Function called when the user presses a key and the control has the keyboard focus
 *
 *  @param c			Pointer to the control member (at offset 0 of the struct)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_tabs_key (CONTROL * c, int scancode, int key)
{
	TABS * tabs = (TABS *)c;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_key (container, scancode, key);
		c->redraw |= container->redraw;
		return result;
	}
	return 0;
}

/** Function called when the user moves (or tries to move) the keyboard focus to the control
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @return			1 if the control is capable of focusing
 */

int gui_tabs_enter (CONTROL * c)
{
	TABS * tabs = (TABS *)c;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_enter (container);
		c->redraw |= container->redraw;
		return result;
	}
	return 0;
}

/** Function called when the keyboard focus is lost 
 *
 *  @param c		Pointer to the control member (at offset 0 of the struct)
 *  @return			1 if the control is capable of focusing (unused)
 */

int gui_tabs_leave (CONTROL * c)
{
	TABS * tabs = (TABS *)c;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_leave (container);
		c->redraw |= container->redraw;
		return result;
	}

	c->focused = 0;
	return 0;
}

/** Function called to check if the control supports the given acces key (or any at all)
 *
 *  @param control	Pointer to the control
 *	@param key		Character pressed or 0 to check if any is present
 *  @return			1 if the key is a shortcut key, 0 otherwise
 **/

int gui_tabs_syskey (CONTROL * c, int key)
{
	TABS * tabs = (TABS *)c;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;
		int result = gui_container_syskey (container, key);
		c->redraw |= container->redraw;
		return result;
	}
	return 0;
}

/** Function called to check what mouse cursor to use when the mouse is
 *  inside the control, at the given coordinates
 *
 *  @param control	Pointer to the control
 *  @param x		Left coordinate (local to the control)
 *	@param y		Top coordinate (local to the control)
 */

int gui_tabs_mousepointer (CONTROL * c, int x, int y)
{
	TABS * tabs = (TABS *)c;

	if (y >= 0 && y < CAPTION_HEIGHT)
		return c->pointer;

	if (tabs->current_page >= 0 && tabs->current_page < tabs->pages_count)
	{
		CONTROL * container = &tabs->pages[tabs->current_page].container->control;

		return gui_container_mousepointer (container, x, y-CAPTION_HEIGHT);
	}
	return c->pointer;
}


/* ------------------------------------------------------------------------------
 *   Constructor
 * ------------------------------------------------------------------------------ */


/** Initialize a tabbed dialog control
 *
 *  @param tabs			Pointer to the TABS struct
 */

void gui_tabs_init (TABS * tabs)
{
	/* Initialize parent class members */
	gui_control_init (&tabs->control);

	/* Initialize data members */
	tabs->pages = NULL;
	tabs->pages_count = 0;
	tabs->pages_allocated = 0;
	tabs->current_page = -1;
	tabs->highlight_page = -1;
	tabs->tabs_width = 0;
	tabs->border = 4;

	/* Initialize member functions */
	tabs->control.draw = gui_tabs_draw;
	tabs->control.enter = gui_tabs_enter;
	tabs->control.leave = gui_tabs_leave;
	tabs->control.key = gui_tabs_key;
	tabs->control.syskey = gui_tabs_syskey;
	tabs->control.mouseenter = gui_tabs_mouseenter;
	tabs->control.mouseleave = gui_tabs_mouseleave;
	tabs->control.mousebutton = gui_tabs_mousebutton;
	tabs->control.mousemove = gui_tabs_mousemove;
	tabs->control.mousepointer = gui_tabs_mousepointer;
	tabs->control.destructor = gui_tabs_destructor;
}

/** Create a new tabbed dialog control
 *
 *	@return				Pointer to the new control
 **/

CONTROL * gui_tabs_new (int width, int height)
{
	TABS * tabs;

	tabs = (TABS *)malloc(sizeof(TABS));
	if (tabs == NULL)
		return NULL;
	gui_tabs_init (tabs);
	tabs->control.width = width;
	tabs->control.height = height;
	return &tabs->control;
}
