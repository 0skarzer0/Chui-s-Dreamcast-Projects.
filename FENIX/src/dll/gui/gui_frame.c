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
 *  Contains the CONTROL:FRAME class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Draw a frame.
 *  This is a member function of the CONTROL:FRAME class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_frame_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	FRAME * frame = (FRAME *)c ;

	int highlight = !(frame->type & FRAME_LOWER) ? color_highlight : color_shadow;
	int shadow    = !(frame->type & FRAME_LOWER) ? color_shadow : color_highlight;
	int width     = c->width;
	int height    = c->height;

	/* Draw the outer rectangle */
	gr_setcolor (highlight);
	gr_hline (dest, clip, x, y, width);
	gr_vline (dest, clip, x, y, height);
	gr_setcolor (shadow);
	if (height > 2) gr_hline (dest, clip, x, y+height-1, width);
	if (width > 2)  gr_vline (dest, clip, x+width-1, y, height);

	/* Draw the inner rectangle */
	if (frame->type & FRAME_LINE)
	{
		x++, y++, width-=2, height-=2;
		gr_setcolor (shadow);
		gr_hline (dest, clip, x, y, width);
		gr_vline (dest, clip, x, y, height);
		gr_setcolor (highlight);
		if (height > 2) gr_hline (dest, clip, x, y+height-1, width);
		if (width > 2)  gr_vline (dest, clip, x+width-1, y, height);
	}
}


/** Creates a new frame control.
 *
 *  A frame is a line or rectangle in 3D. Create a frame with 0 to 2 pixels
 *  width or height to create a single line.
 * 
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param width 	Width in pixels of the frame
 *  @param height	Height in pixels of the frame
 *  @param type		Type of the frame (FRAME_RAISE, FRAME_LOWER, FRAME_LINE)
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_frame_new (int width, int height, int type)
{
	FRAME * frame ;

	/* Alloc memory for the struct */
	frame = (FRAME *) malloc(sizeof(FRAME));
	if (frame == NULL)
		return NULL;
	gui_control_init (&frame->control);

	/* Fill the rest of data members */
	frame->type = type;

	/* Fill the control control struct data members */
	frame->control.bytes = sizeof(FRAME);
	frame->control.width = width;
	frame->control.height = height;
	frame->control.hresizable = 1;
	frame->control.vresizable = 1;
	frame->control.min_height = 1;
	frame->control.min_width = 1;

	/* Fill the control control struct member functions */
	frame->control.draw = gui_frame_draw;

	return &frame->control;
}

