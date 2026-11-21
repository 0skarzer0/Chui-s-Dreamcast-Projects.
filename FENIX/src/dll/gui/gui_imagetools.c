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

void gui_imageeditor_destroytemporary (IMAGEEDITOR * ed);

static int use_bg = 0;

static void generic_render (IMAGEEDITOR * ed, GRAPH * graph)
{
	GRAPH * brush = bitmap_get (ed->info->brush_file, ed->info->brush_graph);
	GRAPH * texture = bitmap_get (ed->info->texture_file, ed->info->texture_graph);

	if (ed->info->texture_graph && texture)
		gdr_renderbytexture (graph, texture, ed->info->alpha, brush);
	else
	{
		if (use_bg)
			gdr_renderbycolor (graph, ed->info->bg, ed->info->fg, ed->info->alpha, brush);
		else
			gdr_renderbycolor (graph, ed->info->fg, ed->info->bg, ed->info->alpha, brush);

		use_bg = 0;
	}
}

static void null_render (IMAGEEDITOR * ed, GRAPH * graph)
{
}

static int generic_bbox (IMAGEEDITOR * ed, REGION * bbox)
{
	GRAPH * brush = bitmap_get (ed->info->brush_file, ed->info->brush_graph);
	return gdr_getboundingbox (bbox, ed->graph, brush);
}

/* --------------------------------------------------------------------------
 * Pencil tool 
 * --------------------------------------------------------------------------*/

void pencil_click (IMAGEEDITOR * ed, int x, int y, int button, int first) 
{
	if (first)
		gdr_reset();
	if (ed->lastpx != x || ed->lastpy != y)
	{
		if (ed->lastpx > INT_MIN && ed->lastpx > INT_MIN)
			gdr_addsegment(ed->lastpx, ed->lastpy, x, y);
		else
			gdr_addpixel(x, y);
		use_bg = button == 2;
		ed->lastpx = x;
		ed->lastpy = y;
	}
}

IMAGEEDITORTOOL tool_pencil = 
{
	/* Type		*/	TOOLTYPE_PENCIL, 
	/* Drag		*/	0, 
	/* Click	*/	pencil_click, 
	/* Pointer	*/	0, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

IMAGEEDITORTOOL tool_pixels = 
{
	/* Type		*/	TOOLTYPE_FILL, 
	/* Drag		*/	0, 
	/* Click	*/	pencil_click, 
	/* Pointer	*/	0, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Line tool 
 * --------------------------------------------------------------------------*/

void line_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	gdr_reset();
	gdr_addline(x, y, x2, y2);
}

int line_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_DRAWLINE;
}

IMAGEEDITORTOOL tool_line = 
{
	/* Type		*/	TOOLTYPE_LINE, 
	/* Drag		*/	line_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	line_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Multiline tool 
 * --------------------------------------------------------------------------*/

void multiline_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	int px = x2, py = y2, i;

	gdr_reset();
	if (ed->mpoints_count > 0)
	{
		px = ed->mpoints[0].x;
		py = ed->mpoints[0].y;
	
		for (i = 1 ; i < ed->mpoints_count ; i++)
		{
			gdr_addline (px, py, ed->mpoints[i].x, ed->mpoints[i].y);
			px = ed->mpoints[i].x;
			py = ed->mpoints[i].y;
		}
	}
	gdr_addline(px, py, x2, y2);
}

IMAGEEDITORTOOL tool_multiline = 
{
	/* Type		*/	TOOLTYPE_MULTILINE, 
	/* Drag		*/	multiline_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	line_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Box tool 
 * --------------------------------------------------------------------------*/

void rect_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	int cx = x + (x2-x)/2;
	int cy = y + (y2-y)/2;

	gdr_reset();
	if (gr_key (KEY_LSHIFT))
	{
		int r = abs(x2-x) > abs(y2-y) ? abs(x2-x) : abs(y2-y);
		int sx = x2 > x ? 1 : -1;
		int sy = y2 > y ? 1 : -1;
		if (gr_key(KEY_LCTRL))
			gdr_addrect (x-r*sx, y-r*sy, x+r*sx, y+r*sy, ed->info->bg != -1);
		else
			gdr_addrect (x, y, x+r*sx, y+r*sy, ed->info->bg != -1);
	}
	else
	{
		if (gr_key(KEY_LCTRL))
			gdr_addrect (x-(x2-x), y-(y2-y), x2, y2, ed->info->bg != -1);
		else
			gdr_addrect (x, y, x2, y2, ed->info->bg != -1);
	}
}

int rect_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_DRAWRECT;
}

IMAGEEDITORTOOL tool_rect = 
{
	/* Type		*/	TOOLTYPE_LINE, 
	/* Drag		*/	rect_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	rect_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Circle tool                                                               
 * --------------------------------------------------------------------------*/

void circle_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	gdr_reset();
	if (gr_key (KEY_LSHIFT))
	{
		if (gr_key (KEY_LCTRL))
			gdr_addcircle (x, y, abs(x2-x) > abs(y2-y) ? abs(x2-x) : abs(y2-y), ed->info->bg != -1);
		else
			if (abs(x2-x) > abs(y2-y))
				gdr_addcircle (x + (x2-x)/2, y + (x2-x)/2, abs(x2-x)/2, ed->info->bg != -1);
			else
				gdr_addcircle (x + (y2-y)/2, y + (y2-y)/2, abs(y2-y)/2, ed->info->bg != -1);
	}
	else
	{
		if (gr_key (KEY_LCTRL))
			gdr_addellipse (x, y, x2-x, y2-y, ed->info->bg != -1);
		else
			gdr_addellipse (x + (x2-x)/2, y + (y2-y)/2, (x2-x)/2, (y2-y)/2, ed->info->bg != -1);
	}
}

int circle_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_DRAWCIRCLE;
}

IMAGEEDITORTOOL tool_circle =
{
	/* Type		*/	TOOLTYPE_LINE, 
	/* Drag		*/	circle_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	circle_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Zoom tool 
 * --------------------------------------------------------------------------*/

void zoom_click (IMAGEEDITOR * ed, int x, int y, int button, int first)
{
	int z;

	if (gr_key(KEY_LSHIFT) || gr_key(KEY_LALT) || gr_key(KEY_RALT) || gr_key(KEY_RSHIFT))
		return;
	if (button & 1)
	{
		for (z = 25 ; z <= 6400 ; z *= 2)
			if (z > ed->info->zoom)
			{
				gui_imageeditor_zoomtopoint (ed, x, y, z);
				break;
			}
	}
	else
	{
		for (z = 50 ; z <= 6400 ; z *= 2)
			if (z >= ed->info->zoom)
			{
				gui_imageeditor_zoomtopoint (ed, x, y, z/2);
				break;
			}
	}
	if (ed->info->zoom > 6400)
		ed->info->zoom = 6400;
	if (ed->info->zoom < 25)
		ed->info->zoom = 25;
}

int zoom_pointer (IMAGEEDITOR * ed, int x, int y)
{
	if (gr_key(KEY_LSHIFT) || gr_key(KEY_LALT) || gr_key(KEY_RALT) || gr_key(KEY_RSHIFT))
		return 0;
	return POINTER_ZOOM;
}

IMAGEEDITORTOOL tool_zoom = 
{
	/* Type		*/	TOOLTYPE_FILL | TOOLTYPE_HARMLESS, 
	/* Drag		*/	0, 
	/* Click	*/	zoom_click, 
	/* Pointer	*/	zoom_pointer, 
	/* Draw		*/	0,
	/* BBox     */  0,
	/* Render	*/  null_render
};

/* --------------------------------------------------------------------------
 * Selection tool (by rectangle)
 * --------------------------------------------------------------------------*/

void select_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	if (button == 2)
	{
		ed->painting = 0;
		gui_imageeditor_removeselection(ed);
	}
	else if (final)
	{
		if (!gr_key(KEY_LSHIFT) && !gr_key(KEY_LCTRL))
			gr_clear (ed->selection);

		gr_setcolor (gr_key(KEY_LCTRL) ? 0:1);
		gr_box (ed->selection, NULL, x, y, x2-x+1, y2-y+1);
		ed->selection_dirty = 1;
	}
}

void select_draw (IMAGEEDITOR * ed, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	int s, n;
	if (ed->painting)
	{

		gr_setcolor (gr_rgb(255,255,255));
		s = drawing_stipple;
		n = 0x33333333 << ((gr_timer() / 100) % 4) ;
		n |= ((n & 0xF0) >> 4);
		drawing_stipple = n;
		gr_rectangle (dest, clip, 
			x-x0+ed->clickx*ed->info->zoom/100 - 1,
			y-y0+ed->clicky*ed->info->zoom/100 - 1,
			(ed->clicktox - ed->clickx + 1)*ed->info->zoom/100 + 2,
			(ed->clicktoy - ed->clicky + 1)*ed->info->zoom/100 + 2);
		drawing_stipple = ~n;
		gr_setcolor (gr_rgb(0,0,0));
		gr_rectangle (dest, clip, 
			x-x0+ed->clickx*ed->info->zoom/100 - 1,
			y-y0+ed->clicky*ed->info->zoom/100 - 1,
			(ed->clicktox - ed->clickx + 1)*ed->info->zoom/100 + 2,
			(ed->clicktoy - ed->clicky + 1)*ed->info->zoom/100 + 2);
		drawing_stipple = s;
	}
}

int select_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_DRAWSELECT;
}

IMAGEEDITORTOOL tool_select = 
{
	/* Type		*/	TOOLTYPE_LINE | TOOLTYPE_HARMLESS, 
	/* Drag		*/	select_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	select_pointer, 
	/* Draw		*/	select_draw,
	/* BBox     */  0,
	/* Render	*/  null_render
};

/* --------------------------------------------------------------------------
 * Control point editor 
 * --------------------------------------------------------------------------*/

void cpe_click (IMAGEEDITOR * ed, int x, int y, int button, int first)
{
	while (ed->graph->ncpoints <= ed->info->cpoint)
		bitmap_add_cpoint (ed->graph, CPOINT_UNDEFINED, CPOINT_UNDEFINED);

	if (x >= 0 && x < ed->graph->width)
		ed->graph->cpoints[ed->info->cpoint].x = x;
	if (y >= 0 && y < ed->graph->height)
		ed->graph->cpoints[ed->info->cpoint].y = y;
}

void cpe_draw (IMAGEEDITOR * ed, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	GRAPH * graph = ed->graph;
	char buffer[16];
	int n, s, w = gr_rgb(255,255,255), t;

	if (!graph->ncpoints)
		bitmap_add_cpoint (graph, graph->width/2, graph->height/2);

	s = 0x55555555 << ((gr_timer() / 100) % 4) ;
	s |= ((s & 0xF0) >> 4);

	t = ed->info->zoom/100;
	if (t < 1) t = 1;

	for (n = 0 ; n < graph->ncpoints ; n++)
	{
		int px = graph->cpoints[n].x;
		int py = graph->cpoints[n].y;
		int bx = x-x0 + px*ed->info->zoom/100 + ed->info->zoom/200;
		int by = y-y0 + py*ed->info->zoom/100 + ed->info->zoom/200;

		if (px < 0 || py < 0)
			continue;

		gr_setcolor (0);
		drawing_stipple = s;
		gr_rectangle (dest, clip, bx-t/2, by-t/2, t, t);
		drawing_stipple = ~s;
		gr_setcolor (w);
		gr_rectangle (dest, clip, bx-t/2, by-t/2, t, t);

		if (ed->info->cpoint == n)
		{
			t += 4 ;
			gr_setcolor (0);
			drawing_stipple = s;
			gr_rectangle (dest, clip, bx-t/2, by-t/2, t, t);
			drawing_stipple = ~s;
			gr_setcolor (w);
			gr_rectangle (dest, clip, bx-t/2, by-t/2, t, t);
			t -= 4;
			if (!n) 
				strcpy (buffer, "Center");
			else
				sprintf (buffer, "%d", n);
			by += ed->info->zoom/100 + 4;
			bx -= gr_text_width(0,buffer)/2;
			gr_text_setcolor (0);
			gr_text_put (dest, clip, 0, bx + 1, by, buffer);
			gr_text_put (dest, clip, 0, bx - 1, by, buffer);
			gr_text_put (dest, clip, 0, bx, by + 1, buffer);
			gr_text_put (dest, clip, 0, bx, by - 1, buffer);
			gr_text_setcolor (w);
			gr_text_put (dest, clip, 0, bx, by, buffer);
			gr_text_setcolor (0);
		}
	}

	drawing_stipple = 0xFFFFFFFF;
}

IMAGEEDITORTOOL tool_cpe = 
{
	/* Type		*/	TOOLTYPE_PENCIL, 
	/* Drag		*/	0, 
	/* Click	*/	cpe_click, 
	/* Pointer	*/	0, 
	/* Draw		*/	cpe_draw,
	/* BBox     */  0,
	/* Render	*/  null_render
};

/* --------------------------------------------------------------------------
 * Magic wand (selection by color)
 * --------------------------------------------------------------------------*/

void wand_click (IMAGEEDITOR * ed, int x, int y, int button, int first)
{
	int color = gr_get_pixel (ed->graph, x, y);

	if (!gr_key(KEY_LSHIFT) && !gr_key(KEY_RSHIFT))
		gui_imageeditor_removeselection(ed);

	if (button == 1)
	{
		/* Left mouse button: select by color, using fill */
		gdr_reset();
		gdr_addfill(x, y, ed->graph, 0, 0);
		if (gr_key(KEY_LCTRL) || gr_key(KEY_RCTRL))
			gui_imageeditor_invertselection(ed);
		gdr_renderbycolor (ed->selection, 1, 1, 255, 0);
		ed->selection_dirty = 1;
		if (gr_key(KEY_LCTRL) || gr_key(KEY_RCTRL))
			gui_imageeditor_invertselection(ed);
	}
	else
	{
		/* Right/middle mouse button: select all pixels of this color */
		if (gr_key(KEY_LCTRL) || gr_key(KEY_RCTRL))
			gui_imageeditor_invertselection(ed);
		gui_imageeditor_selectcolor (ed, color);
		if (gr_key(KEY_LCTRL) || gr_key(KEY_RCTRL))
			gui_imageeditor_invertselection(ed);
	}
}

int wand_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_WAND;
}

IMAGEEDITORTOOL tool_wand =
{
	/* Type		*/	TOOLTYPE_FILL | TOOLTYPE_HARMLESS, 
	/* Drag		*/	0, 
	/* Click	*/	wand_click, 
	/* Pointer	*/	wand_pointer, 
	/* Draw		*/	0,
	/* BBox     */  0,
	/* Render	*/  null_render
};

/* --------------------------------------------------------------------------
 * Bucket (seed fill)
 * --------------------------------------------------------------------------*/

void fill_click (IMAGEEDITOR * ed, int x, int y, int button, int first)
{
	int oldbg = ed->info->bg;
	gdr_reset();
	gdr_addfill(x, y, ed->graph, button == 1 ? 0 : 1,
		ed->graph->depth == 8 ? ed->info->bg8 : ed->info->bg);
	use_bg = 1;
}

int fill_pointer (IMAGEEDITOR * ed, int x, int y)
{
	return POINTER_BUCKET;
}

IMAGEEDITORTOOL tool_fill =
{
	/* Type		*/	TOOLTYPE_FILL, 
	/* Drag		*/	0, 
	/* Click	*/	fill_click, 
	/* Pointer	*/	fill_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Curve
 * --------------------------------------------------------------------------*/

void curve_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	int i;

	gdr_reset();
	if (ed->mpoints_count > 0)
	{
		for (i = 0 ; i < ed->mpoints_count ; i++)
		{
			if (i > 0 && (i % 3) == 0)
			{
				gdr_addcurve (
					ed->mpoints[i-3].x, ed->mpoints[i-3].y,
					ed->mpoints[i-2].x, ed->mpoints[i-2].y,
					ed->mpoints[i-1].x, ed->mpoints[i-1].y,
					ed->mpoints[i].x, ed->mpoints[i].y);
			}
			else if (!final)
				gdr_addpixel (ed->mpoints[i].x, ed->mpoints[i].y);
		}
		switch (i % 3)
		{
			case 0:
				gdr_addcurve (
					ed->mpoints[i-3].x, ed->mpoints[i-3].y,
					ed->mpoints[i-2].x, ed->mpoints[i-2].y,
					ed->mpoints[i-1].x, ed->mpoints[i-1].y, x2, y2);
				break;
			case 2:
				gdr_addcurve (
					ed->mpoints[i-2].x, ed->mpoints[i-2].y,
					ed->mpoints[i-2].x, ed->mpoints[i-2].y,
					ed->mpoints[i-1].x, ed->mpoints[i-1].y, x2, y2);
				break;
			case 1:
				gdr_addline (ed->mpoints[i-1].x, ed->mpoints[i-1].y, x2, y2);
		}
	}
}

IMAGEEDITORTOOL tool_curve =
{
	/* Type		*/	TOOLTYPE_MULTILINE, 
	/* Drag		*/	curve_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	line_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Polycurve
 * --------------------------------------------------------------------------*/

void polycurve_drag (IMAGEEDITOR * ed, int x, int y, int x2, int y2, int button, int final)
{
	int i, cx, cy;

	gdr_reset();
	if (ed->mpoints_count <= 0)
		gdr_addpixel (x2, y2);
	else if (ed->mpoints_count == 1)
		gdr_addline (ed->mpoints[0].x, ed->mpoints[0].y, x2, y2);
	else
	{
		for (i = 0 ; i < ed->mpoints_count-1 ; i++)
		{
			if (i < ed->mpoints_count-2)
			{
				cx = ed->mpoints[i].x + (ed->mpoints[i+1].x - ed->mpoints[i].x)/2 - (ed->mpoints[i+2].x - ed->mpoints[i+1].x)/2;
				cy = ed->mpoints[i].y + (ed->mpoints[i+1].y - ed->mpoints[i].y)/2 - (ed->mpoints[i+2].y - ed->mpoints[i+1].y)/2;
			}
			else
			{
				cx = ed->mpoints[i].x + (ed->mpoints[i+1].x - ed->mpoints[i].x)/2 - (x2 - ed->mpoints[i+1].x)/2;
				cy = ed->mpoints[i].y + (ed->mpoints[i+1].y - ed->mpoints[i].y)/2 - (y2 - ed->mpoints[i+1].y)/2;
			}
			gdr_addcurve (ed->mpoints[i].x, ed->mpoints[i].y,
				cx, cy, cx, cy, ed->mpoints[i+1].x, ed->mpoints[i+1].y);
		}
		if (!final)
			gdr_addline (ed->mpoints[i].x, ed->mpoints[i].y, x2, y2);
	}
}

IMAGEEDITORTOOL tool_polycurve =
{
	/* Type		*/	TOOLTYPE_MULTILINE | TOOLTYPE_DBLCLKADDS, 
	/* Drag		*/	polycurve_drag, 
	/* Click	*/	0, 
	/* Pointer	*/	line_pointer, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};

/* --------------------------------------------------------------------------
 * Paste
 * --------------------------------------------------------------------------*/

GRAPH * paste_graph = NULL;

void paste_click (IMAGEEDITOR * ed, int x, int y, int button, int first) 
{
	if (paste_graph)
		bitmap_destroy(paste_graph);
	paste_graph = gui_paste_graph();
}

void paste_render (IMAGEEDITOR * ed, GRAPH * graph)
{
	if (paste_graph)
	{
		int flags = 0;
		if (ed->info->alpha < 255)
			flags = (B_ALPHA | (ed->info->alpha << B_ALPHA_SHIFT));
		gr_blit (graph, NULL, ed->info->x, ed->info->y, flags, paste_graph);
	}
}

int paste_bbox (IMAGEEDITOR * ed, REGION * bbox)
{
	bbox->x = 0;
	bbox->y = 0;
	bbox->x2 = ed->graph->width-1;
	bbox->y2 = ed->graph->height-1;
	return 1;
}

void paste_draw (IMAGEEDITOR * ed, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	if (paste_graph)
	{
		int cx = ed->info->x * ed->info->zoom / 100;
		int cy = ed->info->y * ed->info->zoom / 100;
		int flags = 0;
		if (ed->info->alpha < 255)
			flags = (B_ALPHA | (ed->info->alpha << B_ALPHA_SHIFT));
		gr_rotated_blit (dest, clip, x-x0+cx, y-y0+cy, flags, 0, ed->info->zoom, ed->info->zoom, paste_graph);
	}
}

IMAGEEDITORTOOL tool_paste = 
{
	/* Type		*/	TOOLTYPE_FILL, 
	/* Drag		*/	0, 
	/* Click	*/	paste_click, 
	/* Pointer	*/	0, 
	/* Draw		*/	paste_draw,
	/* BBox     */  paste_bbox,
	/* Render	*/  paste_render
};

/* --------------------------------------------------------------------------
 * Airbrush
 * --------------------------------------------------------------------------*/

typedef struct _airbrushpx
{
	int		x;
	int		y;
	int		alpha;
}
AIRBRUSHPX;

static AIRBRUSHPX * airbrushpx = NULL;
static int          airbrushpx_allocated = 0;
static int          airbrushpx_count = 0;
static int          airbrushpx_pos;
static int          airbrushpx_file = -1;
static int          airbrushpx_graph = -1;

void airbrush_update (GRAPH * brush)
{
	static GRAPH * null_brush = NULL;
	Uint8 * ptr;
	int x, y, i, n;
	int cx, cy;

	airbrushpx_count = 0;
	airbrushpx_pos = 0;

	if (brush == NULL || brush->depth != 8 || brush->width < 4 || brush->height < 4)
	{
		if (null_brush == NULL)
		{
			null_brush = bitmap_new (0, 11, 11, 8);
			gr_clear(null_brush);
			gr_setcolor (255);
			gr_fcircle (null_brush, NULL, 5, 5, 4);
		}

		brush = null_brush;
	}

	/* Get the brush's center */

	if (brush->cpoints && brush->cpoints[0].x != CPOINT_UNDEFINED)
	{
		cx = brush->cpoints[0].x;
		cy = brush->cpoints[0].y;
	}
	else
	{
		cx = brush->width/2;
		cy = brush->height/2;
	}

	/* Get the brush pixels */

	for (i = 0 ; i < brush->width * brush->height ; i++)
	{
		if (i < brush->width*brush->height/2)
		{
			n = 0, x = 0;
			while (i > n) x++, n += x+1;
			y = x-(n-i);
			x -= y;
		}
		else
		{
			i -= brush->width*brush->height/2;
			n = 0, x = 0;
			while (i > n) x++, n += x+1;
			y = x-(n-i);
			x -= y;
			y = brush->height-1-y;
			x = brush->width-1-x;
			i += brush->width*brush->height/2;
		}

		ptr = (Uint8*)brush->data + brush->pitch * y + x;

		if (*ptr != 0)
		{
			if (airbrushpx_count == airbrushpx_allocated)
			{
				airbrushpx_allocated += 32;
				airbrushpx = (AIRBRUSHPX *) realloc (airbrushpx,
						airbrushpx_allocated * sizeof(AIRBRUSHPX));
			}

			airbrushpx[airbrushpx_count].x = x-cx;
			airbrushpx[airbrushpx_count].y = y-cy;
			airbrushpx[airbrushpx_count].alpha = *ptr;
			airbrushpx_count++;
		}
	}

	/* Scramble the data */

	for (i = 0 ; i < airbrushpx_count ; i++)
	{
		AIRBRUSHPX s;

		x = (int)((double)airbrushpx_count * rand() / (RAND_MAX+1.0));
		y = (int)((double)airbrushpx_count * rand() / (RAND_MAX+1.0));

		s = airbrushpx[x];
		airbrushpx[x] = airbrushpx[y];
		airbrushpx[y] = s;
	}
}

void airbrush_click (IMAGEEDITOR * ed, int x, int y, int button, int first) 
{
	if (first)
		gdr_reset();

	if (airbrushpx_file != ed->info->brush_file ||
		airbrushpx_graph != ed->info->brush_graph)
	{
		airbrushpx_file = ed->info->brush_file;
		airbrushpx_graph = ed->info->brush_graph;
		airbrush_update (bitmap_get(airbrushpx_file, airbrushpx_graph));
	}

	if (airbrushpx_count)
	{
		AIRBRUSHPX * brushpx = &airbrushpx[airbrushpx_pos];
		airbrushpx_pos++;
		if (airbrushpx_pos >= airbrushpx_count)
			airbrushpx_pos = 0;
		gdr_addpixel(x + brushpx->x, y + brushpx->y);
	}

	use_bg = button == 2;
	ed->lastpx = x;
	ed->lastpy = y;
}

IMAGEEDITORTOOL tool_airbrush = 
{
	/* Type		*/	TOOLTYPE_PENCIL | TOOLTYPE_TIMER, 
	/* Drag		*/	0, 
	/* Click	*/	airbrush_click, 
	/* Pointer	*/	0, 
	/* Draw		*/	0,
	/* BBox     */  generic_bbox,
	/* Render	*/  generic_render
};
