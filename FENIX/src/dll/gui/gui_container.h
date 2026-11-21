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
 * FILE        : gui_container.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_CONTAINER_H
#define __GUI_CONTAINER_H

/* ------------------------------------------------------------------------- * 
 *  CONTAINER CONTROL
 * ------------------------------------------------------------------------- */

typedef struct _container
{
	CONTROL		control;		/**< Parent class */

	int			count;			/**< Number of controls */
	int			allocated;		/**< Number of pointer slots available */
	CONTROL * * contents;		/**< Pointer to an array of contens */
	CONTROL *	drag;			/**< Pointer to the control the user is dragging */
	CONTROL *	mouse;			/**< Pointer to the control at the mouse position */

	/** Virtual functions (in substitution of control) */
	void	(*draw) (struct _control *, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip);
	int		(*mouseenter) (struct _control *);
	int		(*mousemove) (struct _control *, int x, int y, int buttons);
	int		(*mousebutton) (struct _control *, int x, int y, int b, int pressed);
	int		(*mouseleave) (struct _control *);
	int		(*key) (struct _control *, int scancode, int character);
	int 	(*enter) (struct _control *);
	int		(*leave) (struct _control *);
	int		(*syskey) (struct _control *, int key);
	void	(*destructor) (struct _control *);
}
CONTAINER;

extern void      gui_container_init (CONTAINER * container, int width, int height);
extern CONTROL * gui_container_new (int width, int height);
extern void	     gui_container_add (CONTROL *, int x, int y, CONTROL *);
extern void      gui_container_move (CONTROL *, int x, int y, CONTROL *);
extern void      gui_container_remove (CONTROL *, CONTROL *);

extern void		 gui_container_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip);
extern void 	 gui_container_destructor (CONTROL * c);
extern int		 gui_container_mouseenter (CONTROL * c);
extern int		 gui_container_mouseleave (CONTROL * c);
extern int		 gui_container_mousemove (CONTROL * control, int x, int y, int buttons);
extern int		 gui_container_mousebutton (CONTROL * c, int x, int y, int b, int pressed);
extern int		 gui_container_mousepointer (CONTROL * control, int x, int y);
extern int		 gui_container_enter (CONTROL * c);
extern int		 gui_container_leave (CONTROL * c);
extern int		 gui_container_key (CONTROL * c, int scancode, int key);
extern int		 gui_container_syskey (CONTROL * c, int key);


#endif
