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
 * FILE        : gui_control.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_CONTROL_H
#define __GUI_CONTROL_H

/* ------------------------------------------------------------------------- * 
 *  CONTROL
 * ------------------------------------------------------------------------- */

#define BG_TRANSPARENT		0
#define BG_SOLID			1
#define BG_WINDOW			2

#define ACTION_LEFTCLICK	0x0001
#define ACTION_RIGHTCLICK	0x0002
#define ACTION_MIDDLECLICk	0x0004
#define ACTION_WHEELUP		0x0008
#define ACTION_WHEELDOWN	0x0010
#define ACTION_DOUBLECLICK	0x0020
#define ACTION_KEY			0x0040
#define ACTION_ENTER		0x0080
#define ACTION_LEAVE		0x0100
#define ACTION_MOUSEMOVE	0x0200
#define ACTION_CHANGE		0x0400

/** This struct contains the control of all controls.
 *
 *  A control struct must begin with a CONTROL struct at offset 0,
 *  and then can have any additional data required, but it must be
 *  all of it self-contained (no pointers to dynamic memory) and
 *  the full size of the struct should be written in the "bytes"
 *  member variable by the constructor.
 */

typedef struct _control
{
	int			bytes;				/**< Memory used by the full control struct */
	int			x;					/**< Pixel-based left position */
	int			y;					/**< Pixel-based top position */
	int			width;				/**< Width of the control in pixels */
	int			height;				/**< Height of the control in pixels */
	int			redraw;				/**< 1 if the control needs to be updated [unused] */
	int			highlight;			/**< 1 if control has the mouse highlighting it */
	unsigned	focused    : 1;		/**< 1 if control has the focus */
	unsigned	innerdrag  : 1;		/**< 1 if control supports inner dragging */
	unsigned	hidden     : 1;		/**< 1 if control should not be drawn */
	unsigned	draggable  : 2;		/**< 1 if control contains a draggable and clicked object */
	unsigned	vresizable : 1;		/**< 1 if control can be resized in the Y coordinate */
	unsigned	hresizable : 1;		/**< 1 if control can be resized in the X coordinate */
	unsigned	allkeys    : 1;		/**< 1 if control handles all key pressed, including ALT+mouse button */
	int			drstring;			/**< String code for draggable object */
	GRAPH	  * drgraph;			/**< Graphic for draggable object */
	int			bgmode;				/**< Background type (see BG_X constants) */
	int			min_width;			/**< Minimal width in pixels */
	int			max_width;			/**< Maximal width in pixels */
	int			min_height;			/**< Minimal height in pixels */
	int			max_height;			/**< Maximal height in pixels */
	int			o_x;				/**< Original position, before window resizing */
	int			o_y;				/**< Original position, before window resizing */
	int			o_width;			/**< Original width, before window resizing */
	int			o_height;			/**< Original height, before window resizing */
	int			width_step;			/**< The width must be multiple of this value */
	int			height_step;		/**< The height must be multiple of this value */
	int			pointer;			/**< POINTER_XXX constant with the mouse pointer to use */
	int			tooltip;			/**< Tooltip string code */
	int			actionflags;		/**< Action flags */

	struct _window * window;		/**< Parent window */
	struct _control * parent;		/**< Rare cases */

	/** Draw the control in a destination buffer, using the given
	 *  coordinates as top-left and clipping it to the given region */
	void	(*draw) (struct _control *, GRAPH * dest, int x, int y, REGION * clip);

	/** Function called when the mouse enters the control */
	int		(*mouseenter) (struct _control *);

	/** Function called when the mouse moves */
	int		(*mousemove) (struct _control *, int x, int y, int buttons);

	/** Function called when a button is pressed */
	int		(*mousebutton) (struct _control *, int x, int y, int b, int pressed);

	/** Function called when the mouse leaves the control */
	int		(*mouseleave) (struct _control *);

	/** Function called in a keypress event 
	 *  @param scancode		Keyboard scan code of the key pressed
	 *  @param character	ASCII code of the character or 0 if N/A */
	int		(*key) (struct _control *, int scancode, int character);

	/** Function called when the control gains focus 
	 *  @return 1 if the control is capable of gaining focus, 0 otherwise */
	int 	(*enter) (struct _control *);

	/** Function called when the control loses the focus */
	int		(*leave) (struct _control *);

	/** Function called when a system key (ALT+Letter) is pressed
	 *  @param key		Character (uppercase) of the key pressed, or 0
	 *					to test if the control supports any key
	 *  @return			1 if the direct access key is matched, 0 otherwise
	 */
	int		(*syskey) (struct _control *, int key);

	/** Function called when a control is resized. The new size is
	 *  stored at the (width, height) members before calling the function.
	 *  You can limit the minimum/maximum control size with the min_/max_
	 *  class members.
	 */
	void		(*resized) (struct _control *);

	/** Function called to choose the mouse pointer
	 *
	 *	@param x			X coordinate, relative to control
	 *	@param y			Y coordinate, relative to control
	 *	@return				One of the POINTER_XXX constants
	 */
	int		(*mousepointer) (struct _control *, int x, int y);

	/** Destructor */
	void	(*destructor) (struct _control *);
}
CONTROL;

extern void gui_control_init (CONTROL * control);
extern void gui_control_destructor (CONTROL * c);

#endif
