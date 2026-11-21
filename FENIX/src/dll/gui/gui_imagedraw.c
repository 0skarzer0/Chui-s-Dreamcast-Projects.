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
 *  Contains CONTROL:IMAGEEDITOR drawing support functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/* ---- Data ---- */

typedef struct _pixel
{
	int x;
	int y;
}
PIXEL;

typedef struct _span
{
	int x;
	int y;
	int width;
}
SPAN;

static PIXEL * pixel = NULL;
static int     pixel_count = 0;
static int     pixel_allocated = 0;

static SPAN *  span = NULL;
static int     span_count = 0;
static int     span_allocated = 0;

static GRAPH * border = NULL;
static int     border_dirty = 1;
static REGION  border_bbox;
static GRAPH * border_brush = NULL;

/** Used when brush is NULL (one pixel brush will be created here) */
static GRAPH * null_brush = NULL;

/* ---- Data end ---- */


/** Returns the bounding box of the current drawing
 *
 *	@param bbox		(out) Pointer to region to be filled with the bounding box
 *	@param dest 	      Pointer to the drawing object (to crop bounding box to its dimensions)
 *  @param brush		  Pointer to the brush object (to enlarge bounding box)
 *	@return				  0 if there is nothing to draw, 1 otherwise
 */

int gdr_getboundingbox (REGION * bbox, GRAPH * dest, GRAPH * brush)
{
	int minx, miny, maxx, maxy, i;

	if (pixel_count == 0 && span_count == 0)
		return 0;

	/* Initialize the bounding box */

	if (pixel_count == 0)
	{
		minx = span[0].x;
		miny = span[0].y;
		maxx = span[0].width + minx - 1;
		maxy = span[0].y;
	}
	else
	{
		minx = maxx = pixel[0].x;
		miny = maxy = pixel[0].y;
	}

	/* Enlarge it to include all pixels */

	for (i = 0 ; i < pixel_count ; i++)
	{
		if (minx > pixel[i].x)
			minx = pixel[i].x;
		if (maxx < pixel[i].x)
			maxx = pixel[i].x;
		if (miny > pixel[i].y)
			miny = pixel[i].y;
		if (maxy < pixel[i].y)
			maxy = pixel[i].y;
	}

	/* Enlarge it to include brush size */

	if (pixel_count > 0 && brush != NULL)
	{
		int centerx, centery;

		centerx = brush->width / 2;
		centery = brush->height / 2;

		if (brush->ncpoints > 0 && brush->cpoints[0].x != CPOINT_UNDEFINED)
		{
			centerx = brush->cpoints[0].x;
			centery = brush->cpoints[0].y;
		}

		minx -= centerx;
		miny -= centery;
		maxx += brush->width - centerx;
		maxy += brush->height - centery;
	}

	/* Enlarge it to include fill/interior filling spans */

	for (i = 0 ; i < span_count ; i++)
	{
		if (minx > span[i].x)
			minx = span[i].x;
		if (maxx < span[i].x + span[i].width - 1)
			maxx = span[i].x + span[i].width - 1;
		if (miny > span[i].y)
			miny = span[i].y;
		if (maxy < span[i].y)
			maxy = span[i].y;
	}

	/* Crop to bitmap dimensions */

	if (minx < 0)
		minx = 0;
	if (miny < 0)
		miny = 0;
	if (maxx < 0)
		maxx = 0;
	if (maxy < 0)
		maxy = 0;
	if (minx > dest->width-1)
		minx = dest->width-1;
	if (maxx > dest->width-1)
		maxx = dest->width-1;
	if (miny > dest->height-1)
		miny = dest->height-1;
	if (maxy > dest->height-1)
		maxy = dest->height-1;

	/* Return the result */

	bbox->x  = minx;
	bbox->x2 = maxx;
	bbox->y  = miny;
	bbox->y2 = maxy;
	return 1;
}

/** Add a pixel to the current drawing
 *
 *  @param x		X Coordinate
 *	@param y		Y Coordinate
 *	@return			Current pixel count
 */

int gdr_addpixel (int x, int y)
{
	if (pixel_count == pixel_allocated)
	{
		pixel_allocated += 128;
		pixel = (PIXEL *) realloc (pixel, sizeof(PIXEL) * pixel_allocated);
	}
	if (pixel != NULL)
	{
		border_dirty = 1;
		pixel[pixel_count].x = x;
		pixel[pixel_count].y = y;
		return ++pixel_count;
	}
	return pixel_count;
}

/** Add an horizontal filled line to the current drawing
 *
 *  @param x		X Coordinate
 *	@param y		Y Coordinate
 *	@param width	Exact width in pixels
 *	@return			Current span count
 */

int gdr_addspan (int x, int y, int width)
{
	if (span_count == span_allocated)
	{
		span_allocated += 64;
		span = (SPAN *)realloc (span, sizeof(SPAN) * span_allocated);
	}
	if (span != NULL)
	{
		span[span_count].x = x;
		span[span_count].y = y;
		span[span_count].width = width;
		return ++span_count;
	}
	return span_count;
}

/** Reset the current drawing
 */

void gdr_reset()
{
	pixel_count  = 0;
	span_count   = 0;
	border_dirty = 1;
}

/** Truncate the current drawing border to the first N points submitted
 *
 *	@param count		Number of pixels (0 <= count <= current count)
 */

void gdr_truncate (int count)
{
	if (count < pixel_count && count >= 0)
	{
		pixel_count  = count;
		border_dirty = 1;
	}
}

/** Create an 8-bits bitmap with the rendering of the current
 *  drawing border, using the given brush. The brush (and the result drawing)
 *  are not palette-based, but each pixel is a magnitude value instead,
 *  used for compositing thereafter. This function is designed
 *  to be used internally by gdr_render.
 *
 *  @param brush			Pointer to the brush graphic (must be 8 bits)
 *	@param bbox  	[out]	Pointer to a region to hold the bounding box
 *	@return					Pointer to the new graphic or NULL if error
 */

GRAPH * gdr_renderborder (GRAPH * brush, REGION * bbox)
{
	GRAPH * graph;
	PIXEL brush_center;
	int i, x, y, bx, by;

	if (!brush || !bbox || !pixel_count)
		return NULL;

	/* Calculate the bounding box of the current drawing */

	bbox->x  = INT_MAX;
	bbox->x2 = INT_MIN;
	bbox->y  = INT_MAX;
	bbox->y2 = INT_MIN;

	for (i = 0 ; i < pixel_count ; i++)
	{
		if (bbox->x  > pixel[i].x)
			bbox->x  = pixel[i].x;
		if (bbox->x2 < pixel[i].x)
			bbox->x2 = pixel[i].x;
		if (bbox->y  > pixel[i].y)
			bbox->y  = pixel[i].y;
		if (bbox->y2 < pixel[i].y)
			bbox->y2 = pixel[i].y;
	}

	/* Given the brush dimensions, adjust the bounding box */
	
	if (!brush->ncpoints || brush->cpoints[0].x == CPOINT_UNDEFINED)
	{
		brush_center.x = brush->width/2;
		brush_center.y = brush->height/2;
	}
	else
	{
		brush_center.x = brush->cpoints[0].x;
		brush_center.y = brush->cpoints[0].y;
	}

	bbox->x  -= brush_center.x;
	bbox->y  -= brush_center.y;
	bbox->x2 += brush->width - brush_center.x;
	bbox->y2 += brush->height - brush_center.y;

	/* Create the result graph */

	graph = bitmap_new (0, bbox->x2 - bbox->x + 1, bbox->y2 - bbox->y + 1, 8);
	gr_clear (graph);
	
	for (i = 0 ; i < pixel_count ; i++)
	{
		bx = pixel[i].x - brush_center.x;
		by = pixel[i].y - brush_center.y;

		for (y = by ; y < by + brush->height ; y++)
		{
			Uint8 * src = (Uint8 *)brush->data + brush->pitch * (y - by);
			Uint8 * ptr = (Uint8 *)graph->data + graph->pitch * (y - bbox->y) + (bx - bbox->x);

			for (x = bx ; x < bx + brush->width ; x++, src++, ptr++)
			{
				if (*src > *ptr)
					*ptr = *src;
			}
		}
	}

	return graph;
}

/** Render the current drawing, using constant colors
 *  for border and contens.
 *
 *	@param graph		Destination graphic
 *  @param fg			Outer color (border)
 *	@param bg			Inner color
 *  @param alpha		Alpha value (255 for opaque, 0 transparent)
 *	@param brush		Current brush
 */

void gdr_renderbycolor (GRAPH * graph, int fg, int bg, int alpha, GRAPH * brush)
{
	int i;
	int factor[256];
	int r, g, b;
	int x, y;
	SDL_Color color[256];

	if (graph == NULL)
		return;

	graph->modified = 1;

	/* Render the internal part */

	gr_setcolor(bg);
	gr_setalpha(alpha);
	for (i = 0 ; i < span_count ; i++)
		gr_hline (graph, NULL, span[i].x, span[i].y, span[i].width);
	gr_setalpha(255);

	/* Render the border */

	if (brush == NULL || brush->depth != 8)
	{
		if (null_brush == NULL)
		{
			null_brush = bitmap_new (0, 1, 1, 8);
			*(Uint8 *)null_brush->data = 255;
		}
		brush = null_brush;
	}

	if (border_dirty || border == NULL || border_brush != brush)
	{
		if (border)
			bitmap_destroy(border);
		border = gdr_renderborder (brush, &border_bbox);
		border_brush = brush;
	}

	if (border != NULL)
	{
		if (graph->depth == 1)
		{
			/* 1 bit per pixel (very simple version, sets pixels to fg) */

			for (y = 0 ; y < graph->height ; y++)
			{
				for (x = 0 ; x < graph->width ; x++)
				{
					if (gr_get_pixel(border, x, y))
						gr_put_pixel(graph, x + border_bbox.x, y + border_bbox.y, fg);
				}
			}
		}
		else if (graph->depth == 8)
		{
			/* 8 bits version (slow, uses alpha blending and find_nearest_color each pixel) */

			Uint8 * ptr;
			Uint8 * src;
			Uint8 color8[256];

			gr_get_rgb (fg, &r, &g, &b);
			r *= alpha;
			g *= alpha;
			b *= alpha;

			for (i = 0 ; i < 256 ; i++)
			{
				color8[i] = gr_find_nearest_color (r*i/65025, g*i/65025, b*i/65025);
				color[i]  = gpalette[color8[i]];
				factor[i] = 255 - (i*alpha/255);
			}

			if (!fg)
				memset (color8, 0, 256);

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint8 *)graph->data + graph->pitch * y;
				src = (Uint8 *)border->data + border->pitch * (y - border_bbox.y);

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = 0 ; x < graph->width ; x++, ptr++)
				{
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						if (factor[*src])
						{
							r = gpalette[*ptr].r * factor[*src] / 255 + color[*src].r;
							g = gpalette[*ptr].g * factor[*src] / 255 + color[*src].g;
							b = gpalette[*ptr].b * factor[*src] / 255 + color[*src].b;
							*ptr = gr_find_nearest_color (r, g, b);
						}
						else
							*ptr = color8[*src];
					}
					src++;
				}
			}
		}
		else if (graph->depth == 16)
		{
			/* 16 bits version (faster, uses pregenerated alpha tables) */

			int color16[256];
			Uint16 * alpha_table;
			Uint16 * ptr;
			Uint8  * src;

			gr_get_rgb (fg, &r, &g, &b);
			r *= alpha;
			g *= alpha;
			b *= alpha;

			for (i = 0 ; i < 256 ; i++)
			{
				color[i].r = r*i/65025;
				color[i].g = g*i/65025;
				color[i].b = b*i/65025;
				color16[i] = gr_rgb (color[i].r, color[i].g, color[i].b);
				factor[i] = 255 - (i*alpha/255);
			}

			if (!fg)
				memset (color16, 0, sizeof(color16));

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint16 *)graph->data + graph->pitch * y / 2;
				src = (Uint8  *)border->data + border->pitch * (y - border_bbox.y);

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = 0 ; x < graph->width ; x++, ptr++)
				{
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						if (factor[*src])
						{
							alpha_table = gr_alpha16(factor[*src]);
							*ptr = alpha_table[*ptr] + color16[*src];
						}
						else
							*ptr = color16[*src];
					}
					src++;
				}
			}
		}
	}
}

/** Render the current drawing, using a texture for border and contens.
 *
 *	@param graph		Destination graphic
 *	@param texture		Texture graphic
 *  @param fg			Outer color (border)
 *	@param bg			Inner color
 *  @param alpha		Alpha value (255 for opaque, 0 transparent)
 *	@param brush		Current brush
 */

void gdr_renderbytexture (GRAPH * graph, GRAPH * texture, int alpha, GRAPH * brush)
{
	int i;
	int factor[256];
	int r, g, b;
	int x, y, text_x;

	if (texture == NULL || graph == NULL)
		return;

	graph->modified = 1;

	/* Render the border */

	if (brush == NULL || brush->depth != 8)
	{
		if (null_brush == NULL)
		{
			null_brush = bitmap_new (0, 1, 1, 8);
			*(Uint8 *)null_brush->data = 255;
		}
		brush = null_brush;
	}

	if (border_dirty || border == NULL || border_brush != brush)
	{
		if (border)
			bitmap_destroy(border);
		border = gdr_renderborder (brush, &border_bbox);
		border_brush = brush;
	}

	if (border == NULL && span_count > 0)
	{
		border = bitmap_new (0, graph->width, graph->height, 8);
		border_bbox.x = 0;
		border_bbox.y = 0;
		border_bbox.x2 = graph->width-1;
		border_bbox.y2 = graph->height-1;
		gr_clear (border);
	}

	if (border != NULL)
	{
		/* Render the internal part */

		gr_setcolor(255);
		for (i = 0 ; i < span_count ; i++)
			gr_hline (border, NULL, span[i].x - border_bbox.x, span[i].y - border_bbox.y, span[i].width);

		if (graph->depth == 8 && texture->depth == 8)
		{
			/* 8 bits version (slow, uses alpha blending and find_nearest_color each pixel) */

			Uint8 * ptr;
			Uint8 * src;
			Uint8 * tex;

			for (i = 0 ; i < 256 ; i++)
				factor[i] = 255 - (i*alpha/255);

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint8 *)graph->data + graph->pitch * y;
				src = (Uint8 *)border->data + border->pitch * (y - border_bbox.y);
				tex = (Uint8 *)texture->data + texture->pitch * (y % texture->height);

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = text_x = 0 ; x < graph->width ; x++, tex++, text_x++, ptr++)
				{
					if (text_x == texture->width)
					{
						tex -= texture->width;
						text_x = 0;
					}
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						if (factor[*src])
						{
							r = (gpalette[*ptr].r * factor[*src] + gpalette[*tex].r * (255 - factor[*src])) / 255 ;
							g = (gpalette[*ptr].g * factor[*src] + gpalette[*tex].g * (255 - factor[*src])) / 255 ;
							b = (gpalette[*ptr].b * factor[*src] + gpalette[*tex].b * (255 - factor[*src])) / 255 ;
							*ptr = gr_find_nearest_color (r, g, b);
						}
						else
							*ptr = *tex;
					}
					src++;
				}
			}
		}
		else if (graph->depth == 8 && texture->depth == 16)
		{
			/* 8 bits version (slow, uses alpha blending and find_nearest_color each pixel) */

			Uint8 * ptr;
			Uint8 * src;
			Uint16 * tex;
			int r, g, b;

			for (i = 0 ; i < 256 ; i++)
				factor[i] = 255 - (i*alpha/255);

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint8 *)graph->data + graph->pitch * y;
				src = (Uint8 *)border->data + border->pitch * (y - border_bbox.y);
				tex = (Uint16 *)texture->data + texture->pitch * (y % texture->height) / 2;

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = text_x = 0 ; x < graph->width ; x++, tex++, text_x++, ptr++)
				{
					if (text_x == texture->width)
					{
						tex -= texture->width;
						text_x = 0;
					}
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						gr_get_rgb (*tex, &r, &g, &b);
						if (factor[*src])
						{
							r = (gpalette[*ptr].r * factor[*src] + r * (255 - factor[*src])) / 255 ;
							g = (gpalette[*ptr].g * factor[*src] + g * (255 - factor[*src])) / 255 ;
							b = (gpalette[*ptr].b * factor[*src] + b * (255 - factor[*src])) / 255 ;
							*ptr = gr_find_nearest_color (r, g, b);
						}
						else
							*ptr = gr_find_nearest_color (r, g, b);
					}
					src++;
				}
			}
		}
		else if (graph->depth == 16 && texture->depth == 16)
		{
			/* 16 bits version (faster, uses pregenerated alpha tables) */

			Uint16 * alpha_table_src;
			Uint16 * alpha_table_dest;
			Uint16 * ptr;
			Uint16 * tex;
			Uint8  * src;

			for (i = 0 ; i < 256 ; i++)
				factor[i] = 255 - (i*alpha/255);

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint16 *)graph->data + graph->pitch * y / 2;
				src = (Uint8  *)border->data + border->pitch * (y - border_bbox.y);
				tex = (Uint16 *)texture->data + texture->pitch * (y % texture->height) / 2;

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = text_x = 0 ; x < graph->width ; x++, ptr++, tex++, text_x++)
				{
					if (text_x == texture->width)
					{
						text_x = 0;
						tex -= texture->width;
					}
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						if (factor[*src])
						{
							alpha_table_src = gr_alpha16(255-factor[*src]);
							alpha_table_dest = gr_alpha16(factor[*src]);
							*ptr = alpha_table_dest[*ptr] + alpha_table_src[*tex];
						}
						else
							*ptr = *tex;
					}
					src++;
				}
			}
		}
		else if (graph->depth == 16 && texture->depth == 8)
		{
			/* 16 bits version (faster, uses pregenerated alpha tables) */

			Uint16 * alpha_table_src;
			Uint16 * alpha_table_dest;
			Uint16 * ptr;
			Uint8  * tex;
			Uint8  * src;

			for (i = 0 ; i < 256 ; i++)
				factor[i] = 255 - (i*alpha/255);

			for (y = 0 ; y < graph->height ; y++)
			{
				if (y < border_bbox.y)
					continue;
				if (y > border_bbox.y2)
					break;

				ptr = (Uint16 *)graph->data + graph->pitch * y / 2;
				src = (Uint8  *)border->data + border->pitch * (y - border_bbox.y);
				tex = (Uint8  *)texture->data + texture->pitch * (y % texture->height);

				if (border_bbox.x < 0)
					src += -border_bbox.x;

				for (x = text_x = 0 ; x < graph->width ; x++, ptr++, tex++, text_x++)
				{
					if (text_x == texture->width)
					{
						text_x = 0;
						tex -= texture->width;
					}
					if (x < border_bbox.x)
						continue;
					if (x > border_bbox.x2)
						break;
					if (*src && factor[*src] < 248)
					{
						if (factor[*src])
						{
							alpha_table_src = gr_alpha16(255-factor[*src]);
							alpha_table_dest = gr_alpha16(factor[*src]);
							*ptr = alpha_table_dest[*ptr] + alpha_table_src[colorequiv[*tex]];
						}
						else
							*ptr = colorequiv[*tex];
					}
					src++;
				}
			}
		}
	}
}

/** Add a line of pixels to the current border
 *
 *  @param x		X coordinate of the first point of the line
 *	@param y		Y coordinate of the first point of the line
 *  @param x2		X coordinate of the second point of the line
 *	@param y2		Y coordinate of the second point of the line
 */

void gdr_addline (int x1, int y1, int x2, int y2)
{
	int dx  = abs(x2-x1);
	int dy  = abs(y2-y1);
	int y   = y1;
	int x   = x1;
	int eps = 0;

	if (x2 < x1)
	{
		x = x2;
		y = y2;
		x2 = x1;
		y2 = y1;
		x1 = x;
		y1 = y;
	}

	if (y2 >= y1)
	{
		if (dx > dy)
		{
			for (x = x1 ; x <= x2 ; x++)
			{
				gdr_addpixel (x, y);
				eps += dy;
				if ((eps << 1) >= dx)
					y++, eps -= dx;
			}
		}
		else
		{
			for (y = y1 ; y <= y2 ; y++)
			{
				gdr_addpixel (x, y);
				eps += dx;
				if ((eps << 1) >= dy)
					x++, eps -= dy;
			}
		}
	}
	else if (y2 < y1)
	{
		if (dx > dy)
		{
			for (x = x1 ; x <= x2 ; x++)
			{
				gdr_addpixel (x, y);
				eps += dy;
				if ((eps << 1) >= dx)
					y--, eps -= dx;
			}
		}
		else
		{
			for (y = y1 ; y >= y2 ; y--)
			{
				gdr_addpixel (x, y);
				eps += dx;
				if ((eps << 1) >= dy)
					x++, eps -= dy;
			}
		}
	}
}

/** Add a line of pixels to the current border, not including
 *  the first and last points (used by the pencil tool)
 *
 *  @param x		X coordinate of the first point of the line
 *	@param y		Y coordinate of the first point of the line
 *  @param x2		X coordinate of the second point of the line
 *	@param y2		Y coordinate of the second point of the line
 */

void gdr_addsegment (int x, int y, int x2, int y2)
{
	int i, steps;
	double incx, incy;

	steps =  (abs(x2-x) > abs(y2-y) ? abs(x2-x) : abs(y2-y));
	if (!steps) 
	{
		gdr_addpixel (x, y);
		return;
	}

	incx = (double)(x2-x)/steps;
	incy = (double)(y2-y)/steps;
	for (i = 1 ; i <= steps ; i++)
		gdr_addpixel (x + (int)(incx*i + 0.5), y + (int)(incy*i + 0.5));
	gdr_addpixel (x2, y2);
}

/** Add a rectangle of pixels to the current drawing (both border & inner section)
 *
 *  @param x		X coordinate of the first point 
 *	@param y		Y coordinate of the first point 
 *  @param x2		X coordinate of the second point 
 *	@param y2		Y coordinate of the second point 
 *	@param filled	1 to draw filled rectangles
 */

void gdr_addrect (int x, int y, int x2, int y2, int filled)
{
	int i;

	if (x2 < x)
	{
		i = x ;
		x = x2;
		x2 = i;
	}
	if (y2 < y)
	{
		i = y ;
		y = y2;
		y2 = i;
	}

	for (i = x ; i <= x2 ; i++)
	{
		gdr_addpixel (i, y);
		if (y2 != y)
			gdr_addpixel (i, y2);
	}
	for (i = y+1 ; i <= y2-1 ; i++)
	{
		gdr_addpixel (x, i);
		if (x != x2)
			gdr_addpixel (x2, i);
		if (x2 > x+1 && filled)
			gdr_addspan (x+1, i, x2-x-1);
	}
}

/** Add a circle to the current drawing (both border & inner section)
 *
 *  @param x		X coordinate of the center
 *	@param y		Y coordinate of the center
 *  @param radius	Radius in pixels
 *	@param filled	1 to draw interior pixels	
 */

void gdr_addcircle (int x, int y, int radius, int filled)
{
	int cx = 0, cy = radius ;
	int df = 1-radius, de = 3, dse = -2*radius + 5 ;
	int * drawn;

	radius = abs(radius);
	if (!radius)
		return;

	drawn = (int *)malloc(4*2*radius + 16);
	memset (drawn, 0, 4*2*radius + 16);

	do 
	{
		gdr_addpixel (x+cx, y+cy);
		gdr_addpixel (x+cx, y-cy);
		gdr_addpixel (x-cx, y+cy);
		gdr_addpixel (x-cx, y-cy);
		gdr_addpixel (x+cy, y+cx);
		gdr_addpixel (x+cy, y-cx);
		gdr_addpixel (x-cy, y+cx);
		gdr_addpixel (x-cy, y-cx);
		
		if (filled)
		{
			if (2*cx > 2)
			{
				if (!drawn[radius-cy])
				{
					gdr_addspan  (x-cx + 1, y-cy, 2*cx - 1);
					drawn[radius-cy] = 1;
				}
				if (!drawn[radius+cy])
				{
					gdr_addspan  (x-cx + 1, y+cy, 2*cx - 1);
					drawn[radius+cy] = 1;
				}
			}
			if (2*cy > 2)
			{
				if (!drawn[radius-cx])
				{
					gdr_addspan  (x-cy + 1, y-cx, 2*cy - 1);
					drawn[radius-cx] = 1;
				}
				if (!drawn[radius+cx])
				{
					gdr_addspan  (x-cy + 1, y+cx, 2*cy - 1);
					drawn[radius+cx] = 1;
				}
			}
		}

		cx++ ;
		if (df < 0) df += de,  de += 2, dse += 2 ;
		else	    df += dse, de += 2, dse += 4, cy-- ;
	}
	while (cx <= cy) ;

	free (drawn);
}

/** Add an ellipse to the current drawing (both border & inner section)
 *
 *  @param cx		X coordinate of the center
 *	@param cy		Y coordinate of the center
 *  @param a		Radius (horizontal)
 *	@param b		Radius (vertical)
 *	@param filled	1 to draw filled ellipses
 */

void gdr_addellipse (int cx, int cy, int a, int b, int filled)
{
	int a2 = a*a, b2 = b*b, fa2 = 4*a2, fb2 = 4*b2;
	int x, y, sigma;
	int * drawn;

	b = abs(b);
	a = abs(a);

	if (!a || !b)
		return;

	drawn = (int *)malloc(4*2*b + 16);
	memset (drawn, 0, 4*2*b + 16);

	for (x = 0, y = b, sigma = 2*b2 + a2*(1-2*b) ; b2*x <= a2*y ; x++)
	{
		gdr_addpixel (cx+x, cy+y);
		gdr_addpixel (cx-x, cy+y);
		gdr_addpixel (cx+x, cy-y);
		gdr_addpixel (cx-x, cy-y);
		
		if (x > 0 && filled)
		{
			assert (y <= b);

			if (!drawn[b-y])
			{
				gdr_addspan  (cx-x, cy-y, 2*x);
				drawn[b-y] = 1;
			}
			if (!drawn[b+y])
			{
				gdr_addspan  (cx-x, y+cy, 2*x);
				drawn[b+y] = 1;
			}
		}

		if (sigma >= 0)
		{
			sigma += fa2 * (1-y);
			y--;
		}
		sigma += b2 * (4*x + 6);
	}

	for (x = a, y = 0, sigma = 2*a2 + b2*(1-2*a) ; a2*y <= b2*x ; y++)
	{
		gdr_addpixel (cx+x, cy+y);
		gdr_addpixel (cx-x, cy+y);
		gdr_addpixel (cx+x, cy-y);
		gdr_addpixel (cx-x, cy-y);
		
		if (x > 0 && filled)
		{
			if (!drawn[b-y])
			{
				gdr_addspan  (cx-x, cy-y, 2*x);
				drawn[b-y] = 1;
			}
			if (!drawn[b+y])
			{
				gdr_addspan  (cx-x, y+cy, 2*x);
				drawn[b+y] = 1;
			}
		}

		if (sigma >= 0)
		{
			sigma += fb2 * (1-x);
			x--;
		}
		sigma += a2 * (4*y + 6);
	}

	free (drawn);
}

/* Flood-fill support data */

#define FILLMAX 1000

typedef struct _fillelement
{	
	int mylx, myrx;			/* Left/right X coordinate of the element */
	int myy;				/* Line of the element */
	int dadlx, dadrx;		/* Left/right X coordinates of the father */
	int mydir;				/* Above or below parent (1 or -1) */
}
FILLELEMENT;

FILLELEMENT fillstack[FILLMAX];
FILLELEMENT * fillsp = fillstack;

static void fill_push (int left, int right, int dadl, int dadr, int y, int dir)
{
	if (fillsp < &fillstack[FILLMAX])
	{
		fillsp->mylx = left;
		fillsp->myrx = right;
		fillsp->dadlx = dadl;
		fillsp->dadrx = dadr;
		fillsp->myy = y;
		fillsp->mydir = dir;
		fillsp++;
	}
}

static void fill_pop (int * lx, int * rx, int * dadl, int * dadr, int * y, int * dir)
{
	if (fillsp > fillstack)
	{
		fillsp--;
		*lx = fillsp->mylx;
		*rx = fillsp->myrx;
		*y = fillsp->myy;
		*dir = fillsp->mydir;
		*dadl = fillsp->dadlx;
		*dadr = fillsp->dadrx;
	}
}

static void fill_stack (int dir, int dadlx, int dadrx, int lx, int rx, int y)
{
	int pushrx = rx + 1;
	int pushlx = lx - 1;
	fill_push (lx, rx, pushlx, pushrx, y+dir, dir);
	if (rx > dadrx)
		fill_push (dadrx + 1, rx, pushlx, pushrx, y-dir, -dir);
	if (lx < dadlx)
		fill_push (lx, dadlx-1, pushlx, pushrx, y-dir, -dir);
}

/** Add a seed fill zone to the current drawing, as horizontal spans
 *
 *	@param x		X coordinate of original seed
 *	@param y		Y coordinate of original seed
 *	@param graph	Pointer to graphic object
 *  @param type		Fill type (0: fill color; 1: fill to border color)
 *  @param border	Border color
 */

void gdr_addfill (int x, int y, GRAPH * graph, int type, int border)
{
	int color;
	int lx, rx;
	int dadl, dadr;
	int dir, wasin;
	GRAPH * mask ;

#   define INSIDE(x,y) \
	( type == 0 ? gr_get_pixel(graph, x, y) == color : \
	              gr_get_pixel(graph, x, y) != border )

	/* Create a fill mask */
	mask = bitmap_new (0, graph->width, graph->height, 8);
	gr_clear (mask);

	color = gr_get_pixel (graph, x, y);

	/* Find the span containing the seed point */
	lx = rx = x;
	while (lx > 0 && gr_get_pixel(graph, lx, y) == color)
		lx--;
	while (lx < graph->width && gr_get_pixel(graph, rx, y) == color)
		rx++;
	lx++, rx--;

	gdr_addspan (lx, y, rx-lx+1);
	gr_hline (mask, 0, lx, y, rx-lx+1);
	fill_push (lx, rx, lx, rx, y+1, 1);
	fill_push (lx, rx, lx, rx, y-1, -1);

	while (fillsp > fillstack)
	{
		fill_pop (&lx, &rx, &dadl, &dadr, &y, &dir);
		if (y < 0 || y >= graph->height)
			continue;
		x = lx+1;
		wasin = (INSIDE(lx,y) && !gr_get_pixel(mask, lx, y));
		if (wasin)
		{
			gr_put_pixel (mask, lx, y, 1);
			if (lx > 0)
			{
				lx--;
				while (lx >= 0 && INSIDE(lx,y) && !gr_get_pixel(mask, lx, y))
				{
					gr_put_pixel (mask, lx, y, 1);
					lx--;
				}
				lx++;
			}
		}
		while (x < graph->width)
		{
			if (wasin)
			{
				if (INSIDE(x,y) && !gr_get_pixel(mask, x, y))
					gr_put_pixel (mask, x, y, 1);
				else
				{
					gdr_addspan (lx, y, x-lx);
					fill_stack (dir, dadl, dadr, lx, x-1, y);
					wasin = 0;
				}
			}
			else
			{
				if (x > rx)
					break;
				if (INSIDE(x,y) && !gr_get_pixel(mask, x, y))
				{
					gr_put_pixel (mask, x, y, 1);
					wasin = 1;
					lx = x;
				}
			}
			x++;
		}
		if (wasin)
		{
			gdr_addspan (lx, y, x-lx);
			fill_stack (dir, dadl, dadr, lx, x-1, y);
		}
	}

#   undef INSIDE

	bitmap_destroy (mask);
}

/** Add a curve to the current drawing
 *
 *  @param x1			Point 1 X coordinate
 *  @param y1			Point 1 Y coordinate
 *  @param x2			Point 2 X coordinate (control node, left)
 *  @param y2			Point 2 Y coordinate
 *  @param x3			Point 3 X coordinate (control node, right)
 *  @param y3			Point 3 Y coordinate
 *  @param x4			Point 4 X coordinate (ending)
 *  @param y4			Point 4 Y coordinate
 */

void gdr_addcurve (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	int level = 8;
	float x = (float)x1, y = (float)y1;
	float xp = x, yp = y;
	float delta;
	float dx, d2x, d3x;
	float dy, d2y, d3y;
	float a, b, c;
	int i;
	int n = 1;
	
	/* Compute number of iterations */
	
	if(level < 1)
		level=1;
	if(level >= 15)
		level=15; 
	while (level-- > 0)
		n*= 2;
	delta = 1.0f / (float)n;
	
	/* Compute finite differences */
	/* a, b, c are the coefficient of the polynom in t defining the parametric curve */
	/* The computation is done independently for x and y */
	
	a = (float)(-x1 + 3*x2 - 3*x3 + x4);
	b = (float)(3*x1 - 6*x2 + 3*x3);
	c = (float)(-3*x1 + 3*x2);
	
	d3x = 6 * a * delta*delta*delta;
	d2x = d3x + 2 * b * delta*delta;
	dx = a * delta*delta*delta + b * delta*delta + c * delta;
	
	a = (float)(-y1 + 3*y2 - 3*y3 + y4);
	b = (float)(3*y1 - 6*y2 + 3*y3);
	c = (float)(-3*y1 + 3*y2);
	
	d3y = 6 * a * delta*delta*delta;
	d2y = d3y + 2 * b * delta*delta;
	dy = a * delta*delta*delta + b * delta*delta + c * delta;
	
	for (i = 0; i < n; i++) 
	{
		x += dx; dx += d2x; d2x += d3x;
		y += dy; dy += d2y; d2y += d3y;
		if((Sint16)(xp) != (Sint16)(x) || (Sint16)(yp) != (Sint16)(y))
			gdr_addline((Sint16)xp,(Sint16)yp,(Sint16)x,(Sint16)y);
		xp = x; yp = y;
	}
}
