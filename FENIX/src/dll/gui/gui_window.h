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
 * FILE        : gui_window.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_WINDOW_H
#define __GUI_WINDOW_H

/* ------------------------------------------------------------------------- * 
 *  WINDOW
 * ------------------------------------------------------------------------- */

#define WINDOW_TEXT_MAX	64

#define DRAG_MOVE		1
#define DRAG_RESIZEH	2
#define DRAG_RESIZEV	4
#define DRAG_RESIZEHV	6

#define DRAG_ATDESKTOP	-1

#define STYLE_NORMAL			0
#define STYLE_FIXED				1
#define STYLE_DIALOG			2
#define STYLE_RESIZABLE_DIALOG	3
#define STYLE_STATIC			4
#define STYLE_DOCK_NORTH		5
#define STYLE_DOCK_SOUTH		6
#define STYLE_DOCK_WEST			7
#define STYLE_DOCK_EAST			8

/** A tension line is a strong connection between two controls
 *  in either the horizontal or vertical direction, or a connection
 *  between a control and a window border. When the window is resized,
 *  the size of all tension lines needs to be preserved.
 */

typedef struct _tension
{
	int first;		/**< Number of the first control or -1 for top or left border */
	int second;		/**< Number of the second control or -1 for bottom or right border */
	int length;		/**< Size of the tension line, in pixels */
}
TENSION;

/** Struct representing a window (that is, a control container) */
typedef struct _window
{
	CONTROL	* *	control;			/**< Pointer to a control array */
	int			control_count;		/**< Number of controls in the window */
	int			control_allocated;	/**< Size of the control array */
	CONTROL *   control_drag;		/**< Inner dragging */
	
	char		text[WINDOW_TEXT_MAX];	/**< Caption text */

	int			id;					/**< Unique identifier for the window */
	int			x;					/**< Pixel-based left position */
	int			y;					/**< Pixel-based top position */
	int			visible;			/**< 1 if the window is visible */
	int			width;				/**< Width of the window in pixels */
	int			height;				/**< Height of the window in pixels */
	int			dirty;				/**< Tension lines and original positioning info is dirty */
	int			o_width;			/**< Original width, before window resizing */
	int			o_height;			/**< Original height, before window resizing */
	int			alpha;				/**< Transparency value, 255 = opaque window */
	int			border;				/**< Size in pixel of the window border */
	int			padding;			/**< Space around window contents */
	int			caption;			/**< Height in pixel of the window caption */
	CONTROL *	focus;				/**< Pointer to the control with the focus */
	int			last_mousex;		/**< Last mouse position received or -1 if none */
	int			last_mousey;		/**< Last mouse position received or -1 if none */
	int			drag;				/**< Bit flags (DRAG_X) if the user is dragging the window */
	unsigned	innerdrag   : 1;	/**< 1 if the user is dragging inside a control */
	unsigned	focused     : 1;	/**< 1 if the window has the focus */
	unsigned	fixed       : 1;	/**< 1 if the window cannot be moved by the user */
	unsigned	stayontop   : 1;	/**< 1 if the window should stay on top */
	unsigned	resizable   : 1;	/**< 1 if the window can be resized by the user */
	unsigned	minimizable : 1;	/**< 1 if the window can be minimized to icon */
	unsigned	moveable    : 1;   	/**< 1 if the window can be moved */
	unsigned	shadow      : 1;	/**< 1 if the window has a shadow */
	unsigned	maximized   : 1;	/**< 1 if the window is maximized */
	unsigned	dontfocus   : 1;	/**< 1 if the window should not receive the focus */
	unsigned	modal       : 1;	/**< 1 if the window requires user interaction */
	unsigned	changed		: 1;	/**< 1 if the document has been modified by the user */
	unsigned	fullscreen  : 1;	/**< 1 if the window should be maximized as fullscreen */
	unsigned    redraw      : 1;    /**< 1 if the window should be redrawn at next frame */
	int			lastwidth;			/**< Width of the window at a resizing start */
	int			lastheight;			/**< Height of the window at a resizing start */
	int			dock;				/**< 0 if not dockable or dock direction otherwise */
	int			restorex;			/**< X Coordinate of restore position */
	int			restorey;			/**< Y Coordinate of restore position */
	int			restorewidth;		/**< Width in restore position */
	int			restoreheight;		/**< Height in restore position */
	int			transparent;		/**< 1 if the window is transparent */
	int			usertype;			/**< STRING ID with user-defined window type */
	int			userdata;			/**< STRING ID with user-defined extra information */
	int			userfile;			/**< STRING ID with user-defined file name */
	
	CONTROL *	maximize;			/**< Maximize button */
	CONTROL *	minimize;			/**< Minimize button */
	CONTROL *	close;				/**< Close button */
	
	CONTROL *   clicked_control[8]; /**< Array of controls the user clicked */
	CONTROL *   captured_mouse;		/**< Control with mouse capture */

	TENSION *   vlines;				/**< Vertical tension lines between controls */
	int         vlines_count;		/**< Number of vertical tension lines */
	int         vlines_allocated;	/**< Allocated space for vertical tension lines */
	TENSION *   hlines;				/**< Horizontal tension lines between controls */
	int         hlines_count;		/**< Number of horizontal tension lines */
	int         hlines_allocated;	/**< Allocated space for horizontal tension lines */
}
WINDOW;

extern WINDOW  *	gui_window_new ();
extern void			gui_window_style (WINDOW * window, int style);
extern WINDOW  *	gui_window_newt (const char * title);
extern void			gui_window_caption (WINDOW * window, const char * caption);
extern void 		gui_window_addcontrol (WINDOW * window, int x, int y, CONTROL * control);
extern void 		gui_window_destroy (WINDOW * window);
extern void 		gui_window_mousemove (WINDOW * window, int x, int y, int buttons);
extern void 		gui_window_mouseenter (WINDOW * window);
extern void 		gui_window_mouseleave (WINDOW * window);
extern void 		gui_window_mousebutton (WINDOW * window, int x, int y, int b, int p);
extern void			gui_window_enter (WINDOW * window);
extern void			gui_window_leave (WINDOW * window);
extern void			gui_window_setfocus (WINDOW * window, CONTROL * control);
extern int			gui_window_key (WINDOW * window, int scancode, int key);
extern void 		gui_window_draw (WINDOW * window, GRAPH * dest, REGION * clip);
extern void			gui_window_draw_shadow (WINDOW * w, GRAPH * dest, REGION * clip);
extern void			gui_window_resize (WINDOW * window, int width, int height);
extern void         gui_window_moveto (WINDOW *, int x, int y);
extern int			gui_window_mousepointer (WINDOW *, int x, int y);
extern CONTROL *	gui_window_findcontrol (WINDOW * window, int x, int y);
extern void         gui_window_capturemouse (WINDOW * window, CONTROL * c);
extern void         gui_window_releasemouse (WINDOW * window, CONTROL * c);

#endif
