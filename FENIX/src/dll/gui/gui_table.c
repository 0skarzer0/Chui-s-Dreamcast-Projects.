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
 *  Contains the CONTROL:TABLE class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define COLSEP 16

/** Draw a preview-mode subtitle (using word wrap)
 *
 *	@param dest		Destination drawing bitmap
 *	@param text		Text to draw
 *	@param x0		Left coordinate of the internal rect to draw at (x, y)
 *	@param y0		Top coordinate of the internal rect to draw at (x, y)
 *	@param x1		Right coordinate of the internal rect to draw at (x, y)
 *	@param y1		Bottom coordinate of the internal rect to draw at (x, y)
 *  @param clip		Clipping region to apply or 0 if none
 */

void gui_text_put_ww (GRAPH * dest, REGION * clip, int x0, int y0, int x1, int y1, const char * text)
{
	char * last_stop = 0;
	char * newtext = strdup(text);
	char * ptr = newtext;
	char * line_start = newtext;
	int last_stop_x = 0;
	int current_x = x0;

	while (y0 < y1-7)
	{
		current_x += gr_text_widthn (0, ptr++, 1);
		if (current_x >= x1-12 || !*ptr)
		{
			if (last_stop && current_x >= x1-12)
			{
				char c = *last_stop;
				*last_stop = 0;
				gr_text_put (dest, clip, 0, x0 + (x1-last_stop_x)/2, y0, line_start);
				*last_stop = c;
				ptr = last_stop;
			}
			else
			{
				char c = *ptr;
				*ptr = 0;
				gr_text_put (dest, clip, 0, x0 + (x1-current_x)/2, y0, line_start);
				*ptr = c;
			}
			if (!*ptr) break;
			y0 += 8;
			current_x = x0;
			last_stop = 0;
			while (*ptr == ' ') ptr++;
			line_start = ptr;
			continue;
		}
		if (*ptr == ' ')
		{
			last_stop = ptr;
			last_stop_x = current_x;
		}
	}
	free(newtext);
}

/** Draw a list in preview mode
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *	@param x0		Left coordinate of the internal rect to draw at (x, y)
 *	@param y0		Top coordinate of the internal rect to draw at (x, y)
 *	@param x1		Right coordinate of the internal rect to draw at (x, y)
 *	@param y1		Bottom coordinate of the internal rect to draw at (x, y)
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_table_draw_preview (CONTROL * c, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	TABLE * table = (TABLE *)c;
	int cx, cy, n;
	int bx, by;
	int height = y1-y0+1;
	int width  = x1-x0+1;
	REGION * region;

	/* Draw the background */
	region = region_new (x, y, width, height);
	if (clip) region_union (region, clip);
	gr_setcolor (color_face);
	gr_box (dest, region, x, y, width, height);

	by = y0 / table->pc_height * table->pc_height;
	bx = x0 / table->pc_width  * table->pc_width;

	for (cy = by ; cy < y1 ; cy += table->pc_height)
	{
		n = cy/table->pc_height * table->pc_cols;

		for (cx = bx ; cx < x1 ; cx += table->pc_width, n++)
		{
			REGION cell, cellclip;

			if (n >= table->lines)
				break;
			if (n == (cy + table->pc_height)/table->pc_height * table->pc_cols)
				break;

			cell.x = x + cx-x0 + table->pc_padding;
			cell.y = y + cy-y0 + table->pc_padding;
			cell.x2 = cell.x + table->pc_width - table->pc_padding*2 - table->pc_spacing;
			cell.y2 = cell.y + table->pc_height - table->pc_padding*2 - table->pc_spacing;
			cellclip = cell;
			region_union (&cellclip, region);

			// Draw the background
			if (table->line == n || ((table->options & LIST_MULTISELECT) && 
				table->linesel_alloc > n && table->linesel[n]))
			{
				gr_setcolor (c->focused ? color_selection:color_highlight);
				gr_box (dest, &cellclip, cell.x, cell.y, table->pc_width-table->pc_spacing, table->pc_height-table->pc_spacing);
			}
			else if (table->highlight == n)
			{
				gr_setcolor (color_highlightface);
				gr_box (dest, &cellclip, cell.x, cell.y, table->pc_width-table->pc_spacing, table->pc_height-table->pc_spacing);
			}

			// Draw the graphic
			if (table->pc_graph >= 0)
			{
				GRAPH * graph = bitmap_get (table->column[table->pc_graph].library, 
					table->column[table->pc_graph].data[n]);
				int y2 = cell.y2;
				if (table->pc_title >= 0)
					y2 -= 17;
				if (graph)
					gui_scale_image (dest, cell.x, cell.y, cell.x2, y2, 0, &cellclip, graph);
			}

			// Draw the title
			if (table->pc_title >= 0)
			{
				const char * txt = string_get (table->column[table->pc_title].data[n]);
				if (txt)
					gui_text_put_ww (dest, &cellclip, cell.x, cell.y2 - 16, cell.x2, cell.y2, txt);
			}

			// Draw the highlight box, if any
			if (table->alphahighlight)
			{
				gr_setalpha (128);
				if (table->line == n || ((table->options & LIST_MULTISELECT) && 
					table->linesel_alloc > n && table->linesel[n]))
				{
					gr_setcolor (c->focused ? color_selection:color_highlight);
					gr_box (dest, &cellclip, cell.x, cell.y, table->pc_width-table->pc_spacing, table->pc_height-table->pc_spacing);
				}
				else if (table->highlight == n)
				{
					gr_setcolor (color_highlightface);
					gr_box (dest, &cellclip, cell.x, cell.y, table->pc_width-table->pc_spacing, table->pc_height-table->pc_spacing);
				}
				gr_setalpha (255);
			}
		}
	}

	region_destroy(region);
}

/** Draw a list.
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the button.control member (at offset 0 of the button)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *	@param x0		Left coordinate of the internal rect to draw at (x, y)
 *	@param y0		Top coordinate of the internal rect to draw at (x, y)
 *	@param x1		Right coordinate of the internal rect to draw at (x, y)
 *	@param y1		Bottom coordinate of the internal rect to draw at (x, y)
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_table_draw (CONTROL * c, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip)
{
	TABLE * list = (TABLE *)c ;
	REGION * region;
	int i, j, cx, cy;
	int height = y1-y0+1;
	int width  = x1-x0+1;
	int focused = c->focused || list->alphahighlight;

	gr_text_setcolor (color_text);

	/* Draw the title */
	if (list->title > 0)
	{
		gr_setcolor (color_highlightface);
		gr_box (dest, clip, x, y, width, list->title);
		gr_setcolor (color_highlight);
		gr_hline (dest, clip, x, y, width);
		gr_vline (dest, clip, x, y, list->title);
		gr_setcolor (color_shadow);
		gr_hline (dest, clip, x, y+list->title-1, width);
		gr_vline (dest, clip, x+width-1, y, list->title-1);
		cx = x+4-list->scroll.scrollx;
		for (j = 0 ; j < list->cols ; cx += list->column[j++].width)
		{
			int tx = cx, ty = y+2;
			REGION cell;

			cell.x = cx-5;
			cell.y = y;
			cell.x2 = cell.x + list->column[j].width-2;
			cell.y2 = cell.y + list->title-1;
			region_union (&cell, clip);

			if (list->hovered == j)
			{
				gr_setcolor (color_highlight);
				if (j == list->cols-1)
					gr_box (dest, &cell, cx-5-list->scroll.scrollx, y+1, x+width-1-(cx-5-list->scroll.scrollx), list->title-2);
				else
					gr_box (dest, &cell, cx-5-list->scroll.scrollx, y+1, list->column[j].width-2, list->title-2);
			}

			if (list->pressed == j)
			{
				gr_setcolor (color_shadow);
				gr_hline (dest, clip, cx-7, y, list->column[j].width);
				gr_vline (dest, clip, cx-7, y, list->title-1);
				tx++, ty++;
			}
			
			if (list->column[j].align == 1)
				tx += (list->column[j].width - gr_text_width (0, list->column[j].name))/2 - 6;
			else if (list->column[j].align == 2)
				tx += list->column[j].width - gr_text_width (0, list->column[j].name) - 12;
				
			gr_text_put (dest, &cell, 0, tx, ty, list->column[j].name);

			/* Sorted by mark */
			if (list->sorted_by == j)
			{
				int vx = cx;

				if (list->column[j].align == 2)
					vx -= 4;
				else
					vx += list->column[j].width - 20;

				gr_setcolor (list->hovered == j ? color_highlight : color_highlightface);
				gr_box (dest, clip, vx, y + 2, 12, 8);
				gr_setcolor (color_border);
				if (list->ascendant)
				{
					gr_hline (dest, &cell, vx + 2, y + 3, 8);
					gr_hline (dest, &cell, vx + 3, y + 4, 6);
					gr_hline (dest, &cell, vx + 4, y + 5, 4);
					gr_hline (dest, &cell, vx + 5, y + 6, 2);
				}
				else
				{
					gr_hline (dest, &cell, vx + 2, y + 6, 8);
					gr_hline (dest, &cell, vx + 3, y + 5, 6);
					gr_hline (dest, &cell, vx + 4, y + 4, 4);
					gr_hline (dest, &cell, vx + 5, y + 3, 2);
				}
			}
		}
		y += list->title;
	}

	/* Draw the background */
	region = region_new (x, y, width, height);
	if (clip) region_union (region, clip);
	gr_setcolor (color_face);
	gr_box (dest, region, x, y, width, height);

	/* Draw the contents */
	cy = -y0;
	for (i = 0 ; i < list->lines ; i++, cy += list->lineheight)
	{
		int bgcolor = -1;

		if (cy < -list->lineheight-4)
			continue;
		if (cy > c->height+4)
			break;

		/* Draw the row background */
		if ((list->options & LIST_MULTISELECT) && 
			list->linesel_alloc > i && list->linesel[i])
		{
			gr_setcolor (bgcolor = (focused ? color_selection:color_highlight));
			gr_box (dest, region, x, cy+y, width, list->lineheight);
		}
		else if (!(list->options & LIST_MULTISELECT) && i == list->line)
		{
			gr_setcolor (bgcolor = (focused ? color_selection:color_highlight));
			gr_box (dest, region, x, cy+y, width, list->lineheight);
		}
		else if (list->highlight == i)
		{
			gr_setcolor (bgcolor = color_highlightface);
			gr_box (dest, region, x, cy+y, width, list->lineheight);
		}
		else if (i & 2)
		{
			gr_setcolor (color_tablebg);
			gr_box (dest, region, x, cy+y, width, list->lineheight);
		}

		/* Draw each cell's contents */
		cx = - list->scroll.scrollx;
		for (j = 0 ; j < list->cols ; cx += list->column[j++].width)
		{
			REGION cell;

			if (list->column[j].count <= i)
				continue;

			cell.x = cx+x-1;
			cell.y = cy+y;
			cell.x2 = cell.x + list->column[j].width-2;
			cell.y2 = cell.y + list->lineheight-1;
			region_union (&cell, region);

			if (list->column[j].type == LIST_TEXTS ||
				list->column[j].type == LIST_CUSTOMTEXTS ||
				list->column[j].type == LIST_STRINGS)
			{
				const char * ptr;
				int tx = cx+x+1;

				if (list->column[j].type == LIST_CUSTOMTEXTS)
					ptr = (*(LISTTEXTPROC *)(list->column[j].data[i]))(i);
				else if (list->column[j].type == LIST_STRINGS)
					ptr = string_get (list->column[j].data[i]);
				else
					ptr = (const char *)(list->column[j].data[i]);
				
				if (!ptr) continue;

				if (list->column[j].align == 1)
					tx += (list->column[j].width - gr_text_width (0, ptr))/2 - 2;
				else if (list->column[j].align == 2)
					tx += list->column[j].width - gr_text_width (0, ptr) - 8;

				gr_text_put (dest, &cell, 0, tx, 
					cy+y+(list->lineheight-gr_text_height(0,ptr))/2, ptr) ;
				if (c->focused && i == list->line)
				{
					gr_setcolor (color_text);
					gr_line (dest, &cell, tx, 
						cy+y+(list->lineheight+gr_text_height(0,ptr))/2, 
						gr_text_width(0, ptr), 0 /*, 0x55555555*/);
				}
			}
			else if (list->column[j].type == LIST_BITMAPS)
			{
				GRAPH * graph = bitmap_get (list->column[j].library, list->column[j].data[i]);
				REGION cell;

				if (!graph || list->column[j].count <= i)
					continue;

				cell.x = cx+x-1;
				cell.y = cy+y;
				cell.x2 = cell.x + list->column[j].width-2;
				cell.y2 = cell.y + list->lineheight-1;
				region_union (&cell, region);
				gui_scale_image (dest, cx+x-1, cy+y, cx+x-1+list->column[j].width,
					cy+y+list->lineheight-1, 0, &cell, graph);
			}
			else if (list->column[j].type == LIST_CUSTOMDRAW)
				(*(LISTDRAWPROC)(list->column[j].data[i]))
				(i, dest, &cell, x, cy+y, c->width, list->lineheight);
		}

		/* Draw the highlight */
		if (list->alphahighlight && bgcolor != -1)
		{
			gr_setalpha(128);
			gr_setcolor (bgcolor);
			gr_box (dest, region, x, cy+y, width, list->lineheight);
			gr_setalpha(255);
		}
	}

	/* Draw vertical lines between columns */
	if (list->title)
	{
		cx = x - list->scroll.scrollx - 2;
		cy = y - list->title;
		for (j = 0 ; j <= list->cols ; j++)
		{
			gr_setcolor (list->pressed != j ? color_highlight : color_border);
			gr_vline (dest, clip, cx, cy, list->title-1);
			gr_setcolor (color_shadow);
			gr_vline (dest, clip, cx-1, cy, list->title-1);

			/* Commented - vertical lines between contents */
			//gr_vline (dest, clip, cx-1, cy + list->title, c->height);
			//gr_setcolor (color_highlight);
			//gr_vline (dest, clip, cx, cy + list->title, c->height);

			if (j == list->cols-1)
				cx += 2;
			if (j < list->cols)
				cx += list->column[j].width;
		}
	}

	region_destroy(region);
}

/** Fill the scroll struct with minimum and maximum values and change the
 *  scroll position to make the current line visible
 *
 *  @param list		Pointer to the list control
 */

void gui_table_fillscroll (TABLE * table)
{
	CONTROL * control = (CONTROL *)table;

	int i;
	int total_height;
	int total_width = 0;
	int ideal_width;
	int current_width;
	int autoexpand_count = 0;

	/* Preview mode */
	if (table->pc)
	{
		int cx = (table->line % table->pc_cols) * table->pc_width;
		int cy = (table->line / table->pc_cols) * table->pc_height;
		gui_scrollable_scrollto (control, cx, -control->height/2 + cy);
		return;
	}

	for (i = 0 ; i < table->cols ; i++)
	{
		if (table->lines < table->column[i].count) 
		    table->lines = table->column[i].count;
		total_width += table->column[i].width;
	}

	total_height = table->lineheight * table->lines;
	gui_scrollable_resizearea (&table->scroll.control, 
		total_width, total_height);

	/* If no horizontal scroll, maximize the table width */
	if (table->scroll.hscroll.control.hidden == 1)
	{
		total_width  = table->scroll.control.width - 2;
		if (table->scroll.vscroll.control.hidden == 0)
			total_width -= table->scroll.vscroll.control.width;
		gui_scrollable_resizearea (&table->scroll.control, 
			total_width, total_height);
	}

	/* Make the selected line visible */
	if (table->scroll.scrolly < table->lineheight * (table->line+1) - table->scroll.vpos.pagestep)
		gui_scrollable_scrollto (&table->scroll.control, table->scroll.scrollx,
			table->lineheight * (table->line+1) - table->scroll.vpos.pagestep);
	if (table->scroll.scrolly > table->lineheight * table->line)
		gui_scrollable_scrollto (&table->scroll.control, table->scroll.scrollx,
			table->lineheight * table->line);

	table->scroll.vpos.step = table->lineheight;

	/* Calculate ideal table width */
	ideal_width = table->scroll.areawidth;
	current_width = control->width-2;
	if (!table->scroll.vscroll.control.hidden)
		current_width -= table->scroll.vscroll.control.width;
	if (current_width < ideal_width)
		ideal_width = current_width;

	/* Find autoexpand-enabled columns to reach ideal table width */
	for (i = total_width = 0 ; i < table->cols ; i++)
	{
		total_width += table->column[i].width;
		if (table->column[i].autoexpand)
			autoexpand_count++;
	}

	/* Distribute remaining width if possible */
	if (ideal_width != total_width && autoexpand_count > 0)
	{
		int some_resized = 0;
		total_width = ideal_width - total_width;
		for (i = 0 ; i < table->cols ; i++)
		{
			if (table->column[i].autoexpand)
			{
				int inc = total_width / autoexpand_count;
				if (table->column[i].width + inc < table->column[i].minwidth)
					inc = table->column[i].minwidth - table->column[i].width;
				table->column[i].width += inc;
				total_width -= inc;
				if (inc != 0)
					some_resized = 1;
				autoexpand_count--;
			}
		}
		if (some_resized)
			gui_table_fillscroll(table);
	}
}

/** Return the cursor pointer
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the list.control member (at offset 0 of the list)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 */

int gui_table_mousepointer (CONTROL * c, int x, int y)
{
	TABLE * list = (TABLE *)c;
	int i;
	int cx = -2;

	if (list->drag >= 0)
		return POINTER_RESIZEH2;
	if (list->drag_column >= 0)
		return POINTER_HAND;

	if (y >= 0 && y <= list->title && (list->options & LIST_USER_RESIZABLE))
	{
		for (i = 0 ; i <= list->cols ; i++)
		{
			if (x >= cx - 2 && x <= cx + 2)
				return POINTER_RESIZEH2;
			if (i < list->cols)
				cx += list->column[i].width;
		}
	}
	return c->pointer;
}

/** Response to mouse move events
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the list.control member (at offset 0 of the list)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param buttons	Button state (1 bit for button)
 * */

int gui_table_mousemove (CONTROL * c, int x, int y, int buttons)
{
	TABLE * list = (TABLE *)c;

	// Preview mode: simply highlight the current cell
	if (list->pc)
	{
		int col = x / list->pc_width;

		if (col < list->pc_cols)
		{
			int row = y / list->pc_height;
			int n = row * list->pc_cols + col;
			if (n < list->lines)
			{
				if (list->highlight != n)
					list->highlight = n, c->redraw = 1;
			}
			else if (list->highlight != -1)
				list->highlight = -1, c->redraw = 1;
		}
		else
			list->highlight = -1, c->redraw = 1;
		return 1;
	}

	if (buttons && list->drag < 0 && list->pressed >= 0 && list->drag_column < 0)
		list->drag_column = list->pressed;

	if (buttons && list->drag >= 0)
	{
		list->column[list->drag].width = list->drag_width + (x - list->drag_x);
		if (list->column[list->drag].width < 4)
		    list->column[list->drag].width = 4;
		gui_table_fillscroll (list);
		c->redraw = 1;
		return 1;
	}

	if (buttons && list->drag_column >= 0)
	{
		int i, cx = 0;
		for (i = 0 ; i < list->cols ; i++)
		{
			cx += list->column[i].width;
			if (x < cx)
			{
				if (x >= cx - list->column[i].width + list->column[list->drag_column].width)
					return 1;
				if (i != list->drag_column)
				{
					LISTCOLUMN col;
					memcpy (&col, &list->column[list->drag_column], sizeof(LISTCOLUMN));
					memcpy (&list->column[list->drag_column], &list->column[i], sizeof(LISTCOLUMN));
					memcpy (&list->column[i], &col, sizeof(LISTCOLUMN));
					if (list->sorted_by == list->drag_column)
						list->sorted_by = i;
					else if (list->sorted_by == i)
						list->sorted_by = list->drag_column;
					list->drag_column = list->hovered = list->pressed = i;
					c->redraw = 1;
				}
				return 1;
			}
		}
		return 1;
	}

	y -= list->title;
	if (y < 0 && list->drag < 0 && list->options & (LIST_USER_MOVEABLE | LIST_USER_SORTABLE) && !buttons)
	{
		int i, cx = 0;
		for (i = 0 ; i < list->cols ; i++)
		{
			cx += list->column[i].width;
			if (x < cx)
			{
				if (list->hovered != i)
				{
					list->hovered = i;
					c->redraw = 1;
				}
				return 1;
			}
		}
	}

	list->hovered = -1;

	if (y < 0 && !buttons)
		return 0;
	if (y <= 0)
		y += list->scroll.vpos.pos;

	y /= list->lineheight;

	if (list->highlight != y)
	{
		list->highlight = y;
		c->redraw = 1;
	}
	if (buttons & 1)
	{
		if (y < list->lines)
		{
			if (y < 0)
				y = 0;
			if (y >= list->lines)
				y = list->lines-1;
			if (!(list->options & LIST_MULTISELECT))
				gui_table_select (c, y);
			//gui_table_fillscroll(list);
			c->redraw = 1;
		}
	}
	return 1;
}

/** Change the list's pressed state when the user clicks it.
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the list.control member (at offset 0 of the list)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the button pressed or released
 *  @param pressed	1 if the list was pressed, 0 if it was released
 * */

int gui_table_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	TABLE * list = (TABLE *)c;
	int i;
	int cx = -2;

	/* Disable dragging (it will be reactivated if the user clicks in a list entry */
	if (c->draggable)
		c->draggable = 2;

	/* Mouse wheel support */
	if (pressed > 0 && b == 3)
	{
		int inc = list->lineheight * 3;
		if (list->scroll.vpos.pos - inc < list->scroll.vpos.min)
			inc = list->scroll.vpos.pos - list->scroll.vpos.min;
		if (inc > 0) {
			list->scroll.vpos.pos -= inc;
			gui_table_mousemove (c, x, y-inc, 0);
			c->redraw = 1;
		}
		return 1;
	}	
	if (pressed > 0 && b == 4)
	{
		int inc = list->lineheight * 3;
		if (list->scroll.vpos.pos + inc > list->scroll.vpos.max)
			inc = list->scroll.vpos.max - list->scroll.vpos.pos;
		if (inc > 0) {
			list->scroll.vpos.pos += inc;
			gui_table_mousemove (c, x, y+inc, 0);
			c->redraw = 1;
		}
		return 1;
	}

	/* Click on preview mode */
	if (list->pc)
	{
		if (pressed && list->highlight >= 0)
		{
			if ((list->options & LIST_MULTISELECT))
			{
				gui_table_select (c, list->highlight);
				if (gr_key(KEY_LCTRL))
					gui_table_lineselected (c, list->highlight, 2);
				else
				{
					gui_table_unselectall (c);
					gui_table_lineselected (c, list->highlight, 1);
				}
			}
			else
				gui_table_select (c, list->highlight);
			return 1;
		}
		return 0;
	}

	/* Release button on titles */
	if (!pressed && y >= 0 && y <= list->title && list->pressed >= 0 && list->drag_column < 0)
	{
		if (list->sorted_by == list->pressed)
			gui_table_sort (c, list->pressed, !list->ascendant);
		else
			gui_table_sort (c, list->pressed, 1);
		list->pressed = -1;
		list->drag = -1;
		return 1;
	}

	/* Click on titles */
	if (pressed && y >= 0 && y <= list->title)
	{
		if (list->options & (LIST_USER_RESIZABLE | LIST_USER_MOVEABLE))
		{
			for (i = 0 ; i <= list->cols ; i++)
			{
				/* Resizing a column */
				if (x >= cx - 2 && x <= cx + 2 && (list->options & LIST_USER_RESIZABLE))
				{
					list->drag_width = list->column[i-1].width;
					list->drag_x = x;
					list->drag = i-1;
					return 1;
				}
				/* Clicking/dragging a column */
				if (i < list->cols)
				{
					if (x >= cx + 2 && x < cx + list->column[i].width - 2 && (list->options & LIST_USER_MOVEABLE))
					{
						list->pressed = i;
						return 1;
					}
					cx += list->column[i].width;
				}
			}
		}
	}
	else if (pressed)
	{
		gui_table_mousemove (c, x, y, 1);
		if (b == 0 && (list->options & LIST_MULTISELECT))
		{
			if (gr_key(KEY_LSHIFT) && list->shiftline == -1)
				list->shiftline = list->line;
			gui_table_select (c, list->highlight);
			if (gr_key(KEY_LCTRL))
				gui_table_lineselected (c, list->highlight, 2);
			else if (gr_key(KEY_LSHIFT))
			{
				gui_table_unselectall(c);
				if (list->shiftline == -1)
					list->shiftline = list->highlight;
				for (i = list->shiftline ; i != list->highlight ; i < list->highlight ? i++ : i--)
					gui_table_lineselected (c, i, 1);
				gui_table_lineselected (c, i, 1);
			}
			else if (gui_table_lineselected (c, list->highlight, -1) == 0)
			{
				gui_table_unselectall(c);
				gui_table_lineselected (c, list->highlight, 1);
				list->shiftline = -1;
			}
			else
				list->shiftline = -1;
		}
		if (c->draggable == 2)
		{
			c->draggable = 1;
			return 0;
		}
	}
	else
	{
		if (b == 0 && (list->options & LIST_MULTISELECT))
		{
			if (!gr_key(KEY_LCTRL) && !gr_key(KEY_LSHIFT) && list->highlight >= 0)
			{
				gui_table_unselectall(c);
				gui_table_lineselected (c, list->highlight, 1);
			}
		}

		list->pressed = -1;
		list->drag = -1;
		list->drag_column = -1;
		gui_desktop_removepopup (c);
	}
	return 1;
}

/** The user moves the focus to the list control
 *
 *  @param c		Pointer to the control member (at offset 0 of the list)
 *  @return			1 if the control is capable of focusing
 */

int gui_table_enter (CONTROL * c)
{
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user presses a key
 *
 *  @param c			Pointer to the control member (at offset 0 of the control)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_table_key (CONTROL * c, int scancode, int character)
{
	TABLE * list = (TABLE *) c;
	int previous_line = list->line, i, n;

	if (scancode == (KEYMOD_CONTROL | KEY_A) && (list->options & LIST_MULTISELECT))
	{
		gui_table_selectall (c);
		return 1;
	}

	switch (scancode & ~(KEYMOD_CONTROL | KEYMOD_SHIFT))
	{
		case KEY_SPACE:
		case KEY_RETURN:
			if (list->options & LIST_MULTISELECT)
				gui_table_lineselected (c, list->line, 2);
			gui_desktop_removepopup(c);
			return 0;
		case KEY_UP:
			if (list->pc)
				gui_table_select (c, list->line - list->pc_cols);
			else
				gui_table_select (c, list->line-1);
			list->highlight = -1;
			break;
		case KEY_DOWN:
			if (list->pc)
				gui_table_select (c, list->line + list->pc_cols);
			else
				gui_table_select (c, list->line+1);
			list->highlight = -1;
			break;
		case KEY_RIGHT:
			if (list->pc)
				gui_table_select (c , list->line+1);
			list->highlight = -1;
			break;
		case KEY_LEFT:
			if (list->pc)
				gui_table_select (c, list->line-1);
			list->highlight = -1;
			break;
		case KEY_HOME:
			gui_table_select (c, 0);
			list->highlight = -1;
			break;
		case KEY_END:
			gui_table_select(c, list->lines-1);
			list->highlight = -1;
			break;
		case KEY_PAGEDOWN:
			gui_scrollable_scrollto (c, list->scroll.scrollx,
				list->scroll.scrolly + list->scroll.vpos.pagestep);
			gui_table_select (c, list->line + list->scroll.control.height / list->lineheight);
			list->highlight = -1;
			break;
		case KEY_PAGEUP:
			gui_scrollable_scrollto (c, list->scroll.scrollx,
				list->scroll.scrolly - list->scroll.vpos.pagestep);
			gui_table_select (c, list->line - list->scroll.control.height / list->lineheight);
			list->highlight = -1;
			break;
		default:
			c->redraw = 1;
			character = toupper(character);
			if (character >= ' ' && character <= 127 && list->cols > 0)
			{
				if (list->line < 0 || list->line >= list->lines)
					break;

				/* Find a suitable column */
				for (n = 0; n < list->cols ; n++)
				{
					if (list->column[n].type != LIST_STRINGS)
						continue;
					if (n >= list->cols)
						return 0;

					/* Search the next character ocurrence */
					i = list->line+1;
					if (i == list->lines)
						i = 0;
					while (i != list->line)
					{
						if (list->column[n].count > i && 
							toupper(string_get(list->column[n].data[i])[0]) == character)
							break;
						if (++i == list->lines)
						{
							i = 0;
							if (list->line == -1)
								i = -1;
						}
					}
					if (i != list->line)
						break;
				}
				list->line = i;
				break;
			}
			return 0;
	}

	if ((list->options & LIST_MULTISELECT) && list->line != previous_line && !(scancode & KEYMOD_CONTROL))
	{
		gui_table_unselectall (c);
		if (scancode & KEYMOD_SHIFT)
		{
			if (list->shiftline != -1)
				previous_line = list->shiftline;
			else
				list->shiftline = previous_line;
			while (previous_line != list->line)
			{
				gui_table_lineselected (c, previous_line, 1);
				previous_line += previous_line < list->line ? 1 : -1;
			}
		}
		else
			list->shiftline = -1;
		gui_table_lineselected (c, list->line, 1);
	}
	else
		list->shiftline = -1;

	gui_table_fillscroll(list);
	c->redraw = 1;
	return 1;
}

/** The mouse leaves a list, change its pressed state.
 *  This is a member function of the CONTROL:TABLE class
 *
 *  @param c		Pointer to the list.control member (at offset 0 of the list)
 * */

int gui_table_mouseleave (CONTROL *c)
{
	TABLE * list = (TABLE *)c ;

	list->hovered = -1;

	if (list->highlight != -1)
	{
		list->highlight = -1;
		c->redraw = 1;
	}
	return 1;
}

/** Response to a resize event
 *
 *	@param c		Pointer to the list
 *	@param width	New width of the control)
 **/

void gui_table_resized (CONTROL * c)
{
	TABLE * table = (TABLE *)c;
	
	if (table->pc)
	{
		int areawidth = c->width - 2;
		int cellwidth = table->pc_width ;
		int cellheight = table->pc_height ;
		if (table->lines / (areawidth / cellwidth) * cellheight > c->height)
			areawidth -= table->scroll.vscroll.control.width;
		areawidth = (areawidth / cellwidth) * cellwidth;
		if (table->scroll.areawidth != areawidth)
		{
			int pc_rows;
			table->pc_cols = areawidth / cellwidth ;
			if (table->pc_cols < 1)
				table->pc_cols = 1;
			pc_rows = table->lines / table->pc_cols;
			if (table->pc_cols * pc_rows < table->lines)
				pc_rows++;
			gui_scrollable_resizearea (c, areawidth, table->pc_height * pc_rows);
		}
	}
	else
	{
		gui_table_fillscroll((TABLE*)c);
	}

	c->redraw = 1;
}

/** Find a column in the list column, given its name
 *
 *	@param control		Pointer to the list control
 *	@param text			String with the column name
 *	@return				Index of the column found, or -1 if none
 */

int gui_table_findcolumn (CONTROL * control, const char * text)
{
	TABLE * list = (TABLE *)control;
	int i;

	for (i = 0 ; i < list->cols ; i++)
		if (strcasecmp (list->column[i].name, text) == 0)
			return i;
	return -1;
}

/** Add a column to a table control
 *
 *	@param control		Pointer to the object
 *	@param column		Column to be added
 *	@param index		Index of position for insertion
 */

void gui_table_addcolumn (CONTROL * control, LISTCOLUMN * data, int index)
{
	TABLE * list = (TABLE *)control;
	int i;

	if (list->lineheight < data->lineheight)
		list->lineheight = data->lineheight;

	list->column = (LISTCOLUMN *)realloc (list->column, sizeof(LISTCOLUMN)*(list->cols+1));
	if (index > list->cols) 
		index = list->cols;
	if (index < 0)
		index = 0;
	if (index < list->cols)
		memmove (&list->column[index+1], &list->column[index], 
				sizeof(LISTCOLUMN) * (list->cols - index));
	memcpy (&list->column[index], data, sizeof(LISTCOLUMN));
	list->column[index].order = list->cols;
	list->cols++;

	if (!data->data)
		data->allocated = 0;

	if (list->column[index].width == 0 && list->column[index].type == LIST_TEXTS)
	{
		for (i = 0 ; i < list->column[index].count ; i++)
		{
			int width = gr_text_width (0, (const char *)list->column[index].data[i]) + COLSEP;
			if (list->column[index].width < width)
				list->column[index].width = width;
		}
		list->column[index].width += 8;
	}

	if (list->column[index].width == 0 && list->column[index].type == LIST_STRINGS)
	{
		for (i = 0 ; i < list->column[index].count ; i++)
		{
			const char * text = string_get ((int)list->column[index].data[i]);
			int width = gr_text_width (0, text) + COLSEP;
			if (list->column[index].width < width)
				list->column[index].width = width;
		}
		list->column[index].width += 8;
	}

	if (list->column[index].name[0] != 0)
	{
		int title = gr_text_height (0, list->column[index].name) + 4;
		int width = gr_text_width (0, list->column[index].name) + COLSEP - 4;
		if (list->title < title)
			list->title = list->scroll.titley = title;
		if (list->column[index].width < width)
			list->column[index].width = width;
	}

	control->redraw = 1;
	gui_table_fillscroll(list);
}

/** Remove a column from a table control
 *
 *	@param control		Pointer to the object
 *	@param index		Index of column to delete
 */

void gui_table_removecolumn (CONTROL * control, int index)
{
	TABLE * list = (TABLE *)control;
	int i;

	if (index > list->cols-1) 
		return;
	if (index < 0)
		return;
	for (i = 0 ; i < list->cols ; i++)
		if (list->column[i].order > list->column[index].order)
			list->column[i].order--;
	if (index < list->cols-1)
		memmove (&list->column[index], &list->column[index+1], 
			sizeof(LISTCOLUMN) * (list->cols - index - 1));
	list->cols--;
	control->redraw = 1;
	gui_table_fillscroll(list);
}

/** Remove one or more lines from a table
 *
 *	@param control			Pointer to the control object
 *	@param first			Index of the first line to remove (0 = topmost)
 *	@param count			Number of lines to remove (1+)
 */

void gui_table_removelines (CONTROL * control, int first, int count)
{
	TABLE * table = (TABLE *)control;
	int i, j, max = 0;

	if (count == 0)
		count = table->lines;

	for (i = 0 ; i < table->cols ; i++)
	{
		if (table->column[i].count > first)
		{
			int n = count;
			if (n + first > table->column[i].count)
				n = table->column[i].count - first;
			if (table->column[i].type == LIST_STRINGS)
			{
				for (j = 0 ; j < n ; j++)
					string_discard (table->column[i].data[j]);
			}
			if (n + first < table->column[i].count)
				memmove (&table->column[i].data[first],
						 &table->column[i].data[first+n],
						 table->column[i].count - n - first);
			table->column[i].count -= n;
			if (max < n) max = n;
		}
	}
	table->lines -= max;


	/* Move the selection data */

	if (table->line >= first)
		table->line -= count;
	if (table->line < 0)
		table->line = -1;
	if (table->linesel && table->linesel_alloc >= first+count)
	{
		memmove (table->linesel + first, table->linesel + first + count,
			table->linesel_alloc - (first+count));
		memset (table->linesel + first + count, 0,
			table->linesel_alloc - (first+count));
	}

	control->redraw = 1;
	gui_table_fillscroll (table);
}

/** Moves some lines inside a table
 *
 *	@param control		Pointer to the table control
 *	@param from			Index of the first original line
 *	@param to			Desired new index of the line
 *	@param count		Number of lines to move
 */

void gui_table_movelines (CONTROL * control, int from, int to, int count)
{
	TABLE * table = (TABLE *) control;
	int i;
	int * data;

	/* Adjust and check the parameters */

	if (from < 0)
		from = 0;
	if (to < 0)
		to = 0;
	if (from >= table->lines)
		from = table->lines-1;
	if (from+count > table->lines)
		count = table->lines-from;
	if (to + count > table->lines)
		to = table->lines-count;
	if (count < 1 || from == to)
		return;
	if (to >= from && to < from+count)
		return;

	/* Move the column data */

	data = (int *)malloc(4 * count);

	for (i = 0 ; i < table->cols ; i++)
	{
		/* Add empty lines to the column if needed */
		if (table->column[i].count < table->lines)
		{
			if (table->column[i].allocated < table->lines)
			{
				table->column[i].data = realloc (table->column[i].data, 4 * table->lines);
				table->column[i].allocated = table->lines;
			}
			memset (&table->column[i].data[table->column[i].count], 0,
				4 * (table->lines - table->column[i].count));
			table->column[i].count = table->lines;
		}

		/* Move the lines */
		if (to > from)
		{
			memmove (data, &table->column[i].data[from], 4 * count);
			memmove (&table->column[i].data[from], &table->column[i].data[from+count], 4 * (to - from));
			memmove (&table->column[i].data[to], data, 4 * count);
		}
		else
		{
			memmove (data, &table->column[i].data[from], 4 * count);
			memmove (&table->column[i].data[from+count-(from-to)], &table->column[i].data[to], 4 * (from - to));
			memmove (&table->column[i].data[to], data, 4 * count);
		}
	}
	control->redraw = 1;

	free(data);
}

/** Add a line to a table
 *
 *	@param control			Pointer to the table
 *	@param count			Number of columns
 *	@param params			Pointer to an array of values
 */

void gui_table_addline (CONTROL * control, int count, int * params)
{
	TABLE * list = (TABLE *)control;
	int i, v;

	if (list->options & LIST_MULTISELECT)
	{
		if (list->linesel && list->linesel_alloc > list->lines)
			list->linesel[list->lines] = 0;
	}

	list->lines++;

	for (i = 0 ; i < list->cols ; i++)
	{
		v = list->column[i].order;

		if (list->column[i].allocated < list->lines)
		{
			list->column[i].allocated = ((list->lines + 32) & ~15);
			list->column[i].data = (int *)realloc (list->column[i].data,
				4 * list->column[i].allocated);
		}
		
		if (i < count)
		{
			if (list->column[i].type == LIST_STRINGS)
			{
				int width = gr_text_width (0, string_get(params[v])) + 16;
				if (list->column[i].width < width)
					list->column[i].width = width;
				list->column[i].data[list->column[i].count++] = params[v];
				string_use (params[v]);
			}
			else
			{
				list->column[i].data[list->column[i].count++] = 
					atoi(string_get(params[v]));
			}
		}
	}

	list->sorted_by = -1;
	control->redraw = 1;
	gui_table_fillscroll (list);
}

static TABLE * sort_table;
static int sort_column;

static int sort_compare_strings_D (const void * a, const void * b)
{
	const char * txt_a = string_get (sort_table->column[sort_column].data[*(int*)a]);
	const char * txt_b = string_get (sort_table->column[sort_column].data[*(int*)b]);
	if (isdigit(*txt_a) && isdigit(*txt_b))
		return atoi(txt_b) - atoi(txt_a);
	return -strcasecmp(txt_a, txt_b);
}
static int sort_compare_strings_A (const void * a, const void * b)
{
	const char * txt_a = string_get (sort_table->column[sort_column].data[*(int*)a]);
	const char * txt_b = string_get (sort_table->column[sort_column].data[*(int*)b]);
	if (isdigit(*txt_a) && isdigit(*txt_b))
		return atoi(txt_a) - atoi(txt_b);
	return strcasecmp(txt_a, txt_b);
}

/** Sort a table's contents
 *
 *	@param control		Pointer to the table control
 *	@param column		Index of the column to sort
 *	@param ascendant	1 = ascendant order; 0 = descendant
 */

void gui_table_sort (CONTROL * control, int column, int ascendant)
{
	TABLE * table = (TABLE *) control;
	int * indexes;
	int i, j;

	sort_table = table;
	sort_column = column;
	if (column < 0 || column >= table->cols)
		return;

	indexes = (int *) malloc (4 * table->lines + 8);

	/* Sort strings */
	if (table->column[column].type == LIST_STRINGS)
	{
		for (i = 0 ; i < table->lines ; i++)
			indexes[i] = i;
		qsort (indexes, table->lines, 4, 
			ascendant ? sort_compare_strings_A : sort_compare_strings_D);
		for (j = 0 ; j < table->cols ; j++)
		{
			int * data = (int *)malloc(table->column[j].allocated * 4);
			for (i = 0 ; i < table->lines ; i++)
				data[i] = table->column[j].data[indexes[i]];
			free (table->column[j].data);
			table->column[j].data = data;
		}
	}

	table->sorted_by = column;
	table->ascendant = ascendant;
	control->redraw = 1;
	free (indexes);
}

/** Activate preview mode in a table control. The preview mode shows a big cell for each table line,
 *  using the data of a graphic column to decorate its contents. An optional title is shown below each cell.
 *
 *	@param control		Pointer to the table control
 *	@param width		Cell width in pixels
 *	@param height		Cell height in pixels
 *	@param title		Index of title column or -1 if none
 *	@param graph		Index of graph column or -1 if none
 */

void gui_table_enablepreview (CONTROL * control, int width, int height, int title, int graph)
{
	TABLE * table = (TABLE *)control;

	if (graph >= table->cols)
		graph = -1;
	if (title >= table->cols)
		title = -1;
	if (graph >= 0 && table->column[graph].type != LIST_BITMAPS)
		graph = -1;
	if (title >= 0 && table->column[title].type != LIST_STRINGS)
		title = -1;

	table->pc_width = width;
	table->pc_height = height;
	table->pc_cols = table->scroll.areawidth / table->pc_width;
	table->pc_title = title;
	table->pc_graph = graph;
	table->scroll.titley = 0;
	table->pc = 1;
	table->scroll.draw = gui_table_draw_preview;
	control->redraw = 1;
	gui_table_resized (control);
}

/** Disable preview mode in a table control
 *
 *	@param control		Pointer to the table control
 */

void gui_table_disablepreview (CONTROL * control)
{
	TABLE * table = (TABLE *)control;
	table->pc = 0;
	table->scroll.titley = table->title;
	table->scroll.draw = gui_table_draw;
	control->redraw = 1;
	gui_table_resized(control);
}

/** Sets of resets the line-selection flag in a multiselect table
 *
 *	@param control		Pointer to the table control
 *	@param line			Line number
 *	@param flags		New selection flag (2 invert, 1 selected, 0 unselected, -1 don't change)
 *	@return				Previous (or new) selection flag
 */

int gui_table_lineselected (CONTROL * control, int line, int flag)
{
	TABLE * table = (TABLE *) control;
	if (line >= table->lines)
		return -1;
	if (line < 0)
		return -1;
	if (table->linesel_alloc <= line)
	{
		table->linesel = (unsigned char *) realloc (table->linesel, line+1);
		while (table->linesel_alloc <= line)
			table->linesel[table->linesel_alloc++] = 0;
	}
	if (flag == 2)
		table->linesel[line] = !table->linesel[line];
	else if (flag >= 0)
		table->linesel[line] = flag;
	control->actionflags |= ACTION_CHANGE;
	control->redraw = 1;
	return table->linesel[line];
}

/** Sets the current selected line in a table
 *
 *	@param control			Pointer to the table control
 *	@param line				New selected line
 */

void gui_table_select (CONTROL * control, int line)
{
	TABLE * table = (TABLE *)control;
	if (table->lines < 1)
		return;
	if (line >= table->lines)
		line = table->lines-1;
	if (line >= -1)
		table->line = line;
	control->redraw = 1;
	control->actionflags |= ACTION_CHANGE;
}

/** De-selects all lines in a multiple-selection table
 *
 *	@param control			Pointer to the table control
 */

void gui_table_unselectall (CONTROL * control)
{
	TABLE * table = (TABLE *)control;
	if (table->linesel_alloc)
		memset (table->linesel, 0, table->linesel_alloc);
	control->redraw = 1;
	control->actionflags |= ACTION_CHANGE;
}

/** Selects all lines in a multiple-selection table
 *
 *	@param control			Pointer to the table control
 */

void gui_table_selectall (CONTROL * control)
{
	TABLE * table = (TABLE *)control;
	if (table->linesel_alloc < table->lines)
		table->linesel = (unsigned char *) realloc (table->linesel, table->linesel_alloc = table->lines);
	memset (table->linesel, 1, table->linesel_alloc);
	control->redraw = 1;
	control->actionflags |= ACTION_CHANGE;
}

/** Destroy a table (free allocated memory)
 *
 *	@param control		Pointer to the table control
 */

void gui_table_destructor (CONTROL * control)
{
	TABLE * list = (TABLE *)control;

	if (list->column)
	{
		int i, j;

		for (i = 0 ; i < list->cols ; i++)
		{
			if (list->column[i].type == LIST_STRINGS)
			{
				for (j = 0 ; j < list->column[i].count ; j++)
					string_discard (list->column[i].data[j]);
			}
			if (list->column[i].data)
				free (list->column[i].data);
		}
		free (list->column);
	}

	if (list->linesel)
		free (list->linesel);

	gui_control_destructor (control);
}

/** Create a new list control  with more than one column
 *
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param width	Width of the control
 *  @param height	Height of the control
 *  @param data		Pointer to the columns data
 *	@param cols		Number of columns
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_table_new (int width, int height)
{
	TABLE * list ;

	/* Alloc memory for the struct */
	list = (TABLE *) malloc(sizeof(TABLE));
	*((int *)list + sizeof(TABLE)/4 - 1) = 9999;
	if (list == NULL)
		return NULL;
	gui_scrollable_init (&list->scroll, width, height);
	gui_scrollable_maxsize (&list->scroll.control, 0, -1);
	list->scroll.control.hresizable = 1;
	list->scroll.control.vresizable = 1;
	list->scroll.control.innerdrag = 1;

	/* Fill the control control struct data members */
	list->scroll.control.bytes = sizeof(TABLE);

	/* Fill the control control struct member functions */
	list->scroll.draw = gui_table_draw;
	list->scroll.mousemove = gui_table_mousemove;
	list->scroll.mousebutton = gui_table_mousebutton;
	list->scroll.mouseleave = gui_table_mouseleave;
	list->scroll.enter = gui_table_enter;
	list->scroll.key = gui_table_key;
	list->scroll.resized = gui_table_resized;
	list->scroll.mousepointer = gui_table_mousepointer;
	list->scroll.control.destructor = gui_table_destructor;

	/* Fill the other data members */
	list->lineheight = 11;
	list->line = 0;
	list->highlight = -1;
	list->lines = 0;
	list->cols = 0;
	list->column = 0;
	list->title = 0;
	list->options = LIST_USER_RESIZABLE + LIST_USER_MOVEABLE + LIST_USER_SORTABLE;
	list->drag = -1;
	list->drag_column = -1;
	list->hovered = -1;
	list->pressed = -1;
	list->sorted_by = -1;
	list->pc = 0;
	list->pc_spacing = 2;
	list->pc_padding = 2;
	list->linesel = NULL;
	list->linesel_alloc = 0;
	list->shiftline = -1;
	list->alphahighlight = 0;

	gui_table_fillscroll(list);

	return &list->scroll.control;
}

/** Initialize all the data members of a LISTCOLUMN array
 */

LISTCOLUMN gui_table_column()
{
	LISTCOLUMN col;

	col.align = 0;
	col.allocated = 0;
	col.count = 0;
	col.data = 0;
	col.library = 0;
	col.lineheight = 0;
	col.name[0] = 0;
	col.type = LIST_STRINGS;
	col.width = 0;
	col.autoexpand = 0;
	col.minwidth = 8;
	return col;
}

