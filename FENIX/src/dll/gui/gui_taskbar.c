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
 *  Contains the CONTROL:TASKBAR class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

static int gui_taskbar_compare (const TASK * a, const TASK * b)
{
	return a->id - b->id;
}

static void gui_taskbar_addtask (TASKBAR * taskbar, TASK * task)
{
	if (taskbar->task_allocated == taskbar->task_count)
	{
		taskbar->task_allocated += 4;
		taskbar->task = (TASK *) realloc(taskbar->task,
			sizeof(TASK) * taskbar->task_allocated);
	}
	taskbar->task[taskbar->task_count++] = *task;
}

/** Update a taskbar control with current window information
 *
 *	@param control		Pointer to the taskbar control
 */

void gui_taskbar_update (CONTROL * control)
{
	TASKBAR * taskbar = (TASKBAR *)control;
	WINDOW  * window;
	int       i;

	taskbar->task_count = 0;
	window = gui_desktop_firstwindow();

	while (window != NULL)
	{
		TASK task;
		
		if (!window->dontfocus)
		{
			task.id = window->id;
			task.window = window;
			task.name = window->text;
			task.active = window->visible;
			if (*task.name == '*') task.name++;
			window = gui_desktop_nextwindow(window);
			task.active = (window == NULL && task.active);
			gui_taskbar_addtask (taskbar, &task);
		}
		else
			window = gui_desktop_nextwindow(window);
	}

	if (taskbar->task_count == 0)
		return;

	qsort (taskbar->task, taskbar->task_count, sizeof(TASK), gui_taskbar_compare);

	taskbar->button_width = (control->width + 3) / taskbar->task_count;
	if (taskbar->button_width > 128)
		taskbar->button_width = 128;

	for (i = 0 ; i < taskbar->task_count ; i++)
	{
		taskbar->task[i].x  = i * taskbar->button_width;
		taskbar->task[i].x2 = (i+1) * taskbar->button_width-4;
	}
}

/** Draw a taskbar.
 *  This is a member function of the CONTROL:TASKBAR class.
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 **/

void gui_taskbar_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	int i;
	int cx, cy, cw, ch;
	TASKBAR * taskbar = (TASKBAR *) c;

	gui_taskbar_update (c);

	cw = taskbar->button_width;
	ch = c->height;

	for (i = 0 ; i < taskbar->task_count ; i++)
	{
		TASK * t = &taskbar->task[i];
		REGION r;

		cx = x + t->x;
		cy = y;
		cw = t->x2 - t->x + 1;
		gr_setcolor (t->active ? color_shadow : color_highlight);
		gr_hline (dest, clip, cx, cy, cw);
		gr_vline (dest, clip, cx, cy, ch);
		gr_setcolor (t->active ? color_highlight : color_shadow);
		gr_hline (dest, clip, cx, cy+ch-1, cw);
		gr_vline (dest, clip, cx+cw-1, cy, ch);

		if (t->active)
		{
			gr_setcolor (color_captionfg);
			gr_box (dest, clip, cx+1, cy+1, cw-2, ch-2);
			cx++;
			cy++;
		}
		else if (taskbar->hovered == i)
		{
			gr_setcolor (color_highlightface);
			gr_box (dest, clip, cx+1, cy+1, cw-2, ch-2);
		}
		else
		{
			gr_setcolor (color_face);
			gr_box (dest, clip, cx+1, cy+1, cw-2, ch-2);
		}

		r.x = cx+2;
		r.y = cy+2;
		r.x2 = cx+cw-8;
		r.y2 = cy+ch-4;
		region_union (&r, clip);

		gr_text_put (dest, &r, 0, cx+4, cy+2, t->name);
	}
}

/** Handle mouse move events
 *
 *	@param c			Pointer to the control
 **/

int gui_taskbar_mousemove (CONTROL * c, int x, int y, int buttons)
{
	TASKBAR * taskbar = (TASKBAR *)c;
	int i;

	for (i = 0 ; i < taskbar->task_count ; i++)
	{
		if (x >= taskbar->task[i].x)
			taskbar->hovered = i;
	}
	return 1;
}

int gui_taskbar_mouseleave (CONTROL * c)
{
	TASKBAR * taskbar = (TASKBAR *)c;
	taskbar->hovered = -1;
	return 1;
}

int gui_taskbar_mousebutton (CONTROL * c, int x, int y, int button, int pressed)
{
	TASKBAR * taskbar = (TASKBAR *)c;

	if (taskbar->hovered >= 0 && pressed)
	{
		if (taskbar->task[taskbar->hovered].active)
			gui_action_minimizewindow (taskbar->task[taskbar->hovered].window);
		else
			gui_desktop_bringtofront (taskbar->task[taskbar->hovered].window);
	}
	return 1;
}

/** Create a new taskbar control
 *
 *	@return				Pointer to the new control
 **/

CONTROL * gui_taskbar_new ()
{
	TASKBAR * taskbar;

	taskbar = (TASKBAR *)malloc(sizeof(TASKBAR));
	if (taskbar == NULL)
		return NULL;
	gui_control_init (&taskbar->control);

	taskbar->control.min_height = 14;
	taskbar->control.height = 14;
	taskbar->control.width = 64;
	taskbar->control.min_width = 64;
	taskbar->control.draw = gui_taskbar_draw;
	taskbar->control.mousemove = gui_taskbar_mousemove;
	taskbar->control.mouseleave = gui_taskbar_mouseleave;
	taskbar->control.mousebutton = gui_taskbar_mousebutton;
	taskbar->control.hresizable = 1;

	taskbar->task_count = 0;
	taskbar->task_allocated = 0;
	taskbar->task = 0;
	taskbar->hovered = -1;

	return &taskbar->control;
}
