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
 * FILE        : gui_scrollable.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_SCROLLABLE_H
#define __GUI_SCROLLABLE_H

/* ------------------------------------------------------------------------- * 
 *  SCROLLABLE CONTROL
 * ------------------------------------------------------------------------- */

#define SCROLLABLE_UNLIMITED	-1

typedef struct _scrollable
{
	CONTROL	control;		/**< Parent class */

	int		showbars;		/**< 1 if scrollbars should be shown, 0 otherwise */
	int		areawidth;		/**< Total width in pixels of the internal area */
	int		areaheight;		/**< Total height in pixels of the internal area */
	int		scrollx;		/**< Horizontal scroll position */
	int		scrolly;		/**< Vertical scroll position */
	int		border;			/**< 1 if the control has an internal border */
	int		titlex;			/**< Width in pixels of any non-scrollable area */
	int		titley;			/**< Height in pixels of any non-scrollable area */
	int		maxwidth;		/**< Maximum expansion width  (-1 unlimited) */
	int		maxheight;		/**< Maximum expansion height (-1 unlimited) */
	int		limit_area;		/**< 1 to always limit drawing area coordinates */
	int		autocenter;		/**< 1 to center an area that is smaller than control size */
	int		offsetx;		/**< Left Offset (extra border) */
	int		offsety;		/**< Top Offset (extra border) */
	int		innerdrag;		/**< The user clicked inside the control (ignore mousemove at scrollbars) */

	/* hscroll and vscroll should be consecutive in the struct */
	SCROLLBAR	hscroll;	/**< Horizontal scroll bar */
	SCROLLBAR	vscroll;	/**< Vertical scroll bar */
	SCROLLPOS	hpos;		/**< Horizontal scroll values */
	SCROLLPOS	vpos;		/**< Vertical scroll values */
	SCROLLBAR * dragbar;	/**< If dragging any scroll bar, pointer to the object */
	CONTROL   * focusbar;	/**< Pointer to the bar with the last mouseenter event */

	/** Virtual functions (in substitution of control) */
	void	(*draw) (struct _control *, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip);
	int		(*mouseenter) (struct _control *);
	int		(*mousemove) (struct _control *, int x, int y, int buttons);
	int		(*mousebutton) (struct _control *, int x, int y, int b, int pressed);
	int		(*mouseleave) (struct _control *);
	int     (*mousepointer) (struct _control *, int x, int y);
	int		(*key) (struct _control *, int scancode, int character);
	int 	(*enter) (struct _control *);
	int		(*leave) (struct _control *);
	int		(*syskey) (struct _control *, int key);
	void	(*resized) (struct _control *);
	void	(*destructor) (struct _control *);
}
SCROLLABLE;

extern void      gui_scrollable_init (SCROLLABLE * scrollable, int width, int height);
extern CONTROL * gui_scrollable_new (int width, int height);
extern void      gui_scrollable_resizearea (CONTROL *, int width, int height);
extern void		 gui_scrollable_scrollto (CONTROL *, int x, int y);
extern void		 gui_scrollable_maxsize (CONTROL *, int maxw, int maxh);

#endif
