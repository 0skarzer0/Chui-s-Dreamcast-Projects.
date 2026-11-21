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
 *  Copyright © 2003 Fenix Team
 *
 */

/*
 * FILE        : ef_slice.c
 * DESCRIPTION : Graphic effects: map "cuts"
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#define MULTIPLE_FILES
#include <fxdll.h>

#include <assert.h>

/** Returns the optimum bounding box of a map, removing any empty borders.
  * No control points will be left outside the bounding box.
  *
  *	@param graph		Original graphic
  *	@param bbox			Pointer to a valid region struct to be overwritten
  */

void bitmap_getoptimalbbox (GRAPH * graph, REGION * bbox)
{
	int minx, miny, maxx, maxy, x, y;

	/* Start with a minimal bounding box */

	minx = graph->width-1;
	miny = graph->height-1;
	maxx = 0;
	maxy = 0;

	/* Adjust the bounding box to capture all control points */

	if (graph->cpoints)
	{
		for (x = 0 ; x < graph->ncpoints ; x++)
		{
			if (graph->cpoints[x].x != CPOINT_UNDEFINED && graph->cpoints[x].y != CPOINT_UNDEFINED)
			{
				if (minx > graph->cpoints[x].x)
					minx = graph->cpoints[x].x;
				if (miny > graph->cpoints[x].y)
					miny = graph->cpoints[x].y;
				if (maxx < graph->cpoints[x].x)
					maxx = graph->cpoints[x].x;
				if (maxy < graph->cpoints[x].y)
					maxy = graph->cpoints[x].y;
			}
		}
	}

	/* Adjust the bounding box to include all non-zero pixels */
	
	if (graph->depth == 8)
	{
		/* 8-Bits version */

		for (y = 0 ; y < miny ; y++)
		{
			Uint8 * ptr = (Uint8*)graph->data + graph->pitch*y;
			for (x = 0 ; x < graph->width ; x++, ptr++)
				if (*ptr) { miny = y; break; }
		}
		for (y = graph->height-1 ; y > maxy ; y--)
		{
			Uint8 * ptr = (Uint8*)graph->data + graph->pitch*y;
			for (x = 0 ; x < graph->width ; x++, ptr++)
				if (*ptr) { maxy = y; break; }
		}
		for (x = 0 ; x < minx ; x++)
		{
			Uint8 * ptr = (Uint8*)graph->data + x;
			for (y = 0 ; y < graph->height ; y++, ptr += graph->pitch)
				if (*ptr) { minx = x; break; }
		}
		for (x = graph->width-1 ; x > maxx ; x--)
		{
			Uint8 * ptr = (Uint8*)graph->data + x;
			for (y = 0 ; y < graph->height ; y++, ptr += graph->pitch)
				if (*ptr) { maxx = x; break; }
		}
	}
	else if (graph->depth == 16)
	{
		/* 16-Bits version */

		for (y = 0 ; y < miny ; y++)
		{
			Uint16 * ptr = (Uint16*)graph->data + graph->pitch*y / 2;
			for (x = 0 ; x < graph->width ; x++, ptr++)
				if (*ptr) { miny = y; break; }
		}
		for (y = graph->height-1 ; y > maxy ; y--)
		{
			Uint16 * ptr = (Uint16*)graph->data + graph->pitch*y / 2;
			for (x = 0 ; x < graph->width ; x++, ptr++)
				if (*ptr) { maxy = y; break; }
		}
		for (x = 0 ; x < minx ; x++)
		{
			Uint16 * ptr = (Uint16*)graph->data + x;
			for (y = 0 ; y < graph->height ; y++, ptr += graph->pitch / 2)
				if (*ptr) { minx = x; break; }
		}
		for (x = graph->width-1 ; x > maxx ; x--)
		{
			Uint16 * ptr = (Uint16*)graph->data + x;
			for (y = 0 ; y < graph->height ; y++, ptr += graph->pitch / 2)
				if (*ptr) { maxx = x; break; }
		}
	}
	else 
	{
		/* Free bit depth version (slower, uses get_pixel()) */

		for (y = 0 ; y < miny ; y++)
		{
			for (x = 0 ; x < graph->width ; x++)
				if (gr_get_pixel(graph, x, y)) { miny = y; break; }
		}
		for (y = graph->height-1 ; y > maxy ; y--)
		{
			for (x = 0 ; x < graph->width ; x++)
				if (gr_get_pixel(graph, x, y)) { maxy = y; break; }
		}
		for (x = 0 ; x < minx ; x++)
		{
			for (y = 0 ; y < graph->height ; y++)
				if (gr_get_pixel(graph, x, y)) { minx = x; break; }
		}
		for (x = graph->width-1 ; x > maxx ; x--)
		{
			for (y = 0 ; y < graph->height ; y++)
				if (gr_get_pixel(graph, x, y)) { maxx = x; break; }
		}
	}

	/* Empty graphic? */

	if (maxx < minx)
		maxx = minx;
	if (maxy < miny)
		maxy = miny;

	/* Return the resulting bounding box */

	bbox->x = minx;
	bbox->x2 = maxx;
	bbox->y = miny;
	bbox->y2 = maxy;

	assert (bbox->y2 < graph->height);
	assert (bbox->x2 < graph->width);
	assert (bbox->x >= 0);
	assert (bbox->y >= 0);
}

/** Cuts part of a graphic. The graphic size will be modified
 *  to the given size and ints contents sliced. The internal
 *	zone should be smaller or equal than the whole graphic.
 *
 *  NOTE: Animations are *not* supported and will be dropped
 *
 *	@param graph		Graphic to be cut
 *	@param x			Internal "left" column (0 for leftmost)
 *	@param y			Internal "top" line (0 for topmost)
 *	@param width		Width of the internal zone
 *	@param height		Height of the internal part
 */

void bitmap_slice (GRAPH * graph, int x, int y, int width, int height)
{
	int     i;
	Uint8 * data;
	GRAPH * result;

	/* Check parameters */

	if (x < 0 || y < 0 || width+x > graph->width || height+y > graph->height)
	{
		gr_error ("Invalid slice params");
		return;
	}
	if (x == 0 && y == 0 && width == graph->width && height == graph->height)
		return;

	/* Change all control points to the new position */

	if (!graph->cpoints)
		bitmap_add_cpoint (graph, graph->width/2, graph->height/2);
	else if (graph->cpoints[0].x == CPOINT_UNDEFINED && graph->cpoints[0].y == CPOINT_UNDEFINED)
	{
		graph->cpoints[0].x = graph->width/2;
		graph->cpoints[0].y = graph->height/2;
	}

	for (i = 0 ; i < graph->ncpoints; i++)
	{
		if (graph->cpoints[i].x != CPOINT_UNDEFINED)
		{
			graph->cpoints[i].x -= x;
			graph->cpoints[i].y -= y;
		}
	}

	/* Create a new data field with a new dword-aligned pitch */

	result = bitmap_new (0, width, height, graph->depth);
	if (!result) return;
	data = result->data;

	/* Copy the relevant bitmap data */

	for (i = 0 ; i < height ; i++)
	{
		memcpy ((Uint8*)data + result->pitch*i,
				(Uint8*)graph->data + graph->pitch*(y+i) + graph->depth*x/8,
				width * graph->depth/8);
	}

	/* Set the new graph data */

	result->data = graph->data;
	graph->data = data;
	graph->pitch = result->pitch;
	graph->width = result->width;
	graph->widthb = result->widthb;
	graph->height = result->height;
	graph->modified = 1;
	bitmap_destroy(result);
}

/** Extract part of a graphic as a new one. The new graphic does not
 *  have any control point or animation.
 *
 *	@param graph		Source graphic
 *	@param bbox			Bounding box of an internal rectangle
 *	@return				Pointer to a new graphic with the extracted part
 */

GRAPH * bitmap_extract (GRAPH * graph, REGION * bbox)
{
	Uint8 * dstptr;
	Uint8 * srcptr;
	GRAPH * result;
	int y;
	int size;
   
	size = (bbox->x2 - bbox->x + 1) * graph->depth / 8;
	
	assert (bbox && bbox->x >= 0 && bbox->y >= 0 &&
	        bbox->x2 < graph->width && bbox->y2 < graph->height);
	assert (bbox->x2 >= bbox->x && bbox->y2 >= bbox->y);

	result = bitmap_new (graph->code, bbox->x2 - bbox->x + 1, bbox->y2 - bbox->y + 1, graph->depth);

	dstptr = (Uint8 *)result->data;
	srcptr = (Uint8 *)graph->data + graph->pitch * bbox->y + bbox->x * graph->depth / 8;

	for (y = 0 ; y < result->height ; y++)
	{
		memcpy (dstptr, srcptr, size);
		dstptr += result->pitch;
		srcptr += graph->pitch;
	}
	return result;
}

/** Copy a smaller graphic into another. Requires at least 8 bit color depth.
 *
 *	@param dest			Destination graphic
 *	@param src			Source graphic
 *	@param bbox			Bounding box (with top-left and src size)
 */

void bitmap_subcopy (GRAPH * dest, GRAPH * src, REGION * bbox)
{
	Uint8 * dstptr;
	Uint8 * srcptr;
	int size = src->depth * src->width / 8;
	int y;

	assert (bbox && bbox->x >= 0 && bbox->y >= 0 &&
	        bbox->x2 < dest->width && bbox->y2 < dest->height);
	assert (bbox->x2 - bbox->x + 1 == src->width &&
		    bbox->y2 - bbox->y + 1 == src->height);
	assert (dest->depth == src->depth);

	dstptr = (Uint8 *)dest->data + dest->pitch * bbox->y + bbox->x * dest->depth / 8;
	srcptr = (Uint8 *)src->data;

	for (y = 0 ; y < src->height ; y++)
	{
		memcpy (dstptr, srcptr, size);
		dstptr += dest->pitch;
		srcptr += src->pitch;
	}
}
