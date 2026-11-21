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
 *  Contains the CONTROL:IMAGEEDITOR class member functions
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

#define UNDO_THRESHOLD	16*1024*1024
#define MB_EMUL         INT_MAX

int gui_imageeditor_mousemove (CONTROL * c, int x, int y, int buttons);

IMAGEEDITORTOOL * imageeditor_tools[] =
{
	&tool_pencil,
	&tool_line,
	&tool_rect,
	&tool_circle,
	&tool_cpe,
	&tool_zoom,
	&tool_select,
	&tool_wand,
	&tool_multiline,
	&tool_fill,
	&tool_polycurve,
	&tool_pixels,
	&tool_curve,
	&tool_paste,
	&tool_airbrush
};

/** Zoom to a given point
 *
 *	@param ed			Pointer to the image editor object
 *	@param x			X coordinate of point
 *	@param y			Y coordinate of point
 *	@param zoom			New zoom level
 */

void gui_imageeditor_zoomtopoint (IMAGEEDITOR * ed, int x, int y, int zoom)
{
	ed->info->zoom = zoom;
	gui_scrollable_resizearea (&ed->scrollable.control, 
		ed->graph->width * ed->info->zoom / 100, 
		ed->graph->height * ed->info->zoom / 100);
	gui_scrollable_scrollto (&ed->scrollable.control, 
		x * zoom / 100 - ed->scrollable.control.width / 2, 
		y * zoom / 100 - ed->scrollable.control.height / 2);
}

/** Move the mouse cursor (usually, because the user pressed a navigation key)
 *
 *	@param ed			Pointer to the image editor object
 *	@param x			New X coordinate (in graph)
 *	@param y			New Y coordinate (in graph)
 */

void gui_imageeditor_movecursor (IMAGEEDITOR * ed, int x, int y)
{
	CONTROL * c = (CONTROL *)ed;

	if (x < 0)
		x = 0;
	if (x >= ed->graph->width)
		x = ed->graph->width-1;
	if (y < 0)
		y = 0;
	if (y >= ed->graph->height)
		y = ed->graph->height-1;

	x = x * ed->info->zoom / 100 + ed->info->zoom/200;
	y = y * ed->info->zoom / 100 + ed->info->zoom/200;

	if (x < ed->scrollable.hpos.pos + 8 || 
		x > ed->scrollable.hpos.pos + c->width - 16)
		gui_scrollable_scrollto (c, x - c->width/2, ed->scrollable.vpos.pos);
	if (y < ed->scrollable.vpos.pos + 8 ||
		y > ed->scrollable.vpos.pos + c->height - 16)
		gui_scrollable_scrollto (c, ed->scrollable.hpos.pos, y - c->height/2);
	x -= ed->scrollable.hpos.pos;
	y -= ed->scrollable.vpos.pos;
	x += ed->scrollable.offsetx + ed->scrollable.border + ed->scrollable.control.x - 1;
	y += ed->scrollable.offsety + ed->scrollable.border + ed->scrollable.control.y - 1;
	
	if (ed->scrollable.control.window)
	{
		x += ed->scrollable.control.window->x;
		y += ed->scrollable.control.window->y + ed->scrollable.control.window->caption;
	}

	GLODWORD(MOUSEX) = x;
	GLODWORD(MOUSEY) = y;
}

/** Remove the current selection. It also creates a new selection graphic:
 *  use this function after the drawing map size changes.
 *
 *  @param ed			Pointer to the image editor object
 */

void gui_imageeditor_removeselection (IMAGEEDITOR * ed)
{
	if (ed->selection)
		bitmap_destroy (ed->selection);
	ed->selection = bitmap_new (0, ed->graph->width, ed->graph->height, 1);
	ed->selection_dirty = 0;
	ed->selection_empty = 1;
	ed->hants_count = 0;
	ed->vants_count = 0;
	gr_clear(ed->selection);
}

/** Invert the current selection 
 *
 *  @param ed			Pointer to the image editor object
 */

void gui_imageeditor_invertselection (IMAGEEDITOR * ed)
{
	int i;
	Uint8 * ptr;

	ptr = (Uint8 *)ed->selection->data;
	for (i = ed->selection->pitch * ed->selection->height ; i > 0 ; i--, ptr++)
		*ptr = ~*ptr;
	ed->selection_dirty = 1;
}

/** Select by color: mark all instances of the given color as selected
 *
 *	@param ed			Pointer to the image editor object
 *	@param color		Color to select
 */

void gui_imageeditor_selectcolor (IMAGEEDITOR * ed, int color)
{
	int x, y, mask;
	Uint8 * sel;

	if (ed->graph->depth == 8)
	{
		Uint8 * ptr;

		for (y = 0 ; y < ed->graph->height ; y++)
		{
			ptr = (Uint8 *)ed->graph->data + ed->graph->pitch * y;
			sel = (Uint8 *)ed->selection->data + ed->selection->pitch * y;
			mask = 0x80;

			for (x = 0 ; x < ed->graph->width ; x++, ptr++)
			{
				if (*ptr == color)
					*sel |= mask;
				mask >>= 1;
				if (!mask)
				{
					mask = 0x80;
					sel++;
				}
			}
		}
	}
	else if (ed->graph->depth == 16)
	{
		Uint16 * ptr;

		for (y = 0 ; y < ed->graph->height ; y++)
		{
			ptr = (Uint16 *)ed->graph->data + ed->graph->pitch * y / 2;
			sel = (Uint8 *)ed->selection->data + ed->selection->pitch * y;
			mask = 0x80;

			for (x = 0 ; x < ed->graph->width ; x++, ptr++)
			{
				if (*ptr == color)
					*sel |= mask;
				mask >>= 1;
				if (!mask)
				{
					mask = 0x80;
					sel++;
				}
			}
		}
	}

	ed->selection_dirty = 1;
}

/** Store the current graphic status at the current undo position
 *
 *  @param ed			Pointer to the image editor object
 *	@param region		Bounding box of the region to change or NULL for the full graphic
 */

void gui_imageeditor_saveundo (IMAGEEDITOR * ed, REGION * region)
{
	int i;

	/* Free some memory if needed */

	while (ed->undo_memory + ed->graph->height * ed->graph->pitch > UNDO_THRESHOLD)
	{
		if (ed->undo_pos > 0)
			ed->undo_pos--;
		ed->undo_used--;
		if (ed->undo[0].undo)
		{
			ed->undo_memory -= ed->undo[0].undo->height * ed->undo[0].undo->pitch;
			bitmap_destroy (ed->undo[0].undo);
		}
		if (ed->undo[0].redo)
		{
			ed->undo_memory -= ed->undo[0].redo->height * ed->undo[0].redo->pitch;
			bitmap_destroy (ed->undo[0].redo);
		}
		memmove (&ed->undo[0], &ed->undo[1], sizeof(UNDOSLOT) * (ed->undo_allocated - 1));
	}

	/* Delete forward redo operations, if any */

	if (ed->undo_pos < ed->undo_used)
	{
		for (i = ed->undo_pos ; i < ed->undo_used ; i++)
		{
			if (ed->undo[i].undo)
			{
				ed->undo_memory -= ed->undo[i].undo->pitch * ed->undo[i].undo->height;
				bitmap_destroy (ed->undo[i].undo);
			}
			if (ed->undo[i].redo)
			{
				ed->undo_memory -= ed->undo[i].redo->pitch * ed->undo[i].redo->height;
				bitmap_destroy (ed->undo[i].redo);
			}
		}
		ed->undo_used = ed->undo_pos;
	}

	/* Allocate more space for pointers, if needed */

	if (ed->undo_pos == ed->undo_allocated)
	{
		ed->undo_allocated += 16;
		ed->undo = (UNDOSLOT *) realloc (ed->undo, ed->undo_allocated * sizeof(UNDOSLOT));
	}

	/* Save the undo point */

	if (region)
	{
		ed->undo[ed->undo_pos].undo = bitmap_extract(ed->graph, region);
		ed->undo[ed->undo_pos].redo = NULL;
		ed->undo[ed->undo_pos].resized = 0;
		ed->undo[ed->undo_pos].bbox = *region;
		ed->undo_memory += ed->undo[ed->undo_pos].undo->pitch * 
			ed->undo[ed->undo_pos].undo->height;
	}
	else
	{
		ed->undo[ed->undo_pos].undo = bitmap_clone(ed->graph);
		ed->undo[ed->undo_pos].redo = NULL;
		ed->undo[ed->undo_pos].resized = 0;
		ed->undo[ed->undo_pos].bbox.x  = 
		ed->undo[ed->undo_pos].bbox.x2 = 
		ed->undo[ed->undo_pos].bbox.y  = 
		ed->undo[ed->undo_pos].bbox.y2 = -1;
		ed->undo_memory += ed->graph->pitch * ed->graph->height;
	}
	ed->undo_pos++;
	ed->undo_used = ed->undo_pos;
}

/** Change the current editing graphic for a new one, with changed size.
 *  This operation also stores an undo saving point.
 *
 *  @param ed			Pointer to the image editor object
 *	@param graph		New graphic to use
 */

void gui_imageeditor_setgraph (IMAGEEDITOR * ed, GRAPH * graph)
{
	ed->painting = 0;
	gui_imageeditor_destroytemporary(ed);

	gui_imageeditor_saveundo(ed, NULL);
	if (graph->width != ed->graph->width ||
		graph->height != ed->graph->height)
		ed->undo[ed->undo_pos-1].resized = 1;

	graph->code = ed->graph->code;
	grlib_add_map (ed->info->file, graph);
	ed->graph = graph;
	gui_imageeditor_removeselection(ed);
}

/** Execute an undo operation
 *
 *  @param ed			Pointer to the image editor object
 */

void gui_imageeditor_undo (IMAGEEDITOR * ed)
{
	REGION bbox;

	gui_imageeditor_destroytemporary (ed);

	if (ed->undo_pos > 0)
	{
		if (ed->undo_pos == ed->undo_allocated)
		{
			ed->undo_allocated += 16;
			ed->undo = (UNDOSLOT *) realloc (ed->undo,
				ed->undo_allocated * sizeof(UNDOSLOT));
		}

		bbox = ed->undo[--ed->undo_pos].bbox;

		/* Store previous state change */

		if (ed->undo[ed->undo_pos].redo == NULL)
		{
			if (bbox.x2 < 0)
			{
				ed->undo[ed->undo_pos].redo = bitmap_clone (ed->graph);
				ed->undo_memory += ed->graph->height * ed->graph->pitch;
			}
			else
			{
				ed->undo[ed->undo_pos].redo = bitmap_extract (ed->graph, &bbox);
				ed->undo_memory += ed->undo[ed->undo_pos].redo->height *
					ed->undo[ed->undo_pos].redo->pitch;
			}
		}

		/* Restore previous state */

		if (ed->undo[ed->undo_pos].resized)
		{
			gui_imageeditor_setgraph(ed, bitmap_clone(ed->undo[ed->undo_pos].undo));
		}
		else if (bbox.x2 < 0)
		{
			memcpy (ed->graph->data, ed->undo[ed->undo_pos].undo->data,
				ed->graph->height * ed->graph->pitch);
		}
		else
		{
			bitmap_subcopy (ed->graph, ed->undo[ed->undo_pos].undo, &bbox);
		}

		ed->graph->modified = 1;
	}
}

/** Execute a redo operation
 *
 *  @param ed			Pointer to the image editor object
 */

void gui_imageeditor_redo (IMAGEEDITOR * ed)
{
	gui_imageeditor_destroytemporary (ed);

	if (ed->undo_pos < ed->undo_used && ed->undo[ed->undo_pos].redo)
	{
		if (ed->undo[ed->undo_pos].resized)
			gui_imageeditor_setgraph(ed, bitmap_clone(ed->undo[ed->undo_pos].redo));
		else if (ed->undo[ed->undo_pos].bbox.x2 < 0)
			memcpy (ed->graph->data, ed->undo[ed->undo_pos].redo->data,
				ed->graph->height * ed->graph->pitch);
		else
			bitmap_subcopy (ed->graph, ed->undo[ed->undo_pos].redo,
				&ed->undo[ed->undo_pos].bbox);
		ed->graph->modified = 1;
		ed->undo_pos++;
	}
}

/** Destroy the temporary bitmap in a image editor
 *
 *  @param ed		Pointer to the image editor object
 */

void gui_imageeditor_destroytemporary (IMAGEEDITOR * ed)
{
	if (ed->temporary)
		bitmap_destroy(ed->temporary);
	ed->temporary = 0;
}

/** Create the temporary bitmap in a image editor
 *
 *  @param ed		Pointer to the image editor object
 *  @param bbox		Only interested in the given region, if != NULL
 */

void gui_imageeditor_createtemporary (IMAGEEDITOR * ed, REGION * bbox)
{
	if (ed->temporary)
	{
		if (ed->temporary->width != ed->graph->width || 
			ed->temporary->height != ed->graph->height)
			bitmap_destroy(ed->temporary);
		else 
		{
			if (bbox != NULL)
			{
				int y, offset, pitch, size;

				offset = bbox->x * ed->graph->depth / 8;
				size = (bbox->x2 - bbox->x + 1) * ed->graph->depth / 8;
				pitch = ed->temporary->pitch;

				for (y = bbox->y ; y <= bbox->y2 ; y++)
				{
					memcpy ((Uint8 *)ed->temporary->data + pitch*y + offset,
							(Uint8 *)ed->graph->data     + pitch*y + offset,
							size);
				}
			}
			else
			{
				memcpy (ed->temporary->data, ed->graph->data,
						ed->graph->height * ed->graph->pitch);
			}
			return;
		}
	}
	ed->temporary = bitmap_clone(ed->graph);
}

/** Apply the current selection (copy from temporary to destination, using the selection as mask)
 *
 *	@param ed			Pointer to the image editor object
 *	@param dest			Destination graphic (usually ed->graph, or another temporary)
 *  @param bbox		Only interested in the given region, if != NULL
 */

void gui_imageeditor_applyselection (IMAGEEDITOR * ed, GRAPH * dest, REGION * bbox)
{
	GRAPH * orig = ed->temporary;
	Uint8 * sel ;
	int x, y, mask = 0x80;
	int minx, maxx, miny, maxy;

	if (bbox == NULL)
	{
		minx = miny = 0;
		maxx = dest->width-1;
		maxy = dest->height-1;
	}
	else
	{
		minx = bbox->x;
		maxx = bbox->x2;
		miny = bbox->y;
		maxy = bbox->y2;
	}

	if (dest->depth == 16)
	{
		Uint16 * ptr, *src ;
		
		for (y = miny ; y <= maxy ; y++)
		{
			mask = (0x01 << (7-(minx & 7)));
			ptr  = (Uint16 *)dest->data + dest->pitch*y/2 + minx;
			src  = (Uint16 *)orig->data + orig->pitch*y/2 + minx;
			sel  = (Uint8  *)ed->selection->data + ed->selection->pitch*y + minx/8;

			for (x = minx ; x <= maxx ; x++, ptr++, src++)
			{
				if (*sel & mask)
					*ptr = *src;

				mask >>= 1;
				if (!mask) mask = 0x80, sel++;
			}
		}
	}
	else if (dest->depth == 8)
	{
		Uint8 * ptr, *src ;
		
		for (y = miny ; y <= maxy ; y++)
		{
			mask = (0x01 << (7-(minx & 7)));
			ptr  = (Uint8 *)dest->data + dest->pitch*y + minx;
			src  = (Uint8 *)orig->data + orig->pitch*y + minx;
			sel  = (Uint8 *)ed->selection->data + ed->selection->pitch*y + minx/8;

			for (x = minx ; x <= maxx ; x++, ptr++, src++)
			{
				if (*sel & mask)
					*ptr = *src;

				mask >>= 1;
				if (!mask) mask = 0x80, sel++;
			}
		}
	}

	dest->modified = 1;
}

/** Add an horizontal "walking ant" strip to the image editor
 *
 *	@param ed		Pointer to the editor object
 *	@param line		Line number, 0-based
 *	@param col		Column number, 0-based
 *	@param size		Width in pixels of the strip
 */

void gui_imageeditor_addhant (IMAGEEDITOR * ed, int col, int line, int size, int topleft)
{
	if (ed->hants_count == ed->hants_allocated)
	{
		ed->hants_allocated += 16;
		ed->hants = (ANT *)realloc(ed->hants, sizeof(ANT) * ed->hants_allocated);
	}
	ed->hants[ed->hants_count].line = line;
	ed->hants[ed->hants_count].col  = col;
	ed->hants[ed->hants_count].size = size;
	ed->hants[ed->hants_count].topleft = topleft;
	ed->hants_count++;
}

/** Add a vertical "walking ant" strip to the image editor
 *
 *	@param ed		Pointer to the editor object
 *	@param line		Line number, 0-based
 *	@param col		Column number, 0-based
 *	@param size		Height in pixels of the strip
 */

void gui_imageeditor_addvant (IMAGEEDITOR * ed, int line, int col, int size, int topleft)
{
	if (ed->vants_count == ed->vants_allocated)
	{
		ed->vants_allocated += 16;
		ed->vants = (ANT *)realloc(ed->vants, sizeof(ANT) * ed->vants_allocated);
	}
	ed->vants[ed->vants_count].line = line;
	ed->vants[ed->vants_count].col  = col;
	ed->vants[ed->vants_count].size = size;
	ed->vants[ed->vants_count].topleft = topleft;
	ed->vants_count++;
}

/** Create or update the "walking ants" arrays
 *
 *	@param ed		Pointer to the image editor object
 */

static void gui_imageeditor_updateselection (IMAGEEDITOR * ed)
{
	int x, y, mask;
	Uint8 * ptr;
	GRAPH * sel = ed->selection;
	Uint32 * top, * bottom, * left, * right;

	/* Initialization */
	top    = (Uint32 *)malloc(4*sel->height);
	bottom = (Uint32 *)malloc(4*sel->height);
	left   = (Uint32 *)malloc(4*sel->width);
	right  = (Uint32 *)malloc(4*sel->width);
	memset (top, 0, 4*sel->height);
	memset (bottom, 0, 4*sel->height);
	memset (left, 0, 4*sel->width);
	memset (right, 0, 4*sel->width);
	ed->hants_count = 0;
	ed->vants_count = 0;

	/* Find vertical strips, and keep count of the horizontal borders,
	   creating horizontal strips as necessary */

	for (x = 0 ; x < sel->width ; x++)
	{
		ptr = (Uint8 *)sel->data + x/8;
		mask = (1 << (7-(x & 7)));
		
		for (y = 0 ; y < sel->height ; )
		{
			/* Find the next vertical top (and seal any opened border) */
			while (!(*ptr & mask) && y < sel->height) 
			{
				if (top[y])
				{
					gui_imageeditor_addhant (ed, x - top[y], y, top[y], 1);
					top[y] = 0;
				}
				if (bottom[y])
				{
					gui_imageeditor_addhant (ed, x - bottom[y], y, bottom[y], 0);
					bottom[y] = 0;
				}
				ptr += sel->pitch, y++;
			}
			if (y == sel->height) break;

			/* Top border found (seal bottom if any) */
			top[y]++;
			if (bottom[y])
			{
				gui_imageeditor_addhant (ed, x - bottom[y], y, bottom[y], 0);
				bottom[y] = 0;
			}

			/* Find bottom border, seal any */
			while ( (*ptr & mask) ) 
			{
				ptr += sel->pitch, y++;
				if (y == sel->height)
					break;
				if (top[y])
				{
					gui_imageeditor_addhant (ed, x - top[y], y, top[y], 1);
					top[y] = 0;
				}
				if (!(*ptr & mask))
					bottom[y-1]++;
				else if (bottom[y-1])
				{
					gui_imageeditor_addhant (ed, x - bottom[y-1], y-1, bottom[y-1], 0);
					bottom[y-1] = 0;
				}
			}
		}
	}

	/* Add remaining (counted) borders as strips */

	for (y = 0 ; y < sel->height ; y++)
	{
		if (top[y])
			gui_imageeditor_addhant (ed, sel->width-top[y], y, top[y], 1);
		if (bottom[y])
			gui_imageeditor_addhant (ed, sel->width-bottom[y], y, bottom[y], 1);
	}

	/* Find vertical strips, and keep count of the horizontal borders,
	   creating horizontal strips as necessary */

	for (y = 0 ; y < sel->height ; y++)
	{
		ptr = (Uint8 *)sel->data + sel->pitch * y;
		mask = 0x80;
		
		for (x = 0 ; x < sel->width ; )
		{
			/* Find the next horizontal left border (and seal any opened border) */
			while (!(*ptr & mask) && x < sel->width) 
			{
				if (left[x])
				{
					gui_imageeditor_addvant (ed, y - left[x], x, left[x], 1);
					left[x] = 0;
				}
				if (right[x])
				{
					gui_imageeditor_addvant (ed, y - right[x], x, right[x], 0);
					right[x] = 0;
				}
				x++; mask >>= 1; 
				if (!mask) mask = 0x80, ptr++;
			}
			if (x == sel->width) break;

			/* Left border found (seal right if any) */
			left[x]++;
			if (right[x])
			{
				gui_imageeditor_addvant (ed, y - right[x], x, right[x], 0);
				right[x] = 0;
			}

			/* Find bottom border, seal any */
			while ( (*ptr & mask) ) 
			{
				x++; mask >>= 1; 
				if (x == sel->width)
					break;
				if (left[x])
				{
					gui_imageeditor_addvant (ed, y - left[x], x, left[x], 1);
					left[x] = 0;
				}
				if (!mask) mask = 0x80, ptr++;
				if (!(*ptr & mask))
					right[x-1]++;
				else if (right[x-1])
				{
					gui_imageeditor_addvant (ed, y - right[x-1], x-1, right[x-1], 0);
					right[x-1] = 0;
				}
			}
		}
	}

	/* Add remaining (counted) borders as strips */

	for (x = 0 ; x < sel->width ; x++)
	{
		if (left[x])
			gui_imageeditor_addvant (ed, sel->height-left[x], x, left[x], 1);
		if (right[x])
			gui_imageeditor_addvant (ed, sel->height-right[x], x, right[x], 1);
	}

	free (top);
	free (bottom);
	free (left);
	free (right);

	ed->selection_empty = !(ed->vants_count || ed->hants_count);
}

/** Create a new graphic with the current selection
 *
 *  @param ed		Pointer to the editor
 *  @returns		New graphic created with bitmap_new or bitmap_clone
 */

GRAPH * gui_imageeditor_getselection (IMAGEEDITOR * ed)
{
	GRAPH * graph;
	GRAPH * sel = ed->selection;
	Uint8 * ptr;
	int x, y, x0, y0, mask;
	int width, height;

	if (ed->selection_empty)
		return bitmap_clone (ed->graph);

	x0 = 0;
	y0 = 0;
	width = sel->width;
	height = sel->height;

	/* Cut top lines */

	for (y = 0 ; y < sel->height ; y++)
	{
		ptr = (Uint8*)sel->data + sel->pitch*y;
		for (x = 0, mask = 0x80 ; x < sel->width ; x++, mask >>= 1)
		{
			if (!mask) mask = 0x80, ptr++;
			if (*ptr & mask) break;
		}
		if (x != sel->width)
			break;
		y0++, height--;
	}

	/* Cut bottom lines */

	for (y = sel->height-1 ; y >= 0 ; y--)
	{
		ptr = (Uint8*)sel->data + sel->pitch*y;
		for (x = 0, mask = 0x80 ; x < sel->width ; x++, mask >>= 1)
		{
			if (!mask) mask = 0x80, ptr++;
			if (*ptr & mask) break;
		}
		if (x != sel->width)
			break;
		height--;
	}

	/* Cut left columns */

	for (ptr = sel->data, x = 0, mask = 0x80 ; x < sel->width ; x++, mask >>= 1)
	{
		if (!mask) ptr++, mask = 0x80;
		for (y = 0 ; y < sel->height ; y++)
		{
			if (ptr[sel->pitch*y] & mask) break;
		}
		if (y != sel->height)
			break;
		x0++, width--;
	}

	/* Cut right columns */

	ptr = (Uint8 *)sel->data + (sel->width-1)/8;
	mask = (0x01 << (7 - ((sel->width-1) & 7)));
	for (x = sel->width-1 ; x >= 0 ; x--, mask <<= 1)
	{
		if (mask == 0x100) mask = 0x01, ptr--;
		for (y = 0 ; y < sel->height ; y++)
		{
			if (ptr[sel->pitch*y] & mask) break;
		}
		if (y != sel->height)
			break;
		width--;
	}

	/* Create the new graphic */

	graph = bitmap_new (ed->graph->code, width, height, ed->graph->depth);
	gr_clear(graph);

	for (x = 0 ; x < width ; x++)
	{
		ptr = (Uint8*)sel->data + (x0+x)/8 + y0*sel->pitch;
		mask = (0x01 << (7 - ((x0+x) & 7)));

		for (y = 0 ; y < height ; y++, ptr += sel->pitch)
		{
			if (*ptr & mask)
				gr_put_pixel (graph, x, y, gr_get_pixel(ed->graph, x+x0, y+y0));
		}
	}

	return graph;
}

/** Draw an image editor object
 *
 *  @param c		Pointer to the control member (at offset 0 of the class)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param x0		Internal left coordinate
 *	@param y0		Internal top coordinate
 *	@param x1		Internal right coordinate
 *	@param y1		Internal bottom coordinate
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_imageeditor_draw (CONTROL * c, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	IMAGEEDITOR * ed = (IMAGEEDITOR *)c;
	IMAGEEDITORINFO * info = ed->info;
	int px, py, i, j, s, w, n;
	int cx, cy, bx, by, c1, c2;
	GRAPH * graph = ed->graph;
	REGION region, region2;

	c->allkeys = ed->active;

	/* Detect changes in the graphic we're editing */

	graph = bitmap_get (ed->info->file, ed->info->graph);
	if (graph != ed->graph)
	{
		gui_imageeditor_destroytemporary (ed);
		ed->mpoints_count = 0;
		ed->painting = 0;
		ed->graph = graph;
	}
	if (!graph) return;

	/* Detect a tool change */

	if (ed->paintingtool != ed->info->tool)
	{
		gui_imageeditor_destroytemporary(ed);
		ed->painting = 0;
		ed->mpoints_count = 0;
	}
	ed->paintingtool = ed->info->tool;

	/* Update if necessary a timer-based tool */

	if (ed->paintingtool > 0)
	{
		IMAGEEDITORTOOL * tool = imageeditor_tools[ed->paintingtool];
		if (tool->type & TOOLTYPE_TIMER)
		{
			while (ed->painting && ed->timer < gr_timer() - 25)
			{
				ed->timer += 25;
				gui_imageeditor_mousemove (c, ed->clickx, ed->clicky, 1);
			}
		}
	}

	/* Detect changes in foreground/background colors and
	   update the current variable (a change at the 16 bit color
	   forces a change at the 8 bit one, and viceversa) */

	if (ed->info->fg8 != ed->currentfg8)
	{
		ed->currentfg8 = ed->info->fg8;
		if (ed->info->fg8 <= 0)
			ed->info->fg = ed->currentfg = ed->info->fg8;
		else
			ed->info->fg = ed->currentfg = colorequiv[ed->info->fg8 & 0xFF];
	}
	if (ed->info->bg8 != ed->currentbg8)
	{
		ed->currentbg8 = ed->info->bg8;
		if (ed->info->bg8 <= 0)
			ed->info->bg = ed->currentbg = ed->info->bg8;
		else
			ed->info->bg = ed->currentbg = colorequiv[ed->info->bg8 & 0xFF];
	}
	if (ed->info->fg != ed->currentfg)
	{
		int r, g, b;
		gr_get_rgb (ed->info->fg, &r, &g, &b);
		if (ed->info->fg <= 0)
		{
			ed->info->fg8 = ed->currentfg8 = ed->info->fg;
			ed->currentfg = ed->info->fg;
		}
		else
		{
			ed->info->fg8 = ed->currentfg8 = gr_find_nearest_color(r, g, b);
			if (ed->graph->depth == 8)
				ed->info->fg = ed->currentfg = colorequiv[ed->info->fg8];
		}
	}
	if (ed->info->bg != ed->currentbg)
	{
		if (ed->info->bg <= 0)
		{
			ed->info->bg8 = ed->currentbg8 = ed->info->bg;
			ed->currentbg = ed->info->bg;
		}
		else
		{
			int r, g, b;
			gr_get_rgb (ed->info->bg, &r, &g, &b);
			ed->info->bg8 = ed->currentbg8 = gr_find_nearest_color(r, g, b);
			if (ed->graph->depth == 8)
				ed->info->bg = ed->currentbg = colorequiv[ed->info->bg8];
		}
	}

	/* Current tool is not a multiline tool, but we have an ongoing
	   multiline selection - abort it */

	if (ed->mpoints_count > 0 && 
		(ed->info->tool < 0 || ed->info->tool >= TOOL_COUNT ||
		 (imageeditor_tools[ed->info->tool]->type & TOOLTYPE_MASK) != TOOLTYPE_MULTILINE))
	{
		ed->mpoints_count = 0;
		ed->painting = 0;
		gui_imageeditor_destroytemporary (ed);
	}

	if (ed->temporary)
		graph = ed->temporary;

	gui_scrollable_resizearea (c, graph->width * info->zoom / 100, graph->height * info->zoom / 100);

	/* Draw the background */

	switch (ed->info->paper_mode)
	{
		case 0:
			c1 = gr_rgb ( 80,  80,  80);
			c2 = gr_rgb (128, 128, 128);
			region.x = x-x0;
			region.y = y-y0;
			region.x2 = region.x + ed->graph->width * ed->info->zoom / 100;
			region.y2 = region.y + ed->graph->height * ed->info->zoom / 100;
			region2 = region;
			region_union (&region2, clip);

			for (i = 0 ; i < region.x2 - region.x ; i += 16)
			{
				if (i+region.x < region2.x)
					i += (region2.x - region.x - i - 16)/16*16;
				if (i+region.x > region2.x2)
					break;
				for (j = 0 ; j < region.y2 - region.y ; j += 16)
				{
					if (j+region.y < region2.y)
						j += (region2.y - region.y - j - 16)/16*16;
					if (j+region.y > region2.y2)
						break;
					gr_setcolor (((i+j)&16) ? c1 : c2);
					gr_box (dest, &region2, region.x+i, region.y+j, 16, 16);
				}
			}
			break;

		case 1:
			gr_setcolor(ed->info->paper_color);
			gr_box (dest, clip, x - x0, y - y0, ed->graph->width * info->zoom / 100, ed->graph->height * info->zoom / 100);
			break;
	}

	/* Draw the image */

	if (ed->graph->ncpoints > 0 && ed->graph->cpoints[0].x != CPOINT_UNDEFINED)
	{
		cx = ed->graph->cpoints[0].x;
		cy = ed->graph->cpoints[0].y;
	}
	else
	{
		cx = ed->graph->width/2;
		cy = ed->graph->height/2;
	}

	bx = x-x0+cx*info->zoom/100;
	by = y-y0+cy*info->zoom/100;
		
	if (info->zoom == 100)
		gr_blit (dest, clip, bx, by, 0, graph);
	else
		gr_rotated_blit (dest, clip, bx, by, 0, 0, info->zoom, info->zoom, graph);

	/* Draw the grid */

	if ((ed->info->grid_visible & 0x01) && ed->info->zoom >= 800)
	{
		gr_setcolor (ed->info->pixelgrid_color);
		for (i = 0 ; i < graph->width ; i++)
			gr_vline (dest, clip, x - x0 + i * ed->info->zoom / 100, y - y0, graph->height * info->zoom / 100);
		for (i = 0 ; i < graph->height ; i++)
			gr_hline (dest, clip, x - x0, y - y0 + i * ed->info->zoom / 100, graph->width * info->zoom / 100);
	}

	if (ed->info->grid_visible & 0x02)
	{
		gr_setcolor (ed->info->grid_color);
		for (i = 0 ; i < graph->width ; i += ed->info->grid_step)
			gr_vline (dest, clip, x - x0 + i * info->zoom / 100, y - y0, graph->height * info->zoom / 100);
		for (i = 0 ; i < graph->height ; i += ed->info->grid_step)
			gr_hline (dest, clip, x - x0, y - y0 + i * info->zoom / 100, graph->width * info->zoom / 100);
	}

	/* Draw the walking ants */
	
	if (ed->selection_dirty)
	{
		ed->selection_dirty = 0;
		gui_imageeditor_updateselection(ed);
	}
	s = drawing_stipple;
	w = gr_rgb(255,255,255);
	drawing_stipple = 0x33333333 << ((gr_timer() / 100) % 4) ;
	drawing_stipple |= ((drawing_stipple & 0xF0) >> 4);
	for (i = 0 ; i < ed->hants_count ; i++)
	{
		px = x - x0 + (info->zoom * ed->hants[i].col  / 100);
		py = y - y0 + (info->zoom * ed->hants[i].line / 100);
		if (!ed->hants[i].topleft)
			py += (info->zoom/100);
		else
			py --;
		gr_setcolor(w);
		n = drawing_stipple;
		gr_hline (dest, clip, px, py, info->zoom * ed->hants[i].size / 100);
		drawing_stipple = ~n;
		gr_setcolor(0);
		gr_hline (dest, clip, px, py, info->zoom * ed->hants[i].size / 100);
		drawing_stipple = ~drawing_stipple;
	}
	for (i = 0 ; i < ed->vants_count ; i++)
	{
		px = x - x0 + (info->zoom * ed->vants[i].col  / 100);
		py = y - y0 + (info->zoom * ed->vants[i].line / 100);
		if (!ed->vants[i].topleft)
			px += (info->zoom/100);
		else
			px --;
		gr_setcolor(w);
		n = drawing_stipple;
		gr_vline (dest, clip, px, py, info->zoom * ed->vants[i].size / 100);
		drawing_stipple = ~n;
		gr_setcolor(0);
		gr_vline (dest, clip, px, py, info->zoom * ed->vants[i].size / 100);
		drawing_stipple = ~drawing_stipple;
	}
	drawing_stipple = s;

	/* Tool drawing */

	if (ed->info->tool >= 0 && ed->info->tool < TOOL_COUNT)
		if (imageeditor_tools[ed->info->tool]->draw)
			(*imageeditor_tools[ed->info->tool]->draw)(ed, dest, x, y, x0, y0, x1, y1, clip);
}

/** Handle mouse click events. It is also responsible of setting the draggable attribute.
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 *	@param x			Local X coordinate of the mouse (or MB_EMUL to use user defined ones instead)
 *	@param y			Local Y coordinate of the mouse (idem)
 *	@param b			Index of the button
 *	@param pressed		0 = Released; 1 = Pressed; 2+ = Double click
 *	@return				1 if the event is processed, 0 if it is ignored
 * */

int gui_imageeditor_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	IMAGEEDITOR * ed = (IMAGEEDITOR *)c;
	IMAGEEDITORINFO * info = ed->info;
	REGION bbox;
	int ox, oy;

	c->redraw = 1;

	if (c->draggable == 2 && !ed->active)
		c->draggable = 1;

	/* Release the button to stop space dragging */

	if (ed->space_drag && !pressed && b == 0)
	{
		ed->space_drag = 0;
		return 1;
	}
	
	/* Use MB_EMUL to use the last keyboard navigation coordinates */

	if (x == MB_EMUL)
		x = ed->info->x * info->zoom / 100;
	if (y == MB_EMUL)
		y = ed->info->y * info->zoom / 100;

	/* Detect a tool change */

	if (ed->paintingtool != ed->info->tool)
	{
		gui_window_releasemouse (c->window, c);
		gui_imageeditor_destroytemporary(ed);
		ed->painting = 0;
	}
	ed->paintingtool = ed->info->tool;

	ox = x, oy = y;

	if (info->zoom < 0) info->zoom = 1;
	x = x * 100 / info->zoom;
	y = y * 100 / info->zoom;

	if (gr_key(KEY_SPACE) && b == 0)
	{
		/* Press SPACE to drag around */
		ox -= ed->scrollable.hpos.pos;
		oy -= ed->scrollable.vpos.pos;
		ed->clickx = ox;
		ed->clicky = oy;
		ed->dragx = ed->scrollable.hpos.pos;
		ed->dragy = ed->scrollable.vpos.pos;
		ed->space_drag = 1;
		return 1;
	}
	else if (gr_key(KEY_LALT) && (b == 1 || b == 0))
	{
		/* Use ALT and LMB or RMB to pick colors */
		int c = gr_get_pixel (ed->graph, x, y);

		/* Don't allow editing if we're not active */
		if (!ed->active)
			return 0;

		if (ed->graph->depth == 8)
		{
			if (b == 0 && pressed)
				ed->info->fg8 = c;
			else if (b == 1 && pressed == 1)
				ed->info->bg8 = c;
			else if (b == 1 && pressed == 2)
				ed->info->bg8 = -1;
		}
		else if (ed->graph->depth == 16)
		{
			if (b == 0 && pressed)
				ed->info->fg = c;
			else if (b == 1 && pressed == 1)
				ed->info->bg = c;
			else if (b == 1 && pressed == 2)
				ed->info->bg = -1;
		}
		return 1;
	}
	else if (info->tool >= 0 && info->tool < TOOL_COUNT)
	{
		IMAGEEDITORTOOL * tool = imageeditor_tools[info->tool];

		/* Don't allow editing if we're not active */
		if (!ed->active)
			return 0;

		if (tool->type & TOOLTYPE_TIMER)
		{
			ed->timer = gr_timer();
			ed->clickx = ox;
			ed->clicky = oy;
		}

		switch (tool->type & TOOLTYPE_MASK)
		{
			/* A fill type tool is activated on mouse press */

			case TOOLTYPE_FILL:
				ed->lastpx = INT_MIN;
				ed->lastpy = INT_MIN;
				if (pressed)
				{
					if (tool->click)
					{
						(*tool->click)(ed, x, y, (1 << b), 1);
						if (!(tool->type & TOOLTYPE_HARMLESS))
						{
							c->actionflags |= ACTION_CHANGE;
							c->redraw = 1;
							if (tool->bbox) (*tool->bbox)(ed, &bbox);
							gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
							if (ed->selection_empty)
								(*tool->render) (ed, ed->graph);
							else
							{
								gui_imageeditor_createtemporary(ed, NULL);
								(*tool->render) (ed, ed->temporary);
								gui_imageeditor_applyselection(ed, ed->graph, tool->bbox ? &bbox : NULL);
								gui_imageeditor_destroytemporary(ed);
							}
						}
					}
				}
				break;

			/* A pencil type tool is activated on mouse press and works at temporary surface only,
			   and is finally rendered to the image on mouse release only */

			case TOOLTYPE_PENCIL:
				ed->lastpx = INT_MIN;
				ed->lastpy = INT_MIN;
				if (pressed)
				{
					if (tool->click)
					{
						ed->painting = 1;

						(*tool->click)(ed, x, y, (1 << b), 1);
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						if (!(tool->type & TOOLTYPE_HARMLESS) && tool->type == TOOLTYPE_FILL)
						{
							c->actionflags |= ACTION_CHANGE;
							c->redraw = 1;
							gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
						}
						if (ed->selection_empty)
						{
							gui_imageeditor_createtemporary(ed, NULL);
							(*tool->render) (ed, ed->temporary);
						}
						else
						{
							GRAPH * new_temporary = bitmap_clone(ed->graph);
							gui_imageeditor_createtemporary(ed, NULL);
							(*tool->render) (ed, ed->temporary);
							gui_imageeditor_applyselection(ed, new_temporary, tool->bbox ? &bbox : NULL);
							gui_imageeditor_destroytemporary(ed);
							ed->temporary = new_temporary;
						}
					}
				}
				if (!pressed)
				{
					ed->painting = 0;

					if (ed->temporary)
					{
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
						if (ed->selection_empty)
							(*tool->render) (ed, ed->graph);
						else
							gui_imageeditor_applyselection(ed, ed->graph, tool->bbox ? &bbox : NULL);
						gui_imageeditor_destroytemporary(ed);
					}
				}
				break;

			/* A multiline tool allows the user to click a series of pixels
			   and stores them at the ed->mpoints array */

			case TOOLTYPE_MULTILINE:
				if (pressed)
				{
					if (b == 0)
					{
						if (ed->mpoints_allocated == ed->mpoints_count)
						{
							ed->mpoints_allocated += 8;
							ed->mpoints = (MULTILINEPOINT *)realloc(ed->mpoints,
								sizeof(MULTILINEPOINT) * ed->mpoints_allocated);
						}
						gui_imageeditor_createtemporary(ed, NULL);
						(*tool->drag)(ed, 0, 0, x, y, 1 << b, 0);
						(*tool->render)(ed, ed->temporary);
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						if (!ed->selection_empty)
						{
							GRAPH * new_temporary = bitmap_clone (ed->graph);
							gui_imageeditor_applyselection (ed, new_temporary, tool->bbox ? &bbox : NULL);
							gui_imageeditor_destroytemporary(ed);
							ed->temporary = new_temporary;
						}
						ed->mpoints[ed->mpoints_count].x = x;
						ed->mpoints[ed->mpoints_count].y = y;
						ed->mpoints_count++;
					}
					else if (b == 1 && ed->mpoints_count > 0)
					{
						if (!(tool->type & TOOLTYPE_DBLCLKADDS))
							ed->mpoints_count--;
						else
						{
							if (ed->mpoints_allocated == ed->mpoints_count)
							{
								ed->mpoints_allocated += 8;
								ed->mpoints = (MULTILINEPOINT *)realloc(ed->mpoints,
									sizeof(MULTILINEPOINT) * ed->mpoints_allocated);
							}
							ed->mpoints[ed->mpoints_count].x = x;
							ed->mpoints[ed->mpoints_count].y = y;
						}
						if (ed->selection_empty)
						{
							(*tool->drag)(ed, 0, 0, 
								ed->mpoints[ed->mpoints_count].x,
								ed->mpoints[ed->mpoints_count].y, (1 << b), 1);
							if (tool->bbox) (*tool->bbox)(ed, &bbox);
							gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
							(*tool->render)(ed, ed->graph);
						}
						else 
						{
							gui_imageeditor_createtemporary(ed, NULL);

							(*tool->drag)(ed, 0, 0, 
								ed->mpoints[ed->mpoints_count].x,
								ed->mpoints[ed->mpoints_count].y, (1 << b), 1);
							if (tool->bbox) (*tool->bbox)(ed, &bbox);
							gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
							(*tool->render)(ed, ed->temporary);
							gui_imageeditor_applyselection (ed, ed->graph, tool->bbox ? &bbox : NULL);
						}
						gui_imageeditor_destroytemporary(ed);
						c->actionflags |= ACTION_CHANGE;
						c->redraw = 1;
						ed->mpoints_count = 0;
					}
				}
				break;

			/* A line tool allows the user to drag between two points,
			   or click in a point and another thereafter */

			case TOOLTYPE_LINE:
				if (ed->painting && (pressed || ed->clickx != x || ed->clicky != y))
				{
					if (!(tool->type & TOOLTYPE_HARMLESS))
					{
						if (tool->drag)
						{
							(*tool->drag)(ed, ed->clickx, ed->clicky, x, y, (1 << b), 1);
							if (tool->bbox) (*tool->bbox)(ed, &bbox);
							gui_imageeditor_saveundo (ed, tool->bbox ? &bbox : NULL);
							if (ed->selection_empty)
								(*tool->render)(ed, ed->graph);
							else if (ed->temporary)
								gui_imageeditor_applyselection(ed, ed->graph, tool->bbox ? &bbox : NULL);
							c->actionflags |= ACTION_CHANGE;
						}
						gui_imageeditor_destroytemporary(ed);
						ed->painting = 0;
					}
					else 
					{
						(*tool->drag)(ed, ed->clickx, ed->clicky, x, y, (1 << b), 1);
						(*tool->render)(ed, ed->graph);
						gui_imageeditor_destroytemporary(ed);
						ed->painting = 0;
					}
					gui_window_releasemouse (c->window, c);
				}
				else if (pressed)
				{
					ed->clickx = x, ed->clicky = y;
					ed->clicktox = x, ed->clicktoy = y;
					ed->painting = 1;
					gui_imageeditor_createtemporary(ed, NULL);
					(*tool->drag)(ed, ed->clickx, ed->clicky, x, y, (1 << b), 0);
					(*tool->render)(ed, ed->temporary);
					gui_window_capturemouse (c->window, c);
				}
				break;
		}

		return 1;
	}

	ed->painting = 0;
	return 0;
}

/** Response to a mouse move event
 *
 *	@param c			Pointer to the control
 *	@param x			Local X coordinate of the mouse
 *	@param y			Local Y coordinate of the mouse
 *	@param buttons		Bit mask with the state of each mouse button
 *	@return				1 if the event is processed, 0 if it is ignored
 **/

int gui_imageeditor_mousemove (CONTROL * c, int x, int y, int buttons)
{
	IMAGEEDITOR * ed = (IMAGEEDITOR *)c;
	IMAGEEDITORINFO * info = ed->info;
	REGION bbox;
	int ox = x, oy = y;

	if (ed->space_drag)
	{
		int incx, incy;
		x -= ed->scrollable.hpos.pos;
		y -= ed->scrollable.vpos.pos;
		incx = x - ed->clickx;
		incy = y - ed->clicky;
		gui_scrollable_scrollto (c, ed->dragx - incx, ed->dragy - incy);
		ed->dragx = ed->scrollable.scrollx;
		ed->dragy = ed->scrollable.scrolly;
		ed->clickx = x;
		ed->clicky = y;
		c->redraw = 1;
		return 1;
	}

	if (buttons || ed->painting)
		c->redraw = 1;

	/* Don't allow anything other than dragging around if we're not active */
	if (!ed->active)
		return 0;

	if (info->zoom < 0) info->zoom = 1;
	x = x * 100 / info->zoom;
	y = y * 100 / info->zoom;

	ed->info->x = x;
	ed->info->y = y;

	/* Use ALT and LMB/RMB to pick colors */
	if (gr_key(KEY_LALT))
	{
		int c = gr_get_pixel (ed->graph, x, y);
		if (ed->graph->depth == 8)
		{
			if (buttons & 1)
				ed->info->fg8 = c;
			if (buttons & 2)
				ed->info->bg8 = c;
		}
		else if (ed->graph->depth == 16)
		{
			if (buttons & 1)
				ed->info->fg = c;
			if (buttons & 2)
				ed->info->bg = c;
		}
		return 1;
	}

	if (info->tool >= 0 && info->tool < TOOL_COUNT)
	{
		IMAGEEDITORTOOL * tool = imageeditor_tools[info->tool];

		if (!(tool->type & TOOLTYPE_HARMLESS) && buttons)
			c->actionflags |= ACTION_CHANGE;

		if (tool->type & TOOLTYPE_TIMER)
		{
			ed->timer = gr_timer();
			ed->clickx = ox;
			ed->clicky = oy;
		}

		switch (tool->type & TOOLTYPE_MASK)
		{
			case TOOLTYPE_PENCIL:
				if (tool->click && buttons)
				{
					(*tool->click)(ed, x, y, buttons, 0);
					if (ed->selection_empty || (tool->type & TOOLTYPE_HARMLESS))
					{
						gui_imageeditor_createtemporary(ed, NULL);
						(*tool->render)(ed, ed->temporary);
					}
					else
					{
						GRAPH * new_temporary = bitmap_clone(ed->graph);
						gui_imageeditor_createtemporary(ed, NULL);
						(*tool->render)(ed, ed->temporary);
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						gui_imageeditor_applyselection(ed, new_temporary, tool->bbox ? &bbox : NULL);
						gui_imageeditor_destroytemporary(ed);
						ed->temporary = new_temporary;
					}
				}
				break;

			case TOOLTYPE_LINE:
				if (tool->drag && (ed->painting || buttons))
				{
					ed->painting = 1;
					gui_imageeditor_createtemporary(ed, NULL);
					(*tool->drag)(ed, ed->clickx, ed->clicky, x, y, buttons, 0);
					(*tool->render)(ed, ed->temporary);
					if (!ed->selection_empty)
					{
						GRAPH * new_temporary = bitmap_clone (ed->graph);
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						gui_imageeditor_applyselection (ed, new_temporary, tool->bbox ? &bbox : NULL);
						gui_imageeditor_destroytemporary(ed);
						ed->temporary = new_temporary;
					}
				}
				if (buttons || ed->painting)
				{
					ed->painting = 1;
					ed->clicktox = x;
					ed->clicktoy = y;
				}
				return 1;

			case TOOLTYPE_MULTILINE:
				if (ed->mpoints_count > 0)
				{
					gui_imageeditor_createtemporary(ed, NULL);
					(*tool->drag)(ed, 0, 0, x, y, buttons, 0);
					(*tool->render)(ed, ed->temporary);
					if (!ed->selection_empty)
					{
						GRAPH * new_temporary = bitmap_clone (ed->graph);
						if (tool->bbox) (*tool->bbox)(ed, &bbox);
						gui_imageeditor_applyselection (ed, new_temporary, tool->bbox ? &bbox : NULL);
						gui_imageeditor_destroytemporary(ed);
						ed->temporary = new_temporary;
					}
				}
				return 1;
		}
	}
	return 0;
}

/** Response to a keyboard press event 
 */

int gui_imageeditor_key (CONTROL * c, int scancode, int key)
{
	IMAGEEDITOR * ed = (IMAGEEDITOR *)c;
	GRAPH * graph;
	REGION  bbox;
	int navinc = 1;

	if (gr_key(KEY_LSHIFT))
		navinc *= 8;
	if (gr_key(KEY_RSHIFT))
		navinc *= 8;

	switch (scancode)
	{
		case KEY_G:
			if (ed->info->zoom >= 800)
				ed->info->grid_visible ^= 0x01;
			else
				ed->info->grid_visible ^= 0x02;
			return 1;

		case KEY_B:
			ed->info->paper_mode ^= 1;
			return 1;
	}

	/* Don't allow editing if we're not active */
	if (!ed->active)
		return 0;

	switch (scancode)
	{
		/* Clipboard */

		case KEY_C | KEYMOD_CONTROL: /* Copy */
			graph = gui_imageeditor_getselection(ed);
			bitmap_getoptimalbbox (graph, &bbox);
			bitmap_slice (graph, bbox.x, bbox.y, bbox.x2-bbox.x+1, bbox.y2-bbox.y+1);
			gui_copy_graph (graph);
			bitmap_destroy(graph);
			if (ed->info->tool != TOOL_PASTE)
				return 1;

		case KEY_V | KEYMOD_CONTROL: /* Paste */
			if (paste_graph)
				bitmap_destroy(paste_graph);
			paste_graph = gui_paste_graph();
			ed->info->tool = TOOL_PASTE;
			break;

		/* Utility keys */

		case KEY_0:
			if (ed->info->fg)
				ed->lastfg = ed->info->fg;
			ed->info->fg = (ed->info->fg ? 0 : ed->lastfg);
			return 1;

		case KEY_0 | KEYMOD_ALT:
			if (ed->info->bg != -1)
				ed->lastbg = ed->info->bg;
			ed->info->bg = (ed->info->bg == -1 ? ed->lastbg : -1);
			return 1;

		/* Color-0 selection */

		case KEY_N:						/* Select all color 0 pixels only */
			gui_imageeditor_removeselection (ed);
			gui_imageeditor_selectcolor (ed, 0);
			gui_imageeditor_invertselection (ed);
			return 1;
		case KEY_N | KEYMOD_CONTROL:	/* Add all color 0 pixels to the selection */
			gui_imageeditor_selectcolor (ed, 0);
			return 1;
		case KEY_N | KEYMOD_SHIFT:		/* Remove any color 0 from selection */
			gui_imageeditor_invertselection (ed);
			gui_imageeditor_selectcolor (ed, 0);
			gui_imageeditor_invertselection (ed);
			return 1;
		case KEY_N | KEYMOD_ALT:		/* Remove selection */
			gui_imageeditor_removeselection (ed);
			return 1;

		/* Keyboard navigation */

		case KEY_O:
		case KEY_O | KEYMOD_SHIFT:
		case KEY_LEFT:
		case KEY_LEFT | KEYMOD_SHIFT:
		case KEY_LEFT | KEYMOD_SHIFT | KEYMOD_ALT:
			gui_imageeditor_movecursor (ed, ed->info->x-navinc, ed->info->y);
			return 1;

		case KEY_P:
		case KEY_P | KEYMOD_SHIFT:
		case KEY_RIGHT:
		case KEY_RIGHT | KEYMOD_SHIFT:
		case KEY_RIGHT | KEYMOD_SHIFT | KEYMOD_ALT:
			gui_imageeditor_movecursor (ed, ed->info->x+navinc, ed->info->y);
			return 1;

		case KEY_Q:
		case KEY_Q | KEYMOD_SHIFT:
		case KEY_UP:
		case KEY_UP | KEYMOD_SHIFT:
		case KEY_UP | KEYMOD_SHIFT | KEYMOD_ALT:
			gui_imageeditor_movecursor (ed, ed->info->x, ed->info->y-navinc);
			return 1;

		case KEY_A:
		case KEY_A | KEYMOD_SHIFT:
		case KEY_DOWN:
		case KEY_DOWN | KEYMOD_SHIFT:
		case KEY_DOWN | KEYMOD_SHIFT | KEYMOD_ALT:
			gui_imageeditor_movecursor (ed, ed->info->x, ed->info->y+navinc);
			return 1;

		case KEY_RETURN:
		case KEY_RETURN | KEYMOD_ALT:
			gui_imageeditor_mousebutton (c, MB_EMUL, MB_EMUL, 0, 1);
			return 1;

		case KEY_RETURN | KEYMOD_SHIFT:
		case KEY_RETURN | KEYMOD_CONTROL:
		case KEY_RETURN | KEYMOD_SHIFT | KEYMOD_ALT:
		case KEY_RETURN | KEYMOD_CONTROL | KEYMOD_ALT:
			gui_imageeditor_mousebutton (c, MB_EMUL, MB_EMUL, 1, 1);
			return 1;

		/* Undo/redo */

		case KEY_Z | KEYMOD_CONTROL:
			gui_imageeditor_undo (ed);
			return 1;

		case KEY_Z | KEYMOD_CONTROL | KEYMOD_SHIFT:
			gui_imageeditor_redo (ed);
			return 1;

		/* Selection operations */

		case KEY_K: /* Crop */
			if (!ed->selection_empty)
				gui_imageeditor_setgraph(ed, gui_imageeditor_getselection(ed));
			return 1;

		/* Tool selection */

		case KEY_Z:
			ed->info->tool = TOOL_ZOOM;
			break;
		case KEY_F1:
			ed->info->tool = TOOL_PIXEL;
			break;
		case KEY_F2:
			ed->info->tool = TOOL_PENCIL;
			break;
		case KEY_F12:
			ed->info->tool = TOOL_CPE;
			break;
		case KEY_F3:
			ed->info->tool = TOOL_LINE;
			break;
		case KEY_F4:
			ed->info->tool = TOOL_MULTILINE;
			break;
		case KEY_F5:
			ed->info->tool = TOOL_CURVE;
			break;
		case KEY_F6:
			ed->info->tool = TOOL_POLYCURVE;
			break;
		case KEY_F7:
			ed->info->tool = TOOL_RECT;
			break;
		case KEY_F8:
			ed->info->tool = TOOL_CIRCLE;
			break;
		case KEY_F9:
			ed->info->tool = TOOL_AIRBRUSH;
			break;
		case KEY_F10:
			ed->info->tool = TOOL_FILL;
			break;
		case KEY_F11:
			ed->info->tool = TOOL_SELECT;
			break;
		case KEY_M:
			ed->info->tool = TOOL_WAND;
			break;
	}
	if (key == '>')
	{
		if (ed->info->cpoint < 999)
			ed->info->cpoint++;
		return 1;
	}
	if (key == '<')
	{
		if (ed->info->cpoint)
			ed->info->cpoint--;
		return 1;
	}
	return 0;
}

/** Return the cursor style at the given internal coordinate
 *
 *	@param c			Pointer to the control
 *	@param x			X coordinate
 *	@param y			Y coordinate
 */

int gui_imageeditor_mousepointer (CONTROL * c, int x, int y)
{
	IMAGEEDITOR * ed = (IMAGEEDITOR *)c;
	if (ed->info->zoom < 0) 
		ed->info->zoom = 1;
	x = x * 100 / ed->info->zoom;
	y = y * 100 / ed->info->zoom;

	if (gr_key (KEY_SPACE))
		return POINTER_HAND;

	if (!ed->active)
		return POINTER_ARROW;

	if (gr_key (KEY_LALT))
		return POINTER_DROPPER;

	if (ed->info->tool >= 0 && ed->info->tool < TOOL_COUNT)
	{
		if (imageeditor_tools[ed->info->tool]->pointer)
			return (*imageeditor_tools[ed->info->tool]->pointer)(ed, x, y);
	}
	return POINTER_CROSS;
}

/** Create an image editor control
 *
 *	@param graph	Image to be edited
 *	@param info		Pointer to user-supplied editor information struct
 *	@param width	Width, in pixels, of the control
 *	@param height	Height, in pixels, of the control
 *  @return			Pointer to the new object
 */

CONTROL * gui_imageeditor_new (GRAPH * graph, IMAGEEDITORINFO * info, int width, int height)
{
	CONTROL * control;
	SCROLLABLE * scrollable;
	IMAGEEDITOR * ed;
	
	/* Initialize parent class */
	ed = (IMAGEEDITOR *) malloc(sizeof(IMAGEEDITOR));
	scrollable = &ed->scrollable;
	control = &scrollable->control;
	gui_scrollable_init (scrollable, width+2, height+2);

	/* Configure parent class */
	control->hresizable = 1;
	control->vresizable = 1;
	control->innerdrag = 1;
	control->allkeys = 1;
	scrollable->autocenter = 1;
	gui_scrollable_resizearea (control, graph->width * info->zoom / 100, graph->height * info->zoom / 100);

	/* Initialize data members */
	ed->temporary		= NULL;
	ed->selection		= bitmap_new (0, graph->width, graph->height, 1);
	ed->clickx			= -1;
	ed->clicky			= -1;
	ed->info			= info;
	ed->graph			= graph;
	ed->space_drag      = 0;
	ed->painting        = 0;
	ed->currentfg       = 0;
	ed->currentfg8      = 0;
	ed->currentbg       = 0;
	ed->currentbg8      = 0;
	ed->active			= 0;
	ed->lastpx			= INT_MIN;
	ed->lastpy			= INT_MIN;
	ed->currentbg       = info->bg;
	ed->currentbg8      = info->bg8;
	ed->currentfg       = info->fg;
	ed->currentfg8      = info->fg8;

	ed->hants			= 0;
	ed->hants_count		= 0;
	ed->hants_allocated	= 0;
	ed->vants			= 0;
	ed->vants_count		= 0;
	ed->vants_allocated	= 0;
	ed->selection_dirty = 0;
	ed->selection_empty = 1;

	ed->undo            = 0;
	ed->undo_allocated  = 0;
	ed->undo_pos        = 0;
	ed->undo_used       = 0;
	ed->undo_memory     = 0;

	ed->mpoints         = 0;
	ed->mpoints_allocated = 0;
	ed->mpoints_count   = 0;

	/* Set the member pointers */
	scrollable->draw = gui_imageeditor_draw;
	scrollable->mousebutton = gui_imageeditor_mousebutton;
	scrollable->mousemove = gui_imageeditor_mousemove;
	scrollable->mousepointer = gui_imageeditor_mousepointer;
	scrollable->key = gui_imageeditor_key;

	gr_clear (ed->selection);
	return control;
}


