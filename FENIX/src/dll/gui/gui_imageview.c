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
 *  Contains the CONTROL:IMAGEVIEW class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"


/** Draw an object centered in a given rectangle, but if the object does not fit, 
 *  scale it down to fill the rectangle while preserving the aspect ratio
 *
 *	@param dest		Destination drawing bitmap
 *	@param x0		Left coordinate
 *	@param y0		Top coordinate
 *	@param x1		Right coordinate
 *	@param y1		Bottom coordinate
 *	@param flags	Blit flags
 *	@param clip		Clipping region to apply or 0 if none
 *  @param graph	Origin graph
 */

void gui_scale_image (GRAPH * dest, int x0, int y0, int x1, int y1, int flags, REGION * clip, GRAPH * graph)
{
	int sx = x0 + (x1-x0+1)/2;
	int sy = y0 + (y1-y0+1)/2;
	int zx = 100;
	int zy = 100;

	if (graph->width > (x1-x0+1))
		zx = (x1-x0+1)*100/graph->width;
	if (graph->height > (y1-y0+1))
		zy = (y1-y0+1)*100/graph->height;
	if (zy < zx) 
		zx = zy;
	else
		zy = zx;

	if (graph->ncpoints && graph->cpoints[0].x != CPOINT_UNDEFINED)
	{
		sx -= graph->width * zx / 200;
		sy -= graph->height * zy / 200;
		sx += graph->cpoints[0].x * zx / 100;
		sy += graph->cpoints[0].y * zy / 100;
	}

	gr_rotated_blit (dest, clip, sx, sy, flags, 0, zx, zy, graph);
}

/** Draw an image object
 *
 *  @param c		Pointer to the control member (at offset 0 of the class)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_imageview_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	int sx, sy, i, j, c1, c2;
	int flags = 0;
	IMAGEVIEW * iv = (IMAGEVIEW *)c;
	REGION region;

	if (iv->graphid)
		iv->graph = bitmap_get(iv->fpg, iv->graphid);

	region.x = x;
	region.y = y;
	region.x2 = x+c->width-1;
	region.y2 = y+c->height-1;
	region_union (&region, clip);

	if (c->window && c->window->alpha != 255)
		flags |= (B_ALPHA | (c->window->alpha << B_ALPHA_SHIFT));

	sx = x + c->width/2;
	sy = y + c->height/2;

	/* Draw the background */

	switch (iv->bg)
	{
		case IMAGEVIEWBG_BLACK:
			gr_setcolor(0);
			gr_box (dest, clip, x, y, c->width, c->height);
			break;

		case IMAGEVIEWBG_GRAY:
			gr_setcolor(gr_rgb(128,128,128));
			gr_box (dest, clip, x, y, c->width, c->height);
			break;

		case IMAGEVIEWBG_CHESSBOARD:
			c1 = gr_rgb ( 80,  80,  80);
			c2 = gr_rgb (128, 128, 128);
			for (i = 0 ; i < c->width ; i += 8)
			{
				for (j = 0 ; j < c->height ; j += 8)
				{
					gr_setcolor (((i+j)&8) ? c1 : c2);
					gr_box (dest, &region, x+i, y+j, 8, 8);
				}
			}
			break;
	}

	/* Draw the image */

	if (!iv->graph)
		return;

	if (iv->zoom == 1.0f && !iv->scale)
	{
		if (iv->graph->cpoints && iv->graph->cpoints[0].x != CPOINT_UNDEFINED)
		{
			sx += (-iv->graph->width/2 + iv->graph->cpoints[0].x);
			sy += (-iv->graph->height/2 + iv->graph->cpoints[0].y);
		}
		gr_blit (dest, &region, sx, sy, flags, iv->graph);
	}
	else
	{
		int zx = (int)(iv->zoom * 100.0f);
		int zy = zx;

		if (iv->scale)
		{
			zx = 100 * c->width / iv->graph->width;
			zy = 100 * c->height / iv->graph->height;

			if (zx > 100) zx = 100;
			if (zy > 100) zy = 100;
			if (iv->aspect)
			{
				if (zx < zy) 
					zy = zx;
				else 
					zx = zy;
			}
		}

		if (iv->graph->cpoints && iv->graph->cpoints[0].x != CPOINT_UNDEFINED)
		{
			sx -= iv->graph->width * zx  / 200;
			sy -= iv->graph->height * zy / 200;
			sx += iv->graph->cpoints[0].x * zx / 100;
			sy += iv->graph->cpoints[0].y * zy / 100;
		}
		if (zx != 100 || zy != 100)
			gr_rotated_blit (dest, &region, sx, sy, 0, flags, zx, zy, iv->graph);
		else
			gr_blit (dest, &region, sx, sy, flags, iv->graph);
	}
}

/** Response to user's mouse clicks. The mouse wheels changes the zoom.
 *
 *  @param c		Pointer to the control member (at offset 0 of the object)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the dropdown pressed or released
 *  @param pressed	1 if the dropdown was pressed, 0 if it was released
 * */

int gui_imageview_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	IMAGEVIEW * image = (IMAGEVIEW *)c;

	if (b == 3 && pressed)
	{
		image->zoom *= 1.5f;
		return 1;
	}
	if (b == 4 && pressed)
	{
		image->zoom /= 1.5f;
		return 1;
	}
	return 0;
}

/** Create an image control. By default, an image control is not
 *  resizable and has the exact original size of the image, with no scaling.
 *
 *	@param graph	Image to be shown in the control
 *  @return			Pointer to the new object
 */

CONTROL * gui_imageview_new (GRAPH * graph)
{
	CONTROL * control;
	IMAGEVIEW * iv;
	
	control = (CONTROL *) malloc(sizeof(IMAGEVIEW));

	/* Initialize parent class members */
	gui_control_init (control);
	if (graph)
	{
		control->width = graph->width;
		control->height = graph->height;
	}
	control->hresizable = 0;
	control->vresizable = 0;
	control->draw = gui_imageview_draw;
	control->mousebutton = gui_imageview_mousebutton;
	control->bgmode = BG_WINDOW;

	/* Initialize imageview object members */
	iv = (IMAGEVIEW *)control;
	iv->graph = graph;
	iv->scale = 0;
	iv->zoom = 1.0f;
	iv->aspect = 1;
	iv->bg = IMAGEVIEWBG_WINDOW;
	iv->fpg = 0;
	iv->graphid = 0;

	return control;
}


/** Create an image control, using the given fpg and graphic number
 *
 *	@param fpg		Library number
 *	@param graphid	Graphic number
 *  @return			Pointer to the new object
 */

CONTROL * gui_imageview_new_fpg (int fpg, int graphid)
{
	CONTROL * control;
	GRAPH * graph;
	IMAGEVIEW * iv;
	
	control = (CONTROL *) malloc(sizeof(IMAGEVIEW));

	/* Initialize parent class members */
	gui_control_init (control);
	graph = bitmap_get(fpg, graphid);
	if (graph)
	{
		control->width = graph->width;
		control->height = graph->height;
	}
	control->hresizable = 0;
	control->vresizable = 0;
	control->draw = gui_imageview_draw;
	control->mousebutton = gui_imageview_mousebutton;
	control->bgmode = BG_WINDOW;

	/* Initialize imageview object members */
	iv = (IMAGEVIEW *)control;
	iv->graph = graph;
	iv->scale = 0;
	iv->zoom = 1.0f;
	iv->aspect = 1;
	iv->bg = IMAGEVIEWBG_WINDOW;
	iv->fpg = fpg;
	iv->graphid = graphid;

	return control;
}

/** Set the zoom level of an image control
 *
 *  @param imageview		Pointer to the imageview control
 *	@param zoom				New zoom level (1.0 = no zoom)
 */

void gui_imageview_zoom (CONTROL * imageview, float zoom)
{
	IMAGEVIEW * iv = (IMAGEVIEW *)imageview;
	iv->zoom = zoom;
	if (iv->graphid)
		iv->graph = bitmap_get(iv->fpg, iv->graphid);
	if (iv->graph && !iv->control.window)
	{
		imageview->width  = (int)(iv->graph->width * zoom);
		imageview->height = (int)(iv->graph->height * zoom);
	}
}


/** Change the image in an image view control
 *
 *	@param imageview		Pointer to the imageview control
 *	@param graph			Pointer to the new graphic
 */

void gui_imageview_image (CONTROL * imageview, GRAPH * graph)
{
	((IMAGEVIEW*)imageview)->graph = graph;
}

/** Change the image in an image view control, using the given FPG-ID number
 *
 *	@param imageview		Pointer to the imageview control
 *	@param graph			Pointer to the new graphic
 */

void gui_imageview_image_fpg (CONTROL * imageview, int fpg, int graphid)
{
	((IMAGEVIEW*)imageview)->fpg = fpg;
	((IMAGEVIEW*)imageview)->graphid = graphid;
	((IMAGEVIEW*)imageview)->graph = bitmap_get(fpg, graphid);
}


/** Set the background type for an image view control
 *
 *  @param imageview		Pointer to the imageview control
 *	@param background		One of the IMAGEVIEWBG_XXX constantes
 */

void gui_imageview_background (CONTROL * imageview, int bg)
{
	IMAGEVIEW * iv = (IMAGEVIEW *)imageview;
	iv->bg = bg;
	iv->control.bgmode = (iv->bg == IMAGEVIEWBG_WINDOW ? BG_WINDOW : 
						  iv->bg == IMAGEVIEWBG_TRANSPARENT ? BG_TRANSPARENT :
						  BG_SOLID);
}

/** Enable or disable automatic scaling in an image control
 *
 *  @param imageview		Pointer to the imageview control
 *	@param resize			1 to activate control resizing
 *	@param scale			1 to activate automatic scaling to control
 *	@param aspect			1 to preserve aspect ratio when scaling
 */

void gui_imageview_scale (CONTROL * imageview, int resize, int scale, int aspect)
{
	IMAGEVIEW * iv = (IMAGEVIEW *)imageview;
	iv->scale = scale;
	iv->aspect = aspect;

	if (resize)
	{
		imageview->hresizable = 1;
		imageview->vresizable = 1;
	}
	else
	{
		imageview->hresizable = 0;
		imageview->vresizable = 0;
	}
}
