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
 *  Contains the CONTROL:IMAGEPALETTE class member functions
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

static GRAPH * rainbow = NULL;
static CONTROL * tooltable = NULL;
static CONTROL * brushtable = NULL;
static CONTROL * texturetable = NULL;
static int brush_fpg = -1;
static int toolgraphlist[] = { 111, 100, 101, 108, 112, 110, 102, 103, 114, 109, 106, 104, 0 };

/** Given a brush graph and a color, create a graph to show the brush's aspect
 *
 *	@param brush	Brush graph
 *	@param color	Foreground color (16 bits)
 */

GRAPH * gui_renderbrush (GRAPH * brush, int color)
{
	GRAPH * render = bitmap_new (brush->code, brush->width, brush->height, 16);
	int x, y, r, g, b;

	gr_get_rgb (color, &r, &g, &b);

	for (y = 0 ; y < brush->height ; y++)
	{
		Uint8  * ptr = (Uint8  *)brush->data + brush->pitch * y;
		Uint16 * dst = (Uint16 *)render->data + render->pitch * y / 2;

		for (x = 0 ; x < brush->width ; x++, ptr++, dst++)
		{
			if (!*ptr)
				*dst = 0;
			else
				*dst = gr_rgb (r**ptr/255, g**ptr/255, b**ptr/255);
		}
	}
	grlib_add_map (brush_fpg, render);
	return render;
}

/** Given a brush library and a color, create an alternative FPG
 *  with all the brushes rendered with gui_renderbrush()
 */

int gui_renderbrushes (int library, int color)
{
	int i;
	GRAPH * graph;

	if (brush_fpg == -1)
		brush_fpg = grlib_new();

	for (i = 1 ; i <= 999 ; i++)
	{
		graph = bitmap_get (library, i);
		if (graph)
			gui_renderbrush(graph, color);
	}
	return brush_fpg;
}

/** Show the tool pop-up
 *
 *	@param c		Pointer to the palette control
 */

void gui_editorpalette_showtoolspopup (CONTROL * c)
{
	CONTROL * table = gui_table_new(72, 208);
	EDITORPALETTE * p = (EDITORPALETTE *)c;
	GRAPH * graph;
	int i;
	LISTCOLUMN col;

	col = gui_table_column();
	col.width = 30;
	col.type = LIST_BITMAPS;
	col.library = p->palette_file;
	col.lineheight = 30;
	gui_table_addcolumn (table, &col, 0);

	for (i = 0 ; toolgraphlist[i] ; i++)
	{
		graph = bitmap_get (p->palette_file, toolgraphlist[i]);
		if (graph)
		{
			int n = string_itoa (toolgraphlist[i]);
			gui_table_addline (table, 1, &n);
			if (p->info->tool == toolgraphlist[i]-100)
				gui_table_select (table, ((TABLE *)table)->lines-1);
			string_discard(n);
		}
	}
	((TABLE *)table)->pc_padding = 0;
	((TABLE *)table)->pc_spacing = 0;
	((SCROLLABLE *)table)->border = 2;
	gui_table_enablepreview (table, 34, 34, 0, 0);
	gui_desktop_addpopup (c->window, table, 0, - table->height);
	tooltable = table;
}

/** Show the brushes pop-up
 *
 *	@param c		Pointer to the palette control
 */

void gui_editorpalette_showbrushespopup (CONTROL * c)
{
	CONTROL * table = gui_table_new(128, 208);
	EDITORPALETTE * p = (EDITORPALETTE *)c;
	GRAPH * graph;
	int i, n;
	LISTCOLUMN col;

	col = gui_table_column();
	col.width = 30;
	col.type = LIST_BITMAPS;
	col.library = gui_renderbrushes(p->info->brush_file, p->info->fg);;
	col.lineheight = 30;
	gui_table_addcolumn (table, &col, 0);

	for (i = 1 ; i < 100 ; i++)
	{
		graph = bitmap_get (p->info->brush_file, i);
		if (!graph) break;
		n = string_itoa (i);
		gui_table_addline (table, 1, &n);
		if (p->info->brush_graph == i)
			gui_table_select (table, ((TABLE *)table)->lines-1);
		string_discard(n);
	}
	((TABLE *)table)->pc_padding = 0;
	((TABLE *)table)->pc_spacing = 0;
	((SCROLLABLE *)table)->border = 2;
	gui_table_enablepreview (table, 26, 26, 0, 0);
	gui_desktop_addpopup (c->window, table, 258 - table->width/2, -table->height);
	brushtable = table;
}

/** Show the textures pop-up
 *
 *	@param c		Pointer to the palette control
 */

void gui_editorpalette_showtexturespopup (CONTROL * c)
{
	CONTROL * table = gui_table_new(128, 208);
	EDITORPALETTE * p = (EDITORPALETTE *)c;
	GRAPH * graph;
	int i, n;
	LISTCOLUMN col;

	col = gui_table_column();
	col.width = 30;
	col.type = LIST_BITMAPS;
	col.library = p->info->texture_file;
	col.lineheight = 30;
	gui_table_addcolumn (table, &col, 0);

	n = string_itoa (0);
	gui_table_addline (table, 1, &n);
	string_discard(n);

	for (i = 1 ; i < 100 ; i++)
	{
		graph = bitmap_get (p->info->texture_file, i);
		if (!graph) break;
		n = string_itoa (i);
		gui_table_addline (table, 1, &n);
		if (p->info->texture_graph == i)
			gui_table_select (table, ((TABLE *)table)->lines-1);
		string_discard(n);
	}
	((TABLE *)table)->pc_padding = 0;
	((TABLE *)table)->pc_spacing = 1;
	((SCROLLABLE *)table)->border = 2;
	gui_table_enablepreview (table, 40, 40, 0, 0);
	gui_desktop_addpopup (c->window, table, 288 - table->width/2, -table->height);
	texturetable = table;
}

/** Function called when a mouse button is pressed or released in an editor palette
 *
 *  @param c			Pointer to the control member (at offset 0 of the class)
 *  @param x			X Coordinate
 *  @param y			Y Coordinate
 *	@param b			Mouse button (0 left, 1 right, 2 middle, 3 wheel up, 4 wheel down)
 *	@param pressed		1 if mouse button was pressed, 0 if released
 */

int gui_editorpalette_mousebutton (CONTROL * c, int x, int y, int bt, int pressed)
{
	EDITORPALETTE * p = (EDITORPALETTE *)c;

	/* Click on tool */
	if (x < 32)
	{
		gui_editorpalette_showtoolspopup (c);
		return 1;
	}
	/* Click on foreground color */
	else if (x >= 97 && x <= 122 && y < 13)
	{
		gui_desktop_removepopup (0);
		gui_desktop_addpopup (c->window, 
			gui_colorchooser_newc(&p->info->fg, 
				CCM_ENABLEOK | CCM_ENABLEPALETTE | CCM_ENABLETRANSPARENT), -1, -175);
		return 1;
	}
	/* Click on background color */
	else if (x >= 107 && x <= 132 && y >= 9)
	{
		gui_desktop_removepopup (0);
		gui_desktop_addpopup (c->window, 
			gui_colorchooser_newc(&p->info->bg, 
				CCM_ENABLEOK | CCM_ENABLEPALETTE |CCM_ENABLETRANSPARENT | CCM_ENABLENONE), -1, -175);
		return 1;
	}
	/* Click on brush */
	else if (x < 271 && x > 241)
	{
		gui_editorpalette_showbrushespopup (c);
		return 1;
	}
	/* Click on textures */
	else if (x < 301 && x >= 271)
	{
		gui_editorpalette_showtexturespopup (c);
		return 1;
	}
	/* Click on rainbow */
	else if (rainbow && x >= 138 && x < 138+rainbow->width && y > 0)
	{
		x -= 138;
		y -= 1;
		
		if (x < rainbow->width && y < rainbow->height)
		{
			int color = gr_get_pixel (rainbow, x, y);

			if (pressed == 1 && bt == 0)
				p->info->fg = color;
			else if (pressed == 1 && bt == 1)
				p->info->bg = color;
			return 1;
		}
	}
	/* Click on RGBA/HSLA label */
	else if (x >= 300 && x < 320 && pressed >= 1)
	{
		p->rgb = !p->rgb;
		return 1;
	}
	/* Click on RGBA/HSLA sliders */
	else if (x >= 320 && x <= 384 && pressed >= 1)
	{
		int r, g, b, h, s, l, c;

		c = (bt == 0 ? p->info->fg : p->info->bg);
		gr_get_rgb (c, &r, &g, &b);
		if (p->rgb)
		{
			x = (x-320)*4+2;
			if (x > 255) x = 255;
			if (y < 8)			c = gr_rgb (x, g, b);
			else if (y < 16)	c = gr_rgb (r, x, b);
			else if (y < 24)	c = gr_rgb (r, g, x);
			else				p->info->alpha = x;
		}
		else
		{
			if (p->h != -1)
				h = p->h, s = p->s, l = p->l;
			else
				rgb2hsl (r, g, b, &h, &s, &l);

			x += 2;
			if (y < 8)
			{
				x = (x-320)*360/64;
				if (x > 360) x = 360;
				hsl2rgb (h = x, s, l, &r, &g, &b);
				c = gr_rgb (r, g, b);
			}
			else if (y < 16)
			{
				x = (x-320)*100/64;
				if (x > 100) x = 100;
				hsl2rgb (h, s = x, l, &r, &g, &b);
				c = gr_rgb (r, g, b);
			}
			else if (y < 24)
			{
				x = (x-320)*100/64;
				if (x > 100) x = 100;
				hsl2rgb (h, s, l = x, &r, &g, &b);
				c = gr_rgb (r, g, b);
			}
			else 
			{
				x = (x-320)*4;
				if (x > 255) x = 255;
				p->info->alpha = x;
			}
			p->h = h;
			p->s = s;
			p->l = l;
		}
		if (bt == 0)
			p->info->fg = c;
		else
			p->info->bg = c;
		return 1;
	}
	return 0;
}

/** Function called when the mouse is moved inside the editor palette
 *
 *	@param c			Pointer to the control
 *  @param x			X Coordinate
 *  @param y			Y Coordinate
 *	@param buttons		Button state array (1 bit per button)
 */

int gui_editorpalette_mousemove (CONTROL * c, int x, int y, int buttons)
{
	if (x >= 320 || (rainbow && buttons && x >= 138 && y >= 1 && x < 138+rainbow->width && y < 1+rainbow->height))
	{
		if (buttons & 1)
			gui_editorpalette_mousebutton (c, x, y, 0, 1);
		if (buttons & 2)
			gui_editorpalette_mousebutton (c, x, y, 1, 1);
		return 1;
	}
	return 0;
}

/** Return the mouse pointer cursor
 *
 *	@param c			Pointer to the control
 *	@param x			Mouse X coordinate 
 *	@param y			Mouse Y coordinate
 */

int gui_editorpalette_mousepointer (CONTROL * c, int x, int y)
{
	if (rainbow && x >= 138 && y >= 1 && x < 138+rainbow->width && y < 1+rainbow->height)
		return POINTER_DROPPER;
	return 0;
}

/** Draw an editor palette
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

void gui_editorpalette_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	EDITORPALETTE * p = (EDITORPALETTE *)c;
	char buffer[32];
	GRAPH * tool = bitmap_get (p->palette_file, 100 + p->info->tool);
	GRAPH * brush = bitmap_get (p->info->brush_file, p->info->brush_graph);
	GRAPH * texture = bitmap_get (p->info->texture_file, p->info->texture_graph);
	GRAPH * graph = bitmap_get (p->info->file, p->info->graph);
	int cx, cy;
	int r, g, b, h, s, l, r2, g2, b2, a;

	/* Check for tool/brush/texture change in a popup window */

	if (tooltable)
	{
		if (!gui_desktop_ispopup(tooltable))
		{
			tooltable = NULL;
		}
		else if (p->info->tool != toolgraphlist[((TABLE *)tooltable)->line]-100)
		{
			p->info->tool = toolgraphlist[ ((TABLE *)tooltable)->line ]-100;
			gui_desktop_removepopup(0);
			tooltable = NULL;
		}
	}

	if (brushtable)
	{
		if (!gui_desktop_ispopup(brushtable))
		{
			brushtable = NULL;
		}
		else if (p->info->brush_graph != ((TABLE *)brushtable)->line+1)
		{
			p->info->brush_graph = ((TABLE *)brushtable)->line+1;
			gui_desktop_removepopup(0);
			brushtable = NULL;
		}
	}

	if (texturetable)
	{
		if (!gui_desktop_ispopup(texturetable))
		{
			texturetable = NULL;
		}
		else if (p->info->texture_graph != ((TABLE *)texturetable)->line)
		{
			p->info->texture_graph = ((TABLE *)texturetable)->line;
			gui_desktop_removepopup(0);
			texturetable = NULL;
		}
	}

	/* Draw current tool */
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x, y, 32);
	gr_vline (dest, clip, x, y, 32);
	gr_setcolor (color_highlight);
	gr_hline (dest, clip, x, y+31, 32);
	gr_vline (dest, clip, x+31, y, 32);
	if (tool) gr_blit (dest, clip, x+16, y+16, 0, tool);

	/* Draw cursor position */
	sprintf (buffer, "X: %04d", p->info->x);
	gr_text_put (dest, clip, 0, x+40, y+2, buffer);
	sprintf (buffer, "Y: %04d", p->info->y);
	gr_text_put (dest, clip, 0, x+40, y+12, buffer);
	if (graph && p->info->x >= 0 && p->info->y >= 0 && p->info->x < graph->width && p->info->y < graph->height)
	{
		sprintf (buffer, graph->depth == 8 ? "C: %04d" : "C: %04X", 
			gr_get_pixel (graph, p->info->x, p->info->y));
		gr_text_put (dest, clip, 0, x+40, y+22, buffer);
	}

	/* Draw foreground/background */
	gr_setcolor (0);
	gr_rectangle (dest, clip, x+106, y+8, 24, 24);
	if (p->info->bg == 0)
	{
		gr_setcolor (gr_rgb(180,180,180));
		gr_box (dest, clip, x+107, y+9, 11, 11);
		gr_box (dest, clip, x+118, y+20, 11, 11);
		gr_setcolor (gr_rgb(128,128,128));
		gr_box (dest, clip, x+107, y+20, 11, 11);
		gr_box (dest, clip, x+118, y+9, 11, 11);
	}
	else if (p->info->bg == -1)
	{
		gr_line (dest, clip, x+106, y+8, 24, 24);
		gr_line (dest, clip, x+106+23, y+8, -24, 24);
	}
	else
	{
		gr_setcolor (p->info->bg);
		gr_box (dest, clip, x+107, y+9, 22, 22);
	}
	gr_setcolor (0);
	gr_rectangle (dest, clip, x+96, y, 24, 24);
	if (p->info->fg == 0)
	{
		gr_setcolor (gr_rgb(180,180,180));
		gr_box (dest, clip, x+97, y+1, 11, 11);
		gr_box (dest, clip, x+108, y+12, 11, 11);
		gr_setcolor (gr_rgb(128,128,128));
		gr_box (dest, clip, x+97, y+12, 11, 11);
		gr_box (dest, clip, x+108, y+1, 11, 11);
	}
	else
	{
		gr_setcolor (p->info->fg);
		gr_box (dest, clip, x+97, y+1, 22, 22);
	}

	/* Draw the color rainbow */
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x+137, y, 98);
	gr_vline (dest, clip, x+137, y, 32);
	gr_setcolor (color_highlight);
	gr_hline (dest, clip, x+137, y+31, 98);
	gr_vline (dest, clip, x+137+97, y, 32);

	if (rainbow == NULL)
	{
		rainbow = bitmap_new (0, 96, 31, 16);
		for (cx = 0; cx < 96 ; cx++)
		{
			for (cy = 0 ; cy < 31 ; cy++)
			{
				hsl2rgb ((int)(cx*3.75f), 100, (int)((cy+1)*3.25f), &r, &g, &b);
				gr_put_pixel (rainbow, cx, cy, gr_rgb(r,g,b));
			}
		}
		bitmap_add_cpoint (rainbow, 0, 0);
	}
	gr_blit (dest, clip, x+138, y+1, 0, rainbow);

	/* Draw current brush and texture */
	gr_setcolor (0);
	gr_rectangle (dest, clip, x+241, y+1, 31, 31);
	gr_rectangle (dest, clip, x+271, y+1, 31, 31);
	if (brush && p->info->brush_graph)
	{
		brush = gui_renderbrush (brush, p->info->fg);
		gui_scale_image (dest, x+241, y+3, x+272, y+30, 0, clip, brush);
	}
	if (texture && p->info->texture_graph)
		gui_scale_image (dest, x+272, y+3, x+302, y+30, 0, clip, texture);


	/* Draw the alpha slider */
	gr_text_put (dest, clip, 0, x+308, y+24, "A");
	a = p->info->alpha;
	for (cx = 0 ; cx < 64 ; cx++)
	{
		if ((cx % 8) > 3)
		{
			gr_setcolor (gr_rgb (128+cx*2, 128+cx*2, 128+cx*2));
			gr_vline (dest, clip, x+cx+320, y+24, 4);
			gr_setcolor (gr_rgb (cx*4, cx*4, cx*4));
			gr_vline (dest, clip, x+cx+320, y+28, 4);
		}
		else
		{
			gr_setcolor (gr_rgb (cx*4, cx*4, cx*4));
			gr_vline (dest, clip, x+cx+320, y+24, 4);
			gr_setcolor (gr_rgb (128+cx*2, 128+cx*2, 128+cx*2));
			gr_vline (dest, clip, x+cx+320, y+28, 4);
		}
	}
	gr_setcolor (0);
	gr_hline (dest, clip, x+320+a/4-1, y+32, 3);
	gr_hline (dest, clip, x+320+a/4, y+31, 1);
	gr_setcolor (gr_rgb(255,255,255));
	gr_line (dest, clip, x+320+a/4-2, y+32, 3, -3);
	gr_line (dest, clip, x+320+a/4+2, y+32, -2, -2);
	gr_hline (dest, clip, x+320+a/4-2, y+33, 5);

	if (p->rgb == 1)
	{
		/* Draw RGBA sliders */
		gr_text_put (dest, clip, 0, x+308, y+0, "R");
		gr_text_put (dest, clip, 0, x+308, y+8, "G");
		gr_text_put (dest, clip, 0, x+308, y+16, "B");

		gr_get_rgb (p->info->fg, &r, &g, &b);
		for (cx = 0 ; cx < 64 ; cx++)
		{
			gr_setcolor (gr_rgb (cx*4, g, b));
			gr_vline (dest, clip, x+cx+320, y+0, 7);
			gr_setcolor (gr_rgb (r, cx*4, b));
			gr_vline (dest, clip, x+cx+320, y+8, 7);
			gr_setcolor (gr_rgb (r, g, cx*4));
			gr_vline (dest, clip, x+cx+320, y+16, 7);
		}
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+r/4-1, y+6, 3);
		gr_hline (dest, clip, x+320+r/4, y+5, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+r/4-2, y+6, 3, -3);
		gr_line (dest, clip, x+320+r/4+2, y+6, -2, -2);
		gr_hline (dest, clip, x+320+r/4-2, y+7, 5);
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+g/4-1, y+14, 3);
		gr_hline (dest, clip, x+320+g/4, y+13, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+g/4-2, y+14, 3, -3);
		gr_line (dest, clip, x+320+g/4+2, y+14, -2, -2);
		gr_hline (dest, clip, x+320+g/4-2, y+15, 5);
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+b/4-1, y+24, 3);
		gr_hline (dest, clip, x+320+b/4, y+23, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+b/4-2, y+24, 3, -3);
		gr_line (dest, clip, x+320+b/4+2, y+24, -2, -2);
		gr_hline (dest, clip, x+320+b/4-2, y+25, 5);
	}
	else
	{
		/* Draw HSL sliders */
		gr_text_put (dest, clip, 0, x+308, y+0, "H");
		gr_text_put (dest, clip, 0, x+308, y+8, "S");
		gr_text_put (dest, clip, 0, x+308, y+16, "L");

		if (p->h != -1)
		{
			h = p->h;
			s = p->s;
			l = p->l;
			hsl2rgb (h, s, l, &r, &g, &b);
			if (gr_rgb (r, g, b) != p->info->fg)
			{
				gr_get_rgb (p->info->fg, &r, &g, &b);
				rgb2hsl (r, g, b, &h, &s, &l);
			}
		}
		else
		{
			gr_get_rgb (p->info->fg, &r, &g, &b);
			rgb2hsl (r, g, b, &h, &s, &l);
		}
		p->h = h, p->s = s, p->l = l;

		for (cx = 0 ; cx < 64 ; cx++)
		{
			hsl2rgb (cx*360/64, s, l, &r2, &g2, &b2);
			gr_setcolor (gr_rgb (r2, g2, b2));
			gr_vline (dest, clip, x+cx+320, y+0, 7);
			hsl2rgb (h, cx*100/64, l, &r2, &g2, &b2);
			gr_setcolor (gr_rgb (r2, g2, b2));
			gr_vline (dest, clip, x+cx+320, y+8, 7);
			hsl2rgb (h, s, cx*100/64, &r2, &g2, &b2);
			gr_setcolor (gr_rgb (r2, g2, b2));
			gr_vline (dest, clip, x+cx+320, y+16, 7);
		}
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+h*64/360-1, y+6, 3);
		gr_hline (dest, clip, x+320+h*64/360, y+5, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+h*64/360-2, y+6, 3, -3);
		gr_line (dest, clip, x+320+h*64/360+2, y+6, -2, -2);
		gr_hline (dest, clip, x+320+h*64/360-2, y+7, 5);
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+s*64/100-1, y+14, 3);
		gr_hline (dest, clip, x+320+s*64/100, y+13, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+s*64/100-2, y+14, 3, -3);
		gr_line (dest, clip, x+320+s*64/100+2, y+14, -2, -2);
		gr_hline (dest, clip, x+320+s*64/100-2, y+15, 5);
		gr_setcolor (0);
		gr_hline (dest, clip, x+320+l*64/100-1, y+24, 3);
		gr_hline (dest, clip, x+320+l*64/100, y+23, 1);
		gr_setcolor (gr_rgb(255,255,255));
		gr_line (dest, clip, x+320+l*64/100-2, y+24, 3, -3);
		gr_line (dest, clip, x+320+l*64/100+2, y+24, -2, -2);
		gr_hline (dest, clip, x+320+l*64/100-2, y+25, 5);
	}
}

/** Create a new editor palette 
 *
 */

CONTROL * gui_editorpalette_new (IMAGEEDITORINFO * info, int palette_file)
{
	EDITORPALETTE * p;

	p = (EDITORPALETTE *)malloc(sizeof(EDITORPALETTE));

	gui_control_init (&p->control);
	p->info = info;
	p->rgb = 0;
	p->h = -1;
	p->s = -1;
	p->l = -1;
	p->palette_file = palette_file;
	p->control.width = 384;
	p->control.height = 32;
	p->control.draw = gui_editorpalette_draw;
	p->control.mousebutton = gui_editorpalette_mousebutton;
	p->control.mousemove = gui_editorpalette_mousemove;
	p->control.mousepointer = gui_editorpalette_mousepointer;
	return &p->control;
}

