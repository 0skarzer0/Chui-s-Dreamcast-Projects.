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

/*
 * FILE        : gui_taskbar.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_TASKBAR_H
#define __GUI_TASKBAR_H

/* ------------------------------------------------------------------------- * 
 *  TASK BAR
 * ------------------------------------------------------------------------- */

typedef struct _task
{
	int				id;				/*< ID of the window */
	int				active;			/*< 1 if taskbar is active */
	WINDOW *		window;			/*< Pointer to the window object */
	int				x;				/*< X left coordinate of the button */
	int				x2;				/*< X right coordinate of the button */
	const char *	name;			/*< Task name (window caption) */
}
TASK;

typedef struct _taskbar
{
	CONTROL		control;
	TASK *		task;
	int			task_count;
	int			task_allocated;
	int			hovered;
	int			button_width;
}
TASKBAR;

extern CONTROL * gui_taskbar_new();

#endif
