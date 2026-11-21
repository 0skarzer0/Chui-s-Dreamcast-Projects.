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
 *  Contains the CONTROL:BUTTON:TOOL class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Count of popup menus currently active and activated
  * from a TOOL control. If there is some menu active and
  * the mouse moves over another TOOL menu, its menu will
  * be automatically opened */

static int any_menu_active = 0;

/** Draw a tool button.
 *  This is a member function of the CONTROL:BUTTON:TOOL class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_tool_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	TOOL * tool = (TOOL *)c;
	BUTTON * button = (BUTTON *)(&tool->button);
	REGION * inner_region;
	int tx, ty;
	int width = c->width;
	int height = c->height;
	int pressed;

	/* Check if the menu is opened and exists */
	if (tool->menu)
	{
		if (gui_desktop_ispopup(tool->menu) && 
			((MENU *)(tool->menu))->caller == &tool->button.control)
		{
			c->highlight = 1;
			button->pressed = 1;
		}
		else
		{
			c->highlight = 0;
			button->pressed = 0;
			tool->menu = NULL;
			any_menu_active--;
		}
	}
	else if (tool->menuoptions)
	{
		button->pressed = 0;

		if (c->highlight && any_menu_active)
		{
			button->pressed = 1;
			gui_desktop_removepopup(0);
			tool->menu = gui_menu_new (tool->menuoptions, tool->menucount);
			gui_menu_context (tool->menu, tool->lefttool, tool->righttool);
			((MENU*)(tool->menu))->caller = &tool->button.control;
			gui_desktop_addpopup (c->window, tool->menu, c->x, c->y + c->height);
			any_menu_active++;
		}
	}

	/* Set the pressed state if the tool is a two-state version */
	pressed = button->pressed;
	if (tool->variable)
	{
		if (tool->state == TOOL_STATE_CHECK)
			pressed |= *(tool->variable);
		else
			pressed |= (*tool->variable == tool->state);
	}

	/* Draw the button frame */
	if (tool->frame)
	{
		width++;
		gr_setcolor (color_border);
		gr_rectangle (dest, clip, x, y, width, height);
		x++, y++, width-=2, height-=2;
	}
	if (c->highlight || tool->frame || pressed)
	{
		gr_setcolor (c->highlight || pressed ? color_highlightface : color_window);
		gr_box (dest, clip, x+1, y+1, width-2, height-2);
		gr_setcolor (!pressed ? color_highlight : color_shadow);
		gr_hline (dest, clip, x, y, width-1);
		gr_vline (dest, clip, x, y, height-1);
		gr_setcolor (!pressed ? color_shadow : color_face);
		gr_hline (dest, clip, x, y+height-1, width);
		gr_vline (dest, clip, x+width-1, y, height);
	}

	/* Draw the text */
	gr_setcolor (color_border);
	tx = x + (width  - gui_text_width  (button->text)) / 2;
	ty = y + (height - gui_text_height (button->text, 0)) / 2;
	tx += tool->offsetx;
	ty += tool->offsety;
	if (pressed) tx++, ty++;
	gr_text_setcolor (color_text);
	gr_setcolor (color_text);
	gui_text_put (dest, clip, tx, ty, button->text, 0, 0) ;

	/* Draw the graphic */
	if (tool->graph)
	{
		inner_region = region_new (x+1, y+1, width-2, height-2);
		if (clip != NULL)
			region_union (inner_region, clip);
		x += (width - tool->graph->width) / 2;
		y += (height - tool->graph->height) / 2;
		x += tool->offsetx;
		y += tool->offsety;
		if (tool->graph->cpoints)
		{
			x += tool->graph->cpoints[0].x;
			y += tool->graph->cpoints[0].y;
		}
		else
		{
			x += tool->graph->width / 2;
			y += tool->graph->height / 2;
		}
		if (pressed) x++, y++;
		gr_blit (dest, inner_region, x, y, 0, tool->graph);
		region_destroy (inner_region);
	}
}


/** Change the tool's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:BUTTON:TOOL class
 *
 *  @param c		Pointer to the control 
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the button pressed or released
 *  @param pressed	1 if the button was pressed, 0 if it was released
 * */

int gui_tool_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	TOOL * tool = (TOOL *)c;
	BUTTON * button = &tool->button;

	if (button->pressed != pressed)
	{
		c->redraw = 1;

		if (button->pressed && !pressed && button->callback)
		{
			if (tool->variable)
			{
				if (tool->state == TOOL_STATE_CHECK)
					(*tool->variable) = !(*tool->variable);
				else
					(*tool->variable) = tool->state;
			}
			gui_button_action(button);
		}

		if (tool->menuoptions != NULL && pressed)
		{
			tool->menu = gui_menu_new (tool->menuoptions, tool->menucount);
			gui_menu_context (tool->menu, tool->lefttool, tool->righttool);
			((MENU *)(tool->menu))->caller = &tool->button.control;
			gui_desktop_addpopup (c->window, tool->menu, c->x, c->y + c->height);
			any_menu_active++;
		}

		button->pressed = pressed;
		c->redraw = 1;
	}
	return 1;
}

extern int gui_button_syskey(CONTROL *, int);

/** Check if the control supports the given acces key (or any at all)
 *
 *  @param control	Pointer to the control
 *	@param key		Character pressed or 0 to check if any is present
 **/

int gui_tool_syskey (CONTROL * control, int key)
{
	TOOL * tool = (TOOL *)control;
	int r = gui_button_syskey(control, key);

	if (r && tool->menuoptions != NULL)
	{
		gui_desktop_removepopup (0);
		tool->menu = gui_menu_new (tool->menuoptions, tool->menucount);
		gui_menu_context (tool->menu, tool->lefttool, tool->righttool);
		((MENU *)(tool->menu))->caller = &tool->button.control;
		gui_desktop_addpopup (control->window, tool->menu, control->x, control->y + control->height);
		any_menu_active = 1;
	}
	return r;
}

/** The user moves the focus to the control
 *
 *  @param c		Pointer to the control member (at offset 0)
 *  @return			1 if the control is capable of focusing
 */

int gui_tool_enter (CONTROL * c)
{
	return 0;
}

/** Initialize the data of a new tool button control 
 *
 *  @param tool		Pointer to the tool button control
 *  @param graph 	Pointer to the graphic
 **/

void gui_tool_init (TOOL * tool, const char * text, GRAPH * graph)
{
	gui_button_init (&tool->button, text);
	if (text)
	{
		tool->button.control.width = gui_text_width(tool->button.text) + 16;
		tool->button.control.height = gui_text_height(tool->button.text, 0) + 6;
	}
	else
	{
		tool->button.control.width = 24;
		tool->button.control.height = 24;
	}
	tool->button.control.draw = gui_tool_draw;
	tool->button.control.mousebutton = gui_tool_mousebutton;
	tool->button.control.syskey = gui_tool_syskey;
	tool->button.control.innerdrag = 0;
	tool->graph = graph;
	tool->frame = 0;
	tool->menu = NULL;
	tool->menucount = 0;
	tool->menuoptions = NULL;
	tool->offsetx = 0;
	tool->offsety = 0;
	tool->button.control.enter = gui_tool_enter;
	tool->lefttool = 0;
	tool->righttool = 0;
	tool->variable = NULL;
	tool->state = TOOL_STATE_CHECK;
}


/** Create a new tool button object
 *
 *  @param graph	Pointer to the graphic
 *	@return			Pointer to the new object
 **/

CONTROL * gui_tool_new (GRAPH * graph)
{
	TOOL * tool ;

	/* Alloc memory for the struct */
	tool = (TOOL *) malloc(sizeof(TOOL));
	if (tool == NULL)
		return NULL;
	gui_tool_init (tool, "", graph);
	return &(tool->button.control);
}

/** Create a new tool button object, checkbox-style (two states)
 *
 *  @param graph	Pointer to the graphic
 *  @param variable	Pointer to state variable (switches between 1 and 0 on check)
 *	@return			Pointer to the new object
 **/

CONTROL * gui_tool_new_check (GRAPH * graph, int * variable)
{
	TOOL * tool = (TOOL *)gui_tool_new(graph);
	tool->variable = variable;
	return &(tool->button.control);
}

/** Create a new tool button object, radio-style (two states)
 *
 *  @param graph	Pointer to the graphic
 *  @param variable	Pointer to state variable (switches to state value on check)
 *	@param state	State value
 *	@return			Pointer to the new object
 **/

CONTROL * gui_tool_new_radio (GRAPH * graph, int * variable, int state)
{
	TOOL * tool = (TOOL *)gui_tool_new(graph);
	tool->variable = variable;
	tool->state = state;
	return &(tool->button.control);
}

/** Create a new tool button object with a given text
 *
 *  @param graph	Pointer to the graphic
 *	@return			Pointer to the new object
 **/

CONTROL * gui_tool_newt (const char * text)
{
	TOOL * tool ;

	/* Alloc memory for the struct */
	tool = (TOOL *) malloc(sizeof(TOOL));
	if (tool == NULL)
		return NULL;
	gui_tool_init (tool, text, NULL);
	return &(tool->button.control);
}

/** Create a new tool button object with a given text and submenu
 *
 *  @param graph	Pointer to the graphic
 *	@return			Pointer to the new object
 **/

CONTROL * gui_tool_newm (const char * text, MENUITEM * items, int count)
{
	TOOL * tool ;

	/* Alloc memory for the struct */
	tool = (TOOL *) malloc(sizeof(TOOL));
	if (tool == NULL)
		return NULL;
	gui_tool_init (tool, text, NULL);
	tool->menuoptions = items;
	tool->menucount = count;
	return &(tool->button.control);
}

/** Put the tool in a menu-bar context
 *
 *	@param control		Pointer to the tool
 *	@param left			Pointer to the tool or control to receive focus when the user presses LEFT
 *	@param right		Pointer to the tool or control to receive focus when the user presses RIGHT
 */

void gui_tool_context (CONTROL * control, CONTROL * left, CONTROL * right)
{
	TOOL * tool = (TOOL *)control;

	tool->lefttool = left;
	tool->righttool = right;
}
