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
 *  Contains the CONTROL:LABEL class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** Returns the pixel width of a label text (ignores & characters).
 *	Also handles multi-line labels.
 *
 *  @param text		String with the label text
 *  @returns		Width of the text in screen pixels
 */

int gui_text_width (const char * text)
{
	char car[2] = " ";
	int  width  = 0;
	int  max = 0;

	while (*text)
	{
		if (*text == '&') 
		{
			text++;
			if (!*text) text--;
		}
		if (*text == '\r')
		{
			text++;
			continue;
		}
		if (*text == '\n')
		{
			text++;
			width = 0;
			continue;
		}
		car[0] = *text++;
		width += gr_text_width (0, car);
		if (width > max)
			max = width;
	}
	return max;
}

/** Returns the pixel width of a label text (only first line).
 *	Also handles multi-line labels.
 *
 *  @param text		String with the label text
 *  @returns		Width of the text in screen pixels
 */

int gui_linewidth (const char * text)
{
	char car[2] = " ";
	int  width  = 0;

	while (*text && *text != '\n')
	{
		if (*text == '\r')
		{
			text++;
			continue;
		}
		if (*text == '&') 
		{
			text++;
			if (!*text) text--;
		}
		car[0] = *text++;
		width += gr_text_width (0, car);
	}
	return width;
}

/** Returns the height of a label text (ignores & characters)
 *
 *  @param text		String with the label text
 *  @returns		Width of the text in screen pixels
 */

int gui_text_height (const char * text, int lead)
{
	char car[2] = " ";
	int	 line_height = gr_font_get(0)->maxheight;
	int  height = line_height;

	while (*text)
	{
		if (*text == '\n') 
			height += line_height + lead;
		text++;
	}
	return height;
}

/** Writes a text label to the screen (cares about alignment and the & prefix)
 *
 *	@param dest			Destination graph
 *	@param clip			Clipping region
 *	@param x			Horizontal screen coordinate
 *	@param y			Vertical screen coordinate
 *	@param text			Label text
 *	@param alignment	Alignment: 0=LEFT; 1=CENTER; 2=RIGHT
 */

void gui_text_put (GRAPH * dest, REGION * clip, int x, int y, const char * text, int alignment, int lead)
{
	char car[2] = " ";
	int  width;
	int	 line_height = gr_font_get(0)->maxheight;
	int  base_x = x;

	if (!text)
		return;

	if (alignment == 1)
		x -= gui_linewidth(text)/2;
	else if (alignment == 2)
		x -= gui_linewidth(text);

	while (*text)
	{
		if (*text == '&') 
		{
			text++;
			if (*text)
			{
				car[0] = *text++;
				width = gr_text_width (0, car);
				gr_text_put (dest, clip, 0, x, y, car);
				gr_hline (dest, clip, x, y + gr_text_height(0, car), width);
				x += width;
				continue;
			}
		}
		if (*text == '\n')
		{
			y += line_height + lead;
			x = base_x;
			text++;
			if (alignment == 1)
				x -= gui_linewidth(text)/2;
			else if (alignment == 2)
				x -= gui_linewidth(text);
			continue;
		}
		if (*text == '\r')
		{
			text++;
			continue;
		}
		car[0] = *text++;
		gr_text_put (dest, clip, 0, x, y, car);
		x += gr_text_width (0, car);
	}
}

/** Draw a label.
 *  This is a member function of the CONTROL:LABEL class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_label_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	LABEL * label = (LABEL *)c ;
	REGION region;

	/* Create a better clipping region */
	region.x = x;
	region.y = y;
	region.x2 = x + c->width-1;
	region.y2 = y + c->height-1;
	region_union (&region, clip);

	/* Draw the text */
	gr_setcolor (color_text);
	gr_text_setcolor (color_text);

	if (label->alignment == 0)
		x += 2;
	else if (label->alignment == 1)
		x += c->width/2;
	else if (label->alignment == 2)
		x += c->width-1;

	gui_text_put (dest, &region, x, 
			y + (c->height - gui_text_height(label->text, label->lead)) / 2, 
			label->text, label->alignment, label->lead) ;
}


/** Check if the label supports the given acces key (or any at all)
 *  This is done searching the text of the label for the \b control character
 *
 *  @param control	Pointer to the label control
 *	@param key		Character pressed or 0 to check if any is present
 **/

int gui_label_syskey (CONTROL * control, int key)
{
	LABEL * label = (LABEL *) control;

	const char * ptr = label->text;

	while (*ptr)
	{
		if (*ptr == '&')
		{
			if (!key || toupper(ptr[1]) == toupper(key))
				return 1 ;
		}
		ptr++;
	}
	return 0;
}

/** Destroys a label control
 *
 *	@param control	Pointer to the object
 */

void gui_label_destructor (CONTROL * control)
{
	LABEL * label = (LABEL *) control;
	if (label->text)
		free (label->text);
	gui_control_destructor (control);
}

/** Create a new label control 
 *
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param text 	Text of the label
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_label_new (const char * text)
{
	LABEL * label ;

	/* Alloc memory for the struct */
	label = (LABEL *) malloc(sizeof(LABEL));
	if (label == NULL)
		return NULL;
	gui_control_init (&label->control);

	/* Fill the rest of data members */
	label->text = strdup(text);
	label->alignment = 0;
	label->lead = 4;

	/* Fill the control control struct data members */
	label->control.bytes = sizeof(LABEL);
	label->control.width = gui_text_width(label->text) + 12;
	label->control.height = gui_text_height(label->text, label->lead) + 6;

	/* Fill the control control struct member functions */
	label->control.draw = gui_label_draw;
	label->control.syskey = gui_label_syskey;

	/* Replace the destructor */
	label->control.destructor = gui_label_destructor;

	return &label->control;
}

/** Change the alignment of a label control
 *
 *  @param label		Pointer to the label control object
 *  @param align		New alignment (see gr_text_put)
 **/

void gui_label_alignment (CONTROL * label, int align)
{
	((LABEL *)label)->alignment = align;
}

/** Change the text of a label control
 *
 *	@param control		Pointer to the label object
 *	@param text			New text
 */

void gui_label_text (CONTROL * control, const char * text)
{
	LABEL * label = (LABEL *)control;

	if (label->text)
		free (label->text);
	label->text = strdup(text);
}
