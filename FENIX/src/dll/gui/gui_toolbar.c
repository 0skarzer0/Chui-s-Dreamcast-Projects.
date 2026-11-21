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
 *  Contains the CONTROL:CONTAINER:TOOLBAR class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

void gui_tool_callback (CONTROL * c, int index)
{
	((TOOLBAR *)c)->pressed = index;
}

/** Set the button type for one toolbar control member. Usage count
 *  is mantained by the toolbar object automatically.
 *
 *	@param c			Pointer to the toolbar control
 *	@param index		Index of the element or -1 for the last one
 *	@param type			Type string 
 */

void gui_toolbar_settype (CONTROL * c, int index, int type)
{
	TOOLBAR * toolbar = (TOOLBAR *)c;

	if (index < 0)
		index = toolbar->tools_count + index;
	if (toolbar->tools_count > index && index >= 0)
	{
		if (toolbar->tools_type[index] != 0)
			string_discard (toolbar->tools_type[index]);
		if (type != 0)
			string_use (type);
		toolbar->tools_type[index] = type;
	}
}

/** Add a new button to an existing toolbar control
 *
 *	@param c			Pointer to the toolbar control
 *	@param graph		Graphic of the toolbar
 **/

void gui_toolbar_addbutton (CONTROL * c, GRAPH * graph)
{
	CONTROL * button = gui_tool_new (graph);
	CONTROL * container = (CONTROL *)c;

	gui_button_callbackpi (button, gui_tool_callback, c, ((CONTAINER *)container)->count);
	gui_container_add (container, c->width, 0, button);
	c->min_width = c->width;
}

/** Add a new button to an existing toolbar control, including some tooltip text
 *
 *	@param c			Pointer to the toolbar control
 *	@param graph		Graphic of the toolbar
 **/

void gui_toolbar_addbutton_tooltip (CONTROL * c, GRAPH * graph, int tooltip)
{
	CONTROL * button = gui_tool_new (graph);
	CONTAINER * container = (CONTAINER *)c;

	gui_button_callbackpi (button, gui_tool_callback, c, container->count);
	button->tooltip = tooltip;
	gui_container_add (c, c->width, 0, button);
	c->min_width = c->width;
}

/** Add a new button (two-state, check type) to an existing toolbar control, including some tooltip text
 *
 *	@param c			Pointer to the toolbar control
 *	@param graph		Graphic of the toolbar
 **/

void gui_toolbar_addbutton_check (CONTROL * c, GRAPH * graph, int tooltip, int * var)
{
	CONTROL * button = gui_tool_new_check (graph, var);
	CONTAINER * container = (CONTAINER *)c;

	gui_button_callbackpi (button, gui_tool_callback, c, container->count);
	button->tooltip = tooltip;
	gui_container_add (c, c->width, 0, button);
	c->min_width = c->width;
}

/** Add a new button (two state, radio type) to an existing toolbar control, including some tooltip text
 *
 *	@param c			Pointer to the toolbar control
 *	@param graph		Graphic of the toolbar
 **/

void gui_toolbar_addbutton_radio (CONTROL * c, GRAPH * graph, int tooltip, int * var, int state)
{
	CONTROL * button = gui_tool_new_radio (graph, var, state);
	CONTAINER * container = (CONTAINER *)c;

	gui_button_callbackpi (button, gui_tool_callback, c, container->count);
	button->tooltip = tooltip;
	gui_container_add (c, c->width, 0, button);
	c->min_width = c->width;
}

/** Add an user-defined control to an existing toolbar control
 *
 *	@param c			Pointer to the toolbar control
 *	@param control		User-defined control
 **/

void gui_toolbar_addcontrol (CONTROL * c, CONTROL * control)
{
	CONTROL * container = (CONTROL *)c;

	gui_container_add (container, c->width, 0, control);
	c->min_width = c->width;
}

/** Add a new menu to an existing toolbar control
 *
 *	@param c			Pointer to the toolbar control
 *	@param text			Text of the menu
 *	@param items		Menu items
 *	@param count		Number of menu items
 **/

void gui_toolbar_addmenu (CONTROL * c, const char * text, 
						  MENUITEM * items, int count)
{
	TOOLBAR * toolbar = (TOOLBAR *)c;
	CONTROL * menu = gui_tool_newm (text, items, count);
	CONTAINER * container = (CONTAINER *)c;

	if (toolbar->tools_count == toolbar->tools_allocated)
	{
		toolbar->tools_allocated += 16;
		toolbar->tools_type = (int *) realloc (toolbar->tools_type, 4 * toolbar->tools_allocated);
		toolbar->tools = (CONTROL **) realloc (toolbar->tools, sizeof(TOOL *) * toolbar->tools_allocated);
	}
	toolbar->tools_type[toolbar->tools_count] = 0;
	toolbar->tools[toolbar->tools_count++] = menu;

	if (toolbar->tools_count > 1)
	{
		gui_tool_context (toolbar->tools[toolbar->tools_count-1],
			toolbar->tools[toolbar->tools_count-2],
			toolbar->tools[0]);
		gui_tool_context (toolbar->tools[toolbar->tools_count-2],
			toolbar->tools[toolbar->tools_count > 2 ? toolbar->tools_count-3 : 1],
			toolbar->tools[toolbar->tools_count - 1]);
		gui_tool_context (toolbar->tools[0],
			toolbar->tools[toolbar->tools_count-1],
			toolbar->tools[1]);
	}

	gui_container_add (c, c->width, 0, (CONTROL *)menu);
	c->min_width = c->width;
}

/** Update the toolbar contents, adjusting it to the current window type.
 *  Hides and shows toolbar menu controls that have no type or have the
 *  same type of the current desktop focused window.
 *
 *	@param control			Pointer to the toolbar control
 */

void gui_toolbar_update (CONTROL * control)
{
	TOOLBAR * toolbar = (TOOLBAR *)control;
	const char * current_type = "NONE";
	static int type_none = 0;
	WINDOW * top;
	int i, x;

	/* Pointers to set each button's context */

	CONTROL * first  = NULL;
	CONTROL * second = NULL;
	CONTROL * prev1  = NULL;
	CONTROL * prev2  = NULL;
	CONTROL * last   = NULL;

	/* Get the top window's type (or "NONE" if no type) */

	top = gui_desktop_lastwindow();
	if (top) 
	{
		current_type = string_get (top->usertype);
		if (!current_type || !*current_type)
			current_type = "NONE";
	}

	/* Walk the tools array */

	for (i = x = 0 ; i < toolbar->tools_count ; i++)
	{
		int type = toolbar->tools_type[i];
		CONTROL * tool = toolbar->tools[i];

		if (type == 0 || strcmp (current_type, string_get(type)) == 0)
		{
			/* If already three buttons or more, set the previous one's context */

			if (prev1 && prev2)
				gui_tool_context (prev1, prev2, tool);

			/* Enable the control's visibility and set its x coordinate */

			tool->x = x;
			tool->hidden = 0;
			x += tool->width;

			/* Update the context pointers */

			if (first == NULL)
				first = tool;
			else if (second == NULL)
				second = tool;
			last = tool;
			prev2 = prev1;
			prev1 = tool;
		}
		else
		{
			tool->hidden = 1;
		}
	}

	/* Updates first and last button context */

	if (first && second)
	{
		gui_tool_context (first, last, second);
		gui_tool_context (last, prev2, first);
	}
}

/** Drawing stub (only updates the control and then uses CONTAINER:DRAW) */

void gui_toolbar_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	gui_toolbar_update(c);
	gui_container_draw(c, dest, x, y, clip);
}

/** Add a new separator to an existing toolbar control
 *
 *	@param c			Pointer to the toolbar control
 **/

void gui_toolbar_addseparator (CONTROL * c)
{
	CONTAINER * container = (CONTAINER *)c;

	c->width += 8;
	gui_container_add (c, c->width-5, 2, 
		gui_frame_new (2, c->height-4, FRAME_INSET));
	c->min_width = c->width;
}

/** The user moves the focus to the toolbar
 *
 *  @param c		Pointer to the toolbar.control member (at offset 0 of the toolbar)
 *  @return			1 if the control is capable of focusing
 */

int gui_toolbar_enter (CONTROL * c)
{
	return 0;
}

/** Destroy a toolbar (free any memory used by the control)
 *
 *  @param c		Pointer to the toolbar.control member (at offset 0 of the toolbar)
 */

void gui_toolbar_destructor (CONTROL * c)
{
	TOOLBAR * toolbar = (TOOLBAR *)c;

	if (toolbar->tools_type)
		free (toolbar->tools_type);
	if (toolbar->tools)
		free (toolbar->tools);

	gui_container_destructor (c);
}

/** Initialize the data members of a toolbar class
 *
 *	@param toolbar		Pointer to the toolbar object
 **/

void gui_toolbar_init (TOOLBAR * toolbar)
{
	gui_container_init (&toolbar->container, 0, 14);
	toolbar->container.control.enter = gui_toolbar_enter;
	toolbar->container.control.destructor = gui_toolbar_destructor;
	toolbar->container.control.draw = gui_toolbar_draw;
	toolbar->tools = 0;
	toolbar->tools_type = 0;
	toolbar->tools_allocated = 0;
	toolbar->tools_count = 0;
	toolbar->pressed = -1;
	toolbar->container.control.hresizable = 1;
	toolbar->container.control.innerdrag = 0;
}

/** Create a new toolbar control
 *
 *	@return				Pointer to the new control
 **/

CONTROL * gui_toolbar_new ()
{
	TOOLBAR * toolbar;

	toolbar = (TOOLBAR *)malloc(sizeof(TOOLBAR));
	if (toolbar == NULL)
		return NULL;
	gui_toolbar_init (toolbar);
	return &toolbar->container.control;
}
