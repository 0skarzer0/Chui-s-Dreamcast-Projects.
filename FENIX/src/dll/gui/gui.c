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
 * FILE        : gui.c
 * DESCRIPTION : Graphic User Interface DLL
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#define MULTIPLE_FILES
#define MAIN_FILE

#include <fxdll.h>
#include <dcb.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

#include "gui.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define WINDOW_ALPHA 1 
#define WINDOW_STYLE 3
#define WINDOW_MOVEABLE 4
#define WINDOW_ONTOP 5
#define WINDOW_MODAL 6
#define WINDOW_WIDTH 7
#define WINDOW_HEIGHT 8
#define WINDOW_CAPTION 9
#define WINDOW_DONTFOCUS 10
#define WINDOW_FULLSCREEN 11
#define WINDOW_MAXIMIZED 12

static int lastmx = -1;
static int lastmy = -1;
static int lastmleft = -1;
static int lastmright = -1;
static int lastmcenter = -1;
static int lastmwup = -1;
static int lastmwdown = -1;
static int lastkey = -1;

int gui_full_redraw = 0;

/** Returns the desktop bounding-box and update flag.
 */

int info_desktop (void * user, REGION * bbox)
{
	WINDOW * window;

	gui_desktop_markpopups();

	if (gui_full_redraw)
	{
		bbox->x = 0;
		bbox->y = 0;
		bbox->x2 = scr_width-1;
		bbox->y2 = scr_height-1;
		gui_full_redraw = 0;
		return 1;
	}

	/* Mark for redraw any updated controls */

	for (window = gui_desktop_firstwindow() ; window ; window = gui_desktop_nextwindow(window))
	{
		int i;

		if (window->redraw)
		{
			window->redraw = 0;
			gr_mark_rect (window->x, window->y, window->width, window->height + window->caption);
			continue;
		}

		for (i = 0 ; i < window->control_count ; i++)
		{
			CONTROL * control = window->control[i] ; 

			if (control->redraw)
			{
				REGION bbox;

				bbox.x = control->x + window->x;
				bbox.y = control->y + window->y + window->caption;
				control->redraw = 0;
				gr_mark_rect (bbox.x, bbox.y, control->width, control->height);
			}
		}
	}

	/* Return the mouse cursor rect bounding box */

	if (lastmx != GLODWORD(MOUSEX) || lastmy != GLODWORD(MOUSEY)
	 || lastmleft != GLODWORD(MOUSELEFT)
	 || lastmright != GLODWORD(MOUSERIGHT)
	 || lastmcenter != GLODWORD(MOUSEMIDDLE))
	{
		int minx, maxx;
		int miny, maxy;

		if (lastmx < GLODWORD(MOUSEX))
			minx = lastmx, maxx = GLODWORD(MOUSEX);
		else
			maxx = lastmx, minx = GLODWORD(MOUSEX);

		if (lastmy < GLODWORD(MOUSEY))
			miny = lastmy, maxy = GLODWORD(MOUSEY);
		else
			maxy = lastmy, miny = GLODWORD(MOUSEY);

		gr_mark_rect (minx-32, miny-32, maxx-minx+64, maxy-miny+64);
	}

	bbox->x = 0;
	bbox->y = 0;
	bbox->x2 = scr_width-1;
	bbox->y2 = scr_height-1;
	return 0;
}

/** Draws the desktop each frame
 */

void draw_desktop (void * user, REGION * clip)
{
	static Uint32 last_frame_count = -1;
	int bitmapcursor;

	gui_desktop_draw(scrbitmap, clip);

	if (!cursors[0])
		gui_create_cursors();

	bitmapcursor = gui_desktop_mousepointer();
	if (bitmapcursor >= 0)
	{
		if (bitmapcursor != POINTER_IBEAM && bitmapcursor != POINTER_CROSS && cursors_black[bitmapcursor])
			gr_blit (scrbitmap, NULL, GLODWORD(MOUSEX)+2, GLODWORD(MOUSEY)+2, 
				B_ALPHA | (48 << B_ALPHA_SHIFT), cursors_black[bitmapcursor]);
		gr_blit (scrbitmap, NULL, GLODWORD(MOUSEX), GLODWORD(MOUSEY), 0, cursors[bitmapcursor]);
	}

	/* Propagate events */

	if (frame_count != last_frame_count)
	{
		last_frame_count = frame_count;
		if (lastmx != GLODWORD(MOUSEX) || lastmy != GLODWORD(MOUSEY))
		{
			lastmx = GLODWORD(MOUSEX);
			lastmy = GLODWORD(MOUSEY);
			gui_desktop_mousemove (lastmx, lastmy);
		}

		if (lastmleft != GLODWORD(MOUSELEFT)) {
			lastmleft = GLODWORD(MOUSELEFT) ;
			gui_desktop_mousebutton (lastmx, lastmy, 0, lastmleft) ;
		}
		if (lastmright != GLODWORD(MOUSERIGHT)) {
			lastmright = GLODWORD(MOUSERIGHT) ;
			gui_desktop_mousebutton (lastmx, lastmy, 1, lastmright) ;
		}
		if (lastmcenter != GLODWORD(MOUSEMIDDLE)) {
			lastmcenter = GLODWORD(MOUSEMIDDLE) ;
			gui_desktop_mousebutton (lastmx, lastmy, 2, lastmcenter) ;
		}
		if (lastmwup != GLODWORD(MOUSEWHEELUP)) {
			lastmwup = GLODWORD(MOUSEWHEELUP) ;
			gui_desktop_mousebutton (lastmx, lastmy, 3, lastmwup) ;
		}
		if (lastmwdown != GLODWORD(MOUSEWHEELDOWN)) {
			lastmwdown = GLODWORD(MOUSEWHEELDOWN) ;
			gui_desktop_mousebutton (lastmx, lastmy, 4, lastmwdown) ;
		}

		if (lastkey != GLODWORD(SCANCODE))
		{
			lastkey = GLODWORD(SCANCODE);
			if (lastkey != 0)
			{
				int code = lastkey;

				if (gr_key(KEY_RALT) || gr_key(KEY_LALT))
					code = KEYMOD_ALT + code;
				if (gr_key(KEY_RCTRL) || gr_key(KEY_LCTRL))
					code = KEYMOD_CONTROL + code;
				if (gr_key(KEY_RSHIFT) || gr_key(KEY_LSHIFT))
					code = KEYMOD_SHIFT + code;

				gui_desktop_key (code, GLODWORD(ASCII));
			}
		}
	}

	return;
}

// ----------------------------------------------------------------------------
//  FENIX FUNCTIONS
// ----------------------------------------------------------------------------

/** Number of the desktop object identifier,
 *  as returned by gr_new_object */
static int desktop_object = -1;

/** MAP_SETDEPTH (LIBRARY, GRAPH, DEPTH)
  * Changes the bit depth of a graphic (to 8 or 16) */
static int fxi_map_setdepth (INSTANCE * my, int * params)
{
	GRAPH * graph = bitmap_get (params[0], params[1]);
	bitmap_setdepth (graph, params[2]);
	return 0;
}

/** MAP_SLICE (LIBRARY, GRAPH)
  * Cuts a map to its optimal size (this function is here only until EFFECTS.DLL exists) */
static int fxi_map_slice (INSTANCE * my, int * params)
{
	GRAPH * graph = bitmap_get (params[0], params[1]);
	REGION  bbox;

	if (!graph || (!params[0] && !params[1]))
		return 0;
	bitmap_getoptimalbbox (graph, &bbox);
	bitmap_slice (graph, bbox.x, bbox.y, bbox.x2-bbox.x+1, bbox.y2-bbox.y+1);
	return 1;
}

/** GUI_SHOW(Z)
 *  Show the GUI desktop and controls, using Z as the drawing depth */
static int gui_show (INSTANCE * my, int * params)
{
	/* Default color theme */
	color_border		= gr_rgb (  0,   0,   0);
	color_text			= gr_rgb (  0,   0,   0);
	color_shadow		= gr_rgb (150, 150, 150);
	color_tablebg		= gr_rgb (196, 196, 196);
	color_selection		= gr_rgb (192, 192, 255);
	color_window		= gr_rgb (200, 200, 200);
	color_face			= gr_rgb (210, 210, 210);
	color_input			= gr_rgb (225, 225, 225);
	color_highlightface = gr_rgb (235, 235, 235);
	color_highlight		= gr_rgb (255, 255, 255);
	color_captionfg		= gr_rgb (166, 202, 240);
	color_captionbg		= gr_rgb (202, 202, 202);

	/* Alternative color theme */
	color_border		= gr_rgb (  0,   0,   0);
	color_text			= gr_rgb (255, 255, 255);
	color_shadow		= gr_rgb ( 41,  56,  74);
	color_tablebg		= gr_rgb ( 55,  79, 106);
	color_selection		= gr_rgb ( 65,  89, 180);
	color_window		= gr_rgb ( 85, 100, 120);
	color_face			= gr_rgb ( 65,  89, 106);
	color_input			= gr_rgb ( 85, 119, 146);
	color_highlightface = gr_rgb ( 90, 124, 156);
	color_highlight		= gr_rgb ( 95, 129, 162);
	color_captionfg		= gr_rgb (104, 130, 160);
	color_captionbg		= gr_rgb ( 85, 100, 120);

	gui_desktop_adjustdock();

	if (desktop_object != -1)
	{
		gr_destroy_object (desktop_object);
		desktop_object = -1;
	}
	desktop_object = gr_new_object (params[0], info_desktop, draw_desktop, NULL);
	return desktop_object;
}

/** GUI_HIDE()
 *  Hide the GUI desktop and controls, if visible */
static int gui_hide (INSTANCE * my, int * params)
{
	if (desktop_object != -1)
	{
		gr_destroy_object (desktop_object);
		desktop_object = -1;
	}
	return 0;
}

/* ---- Label class interface ---- */

static int label_new (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_label_new(text);
	string_discard(params[0]);
	return (int)control;
}

static int label_new_2 (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_label_new(text);
	gui_label_alignment (control, params[1]);
	string_discard(params[0]);
	return (int)control;
}

static int label_new_3 (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_label_new(text);
	gui_label_alignment (control, params[1]);
	control->hresizable = 1;
	control->min_width = control->width;
	if (params[2] > control->min_width)
		control->width = params[2];
 	string_discard(params[0]);
	return (int)control;
}

/** LABEL_TEXT (LABEL, "TEXT")
 *  Changes the text of a label control
 */

static int label_text (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	gui_label_text (control, string_get(params[1]));
	string_discard(params[1]);
	control->redraw = 1;
	return 0;
}

/* ---- Button class interface ---- */

static int button_new (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_button_new (text);
	string_discard(params[0]);
	return (int)control;
}

static void button_callback (int type)
{
	INSTANCE * proc = instance_new (procdef_get(type), first_instance);
	instance_go (proc);
}

static int button_new_2 (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_button_new (text);
	gui_button_callbacki (control, button_callback, params[1]);
	string_discard(params[0]);
	return (int)control;
}

/** BUTTON_PRESSED (BUTTON)
 *	Returns 1 if the button was pressed since the last call
 */

static int button_pressed (INSTANCE * my, int * params)
{
	BUTTON * button = (BUTTON *)params[0];
	int result = button->was_pressed;
	button->was_pressed = 0;
	return result;
}

/* ---- Radio class interface ---- */

static int radio_new (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_radio_new (text, (int *)params[1], params[2]);
	string_discard(params[0]);
	return (int)control;
}

/* ---- Checkbox class interface ---- */

static int checkbox_new (INSTANCE * my, int * params)
{
	const char * text = string_get(params[0]);
	CONTROL * control = gui_checkbox_new (text, (int *)params[1]);
	string_discard(params[0]);
	return (int)control;
}

/* ---- Frame class interface ---- */

static int frame_new (INSTANCE * my, int * params)
{
	return (int) gui_frame_new(params[0], params[1], params[2]);
}

/* ---- Scrollbar class interface ---- */

static int hscrollbar_new (INSTANCE * my, int * params)
{
	return (int) gui_hscrollbar_new (params[0], (SCROLLPOS *) params[1]);
}

static int hscrollbar_new_2 (INSTANCE * my, int * params)
{
	CONTROL * bar = gui_hscrollbar_new (params[0], (SCROLLPOS *) params[1]);
	gui_scrollbar_style (bar, params[2]);
	return (int) bar;
}

static int vscrollbar_new (INSTANCE * my, int * params)
{
	return (int) gui_vscrollbar_new (params[0], (SCROLLPOS *) params[1]);
}

static int vscrollbar_new_2(INSTANCE * my, int * params)
{
	CONTROL * bar = gui_vscrollbar_new (params[0], (SCROLLPOS *) params[1]);
	gui_scrollbar_style (bar, params[2]);
	return (int) bar;
}

/* ---- Inputline class interface ---- */

static int inputline_new (INSTANCE * my, int * params)
{
	DCB_TYPEDEF * var = (DCB_TYPEDEF *)params[2];

	if (var->BaseType[0] != TYPE_ARRAY || var->BaseType[1] != TYPE_CHAR)
		gr_error ("No es especificó una variable char[] a inputline_new");
	return (int)gui_inputline_new (params[0], (char *)params[1], var->Count[0]);
}

/* ---- Container class interface ---- */

static int container_new (INSTANCE * my, int * params)
{
	return (int)gui_container_new (params[0], params[1]);
}

static int container_add (INSTANCE * my, int * params)
{
	gui_container_add ((CONTROL *) params[0], params[1], params[2], (CONTROL *)params[3]);
	return 0;
}
static int container_move (INSTANCE * my, int * params)
{
	gui_container_move ((CONTROL *) params[0], params[1], params[2], (CONTROL *)params[3]);
	return 0;
}

static int container_remove (INSTANCE * my, int * params)
{
	gui_container_remove ((CONTROL *) params[0], (CONTROL *)params[1]);
	return 0;
}

/* ---- Palette Panel interface ---- */

static int palettepanel_new (INSTANCE * my, int * params)
{
	return (int) gui_palettepanel_news (params[0], params[1], NULL);
}

static int palettepanel_new_2 (INSTANCE * my, int * params)
{
	return (int) gui_palettepanel_news (params[0], params[1], (int *)params[2]);
}

/** PALETTE_PANEL_SET (PANEL, &VAR)
 *  Changes the color variable refered by a palette panel
 */

static int palettepanel_set (INSTANCE * my, int * params)
{
	((PALETTEPANEL *)params[0])->selected = (int *)params[1];
	return 0;
}

/** PALETTE_PANEL_SELECTION (PANEL, &ARRAY)
 *  Sets the palette panel selection array (256 BYTEs)
 */

static int palettepanel_selection (INSTANCE * my, int * params)
{
	gui_palettepanel_selection ((CONTROL*)params[0], (char*)params[1]);
	return 0;
}

/* ---- Image View class interface ---- */

static int imageview_new (INSTANCE * my, int * params)
{
	return (int)gui_imageview_new_fpg (params[0], params[1]);
}

static int imageview_new_2 (INSTANCE * my, int * params)
{
	CONTROL * c = gui_imageview_new_fpg (params[0], params[1]);
	gui_imageview_scale (c, 1, 1, 1);
	return (int)c;
}

static int imageview_new_3 (INSTANCE * my, int * params)
{
	CONTROL * c = gui_imageview_new_fpg (params[0], params[1]);
	gui_imageview_scale (c, 1, 1, 1);
	if (params[2] != 0)
	{
		gui_imageview_scale (c, 1, 0, 0);
		gui_imageview_zoom (c, abs(params[2]) / 100.0f);
	}
	return (int)c;
}

static int imageview_new_4 (INSTANCE * my, int * params)
{
	CONTROL * c = gui_imageview_new_fpg (params[0], params[1]);
	gui_imageview_scale (c, 1, 1, params[2] >= 0);
	if (params[2] != 0)
	{
		if (params[2] > 0)
			gui_imageview_scale (c, 1, 0, 0);
		gui_imageview_zoom (c, abs(params[2]) / 100.0f);
	}
	gui_imageview_background (c, params[3]);
	return (int)c;
}

static int imageview_setimage (INSTANCE * my, int * params)
{
	gui_imageview_image_fpg ((CONTROL *)params[0], params[1], params[2]);
	return 0;
}

/** IMAGE_VIEW_ZOOM (IMAGE, ZOOM, AUTOSCALE, MANTAIN_ASPECT)
 *  Changes the zoom level and options of an image viewer
 */

static int imageview_zoom (INSTANCE * my, int * params)
{
	gui_imageview_zoom  ((CONTROL *)params[0], params[1] / 100.0f);
	gui_imageview_scale ((CONTROL *)params[0], 1, params[2], params[3]);
	return 0;
}

/* ---- Image editor class interface ---- */

/** IMAGEEDITOR (FPG, GRAPH, &INFO, WIDTH, HEIGHT)
 *  Creates an image editor
 */

static int greditor_new (INSTANCE * my, int * params)
{
	IMAGEEDITORINFO * info = (IMAGEEDITORINFO *)params[2];
	info->file = params[0];
	info->graph = params[1];
	return (int)gui_imageeditor_new (bitmap_get (params[0], params[1]), info, params[3], params[4]);
}

/** GREDITOR_INFO (EDITOR)
 *  Returns a pointer to an editor's information struct
 */

static int greditor_info (INSTANCE * my, int * params)
{
	return (int) ((IMAGEEDITOR *)params[0])->info;
}

/** GREDITOR_ZOOMTO (EDITOR, X, Y, ZOOM)
 *  Changes the zoom level of an editor and centers the view around the given pixel
 */

static int greditor_zoomto (INSTANCE * my, int * params)
{
	gui_imageeditor_zoomtopoint ((IMAGEEDITOR *)params[0], params[1], params[2], params[3]);
	return 0;
}

/** GREDITOR_UNDOMEM (EDITOR)
 *  Returns the ammount of Undo memory used by the control
 */

static int greditor_undomem (INSTANCE * my, int * params)
{
	return ((IMAGEEDITOR *)params[0])->undo_memory;
}

/** GREDITOR_SHOWBARS (EDITOR, SHOW_BARS)
 *  Shows or hides the scroll bars of the editor (TRUE or FALSE)
 */

static int greditor_showbars (INSTANCE * my, int * params)
{
	((SCROLLABLE *)params[0])->showbars = params[1];
	return 0;
}

/** GREDITOR_ACTIVE (EDITOR)
 *	Returns the active status of an editor
 */

static int greditor_active (INSTANCE * my, int * params)
{
	return ((IMAGEEDITOR *)params[0])->active;
}

/** GREDITOR_ACTIVE (EDITOR, STATUS)
 *  Changes the active status of an editor
 */

static int greditor_active_2 (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	if (c->window)
		gui_window_releasemouse (c->window, c);
	((IMAGEEDITOR *)c)->active = params[1];
	return 0;
}

/** GRPALETTE (&INFO, FPG)
 *  Creates an image editor palette, using the given FPG file for tool/decoration graphics
 */

static int grpalette_new (INSTANCE * my, int * params)
{
	return (int) gui_editorpalette_new ((IMAGEEDITORINFO *)params[0], params[1]);
}

/** GRPALETTE_SET (CONTROL, &INFO)
 *  Changes the palette information pointer
 */

static int grpalette_set (INSTANCE * my, int * params)
{
	EDITORPALETTE * p = (EDITORPALETTE *)params[0];
	IMAGEEDITORINFO * i = (IMAGEEDITORINFO *)params[1];
	p->info = i;
	return 0;
}

/** COLOR_CHOOSER (&COLOR, OPTIONS)
 *  Creates a color chooser control
 */

static int color_chooser_new (INSTANCE * my, int * params)
{
	return (int) gui_colorchooser_newc ((int *)params[0], params[1]);
}

/** CLICKABLE_COLOR (&COLOR)
 *	Creates a clickable color 
 */

static int clickablecolor_new (INSTANCE * my, int * params)
{
	return (int) gui_clickablecolor_new ((int *)params[0], 0);
}

/** CLICKABLE_COLOR (&COLOR, OPTIONS)
 *	Creates a clickable color 
 */

static int clickablecolor_new_2 (INSTANCE * my, int * params)
{
	return (int) gui_clickablecolor_new ((int *)params[0], params[1]);
}

/* ---- List/table class interface ---- */

static int table_column (TABLE * list, int string)
{
	const char * text = string_get (string);
	int i;

	for (i = 0 ; i < list->cols ; i++)
	{
		if (strcasecmp (list->column[i].name, text) == 0)
			break;
	}
	if (i == list->cols)
		i = atoi(text);
	if (i < 0 || i >= list->cols)
		return -1;
	return i;
}

static int table_new (INSTANCE * my, int * params)
{
	return (int) gui_table_new (params[0], params[1]);
}

static int table_addcolumn_0 (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	const char * text = string_get (params[1]);
	LISTCOLUMN column = gui_table_column();

	strncpy (column.name, text, sizeof(column.name));
	column.type  = LIST_STRINGS;
	gui_table_addcolumn (control, &column, 65535);
	string_discard(params[1]);
	return 0;
}

static int table_addcolumn_b (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	const char * text = string_get (params[1]);
	LISTCOLUMN column = gui_table_column();

	strncpy (column.name, text, sizeof(column.name));
	column.type  = LIST_BITMAPS;
	gui_table_addcolumn (control, &column, 65535);
	string_discard(params[1]);
	return 0;
}

static int table_addcolumn (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	const char * text = string_get (params[1]);
	DCB_TYPEDEF * var = (DCB_TYPEDEF *)params[3];
	LISTCOLUMN column = gui_table_column();
	int i;

	strncpy (column.name, text, sizeof(column.name));

	if (var->BaseType[0] != TYPE_ARRAY)
		gr_error ("Se esperaba un Array");

	switch (var->BaseType[1])
	{
		case TYPE_STRING:
			column.allocated = ((var->Count[0] + 64) & ~31);
			column.data = malloc(4 * column.allocated);
			column.count = var->Count[0];
			column.type = LIST_STRINGS;
			memcpy (column.data, (void *)params[2], 4 * var->Count[0]);
			for (i = 0 ; i < (int)var->Count[0] ; i++)
				string_use (column.data[i]);
			break;

		case TYPE_DWORD:
		case TYPE_INT:
			column.allocated = ((var->Count[0] + 64) & ~31);
			column.data = malloc(4 * column.allocated);
			column.count = var->Count[0];
			column.type = LIST_BITMAPS;
			memcpy (column.data, (void *)params[2], 4 * var->Count[0]);
			break;

		default:
			gr_error ("No puede representarse el tipo de dato de la columna");
	}

	gui_table_addcolumn (control, &column, 65535);
	string_discard(params[1]);
	return 0;
}

static int table_addline_X (INSTANCE * my, int * params, int count)
{
	CONTROL * control = (CONTROL *)params[0];
	int i;

	params++;
	gui_table_addline (control, count, params);

	for (i = 0 ; i < count ; i++)
		string_discard (params[i]);
	return 0;
}

static int table_addline_1  (INSTANCE * my, int * params) { return table_addline_X(my,params,1); }
static int table_addline_2  (INSTANCE * my, int * params) { return table_addline_X(my,params,2); }
static int table_addline_3  (INSTANCE * my, int * params) { return table_addline_X(my,params,3); }
static int table_addline_4  (INSTANCE * my, int * params) { return table_addline_X(my,params,4); }
static int table_addline_5  (INSTANCE * my, int * params) { return table_addline_X(my,params,5); }
static int table_addline_6  (INSTANCE * my, int * params) { return table_addline_X(my,params,6); }
static int table_addline_7  (INSTANCE * my, int * params) { return table_addline_X(my,params,7); }
static int table_addline_8  (INSTANCE * my, int * params) { return table_addline_X(my,params,8); }
static int table_addline_9  (INSTANCE * my, int * params) { return table_addline_X(my,params,9); }
static int table_addline_10 (INSTANCE * my, int * params) { return table_addline_X(my,params,10); }
static int table_addline_11 (INSTANCE * my, int * params) { return table_addline_X(my,params,11); }
static int table_addline_12 (INSTANCE * my, int * params) { return table_addline_X(my,params,12); }
static int table_addline_13 (INSTANCE * my, int * params) { return table_addline_X(my,params,13); }
static int table_addline_14 (INSTANCE * my, int * params) { return table_addline_X(my,params,14); }
static int table_addline_15 (INSTANCE * my, int * params) { return table_addline_X(my,params,15); }

static int table_removelines (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	gui_table_removelines (control, params[1], params[2]);
	return 0;
}

static int table_movelines (INSTANCE * my, int * params)
{
	gui_table_movelines ((CONTROL *)params[0], params[1], params[2], params[3]);
	return 0;
}

/** TABLE_LINES (TABLE)
 *	Returns the number of lines in a table
 */

static int table_lines (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	return table->lines;
}

/** TABLE_SELECTEDCOUNT (TABLE)
 *	Returns the number of selected lines in a multi-selection table control
 */

static int table_selectedcount (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	int result, i;
	for (i = result = 0 ; i < table->lines ; i++)
		result += gui_table_lineselected ((CONTROL *)params[0], i, -1);
	return result;
}

/** TABLE_SELECTED (TABLE)
 *	Returns the current selected line of a table
 */

static int table_selected (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	return table->line;
}

/** TABLE_SELECTED (TABLE, LINE)
 *	Returns the status of a line selection in a multi-selection table control
 */

static int table_selected_2 (INSTANCE * my, int * params)
{
	return gui_table_lineselected ((CONTROL *)params[0], params[1], -1);
}

/** TABLE_SELECT (TABLE)
 *	Changes the current selected line in the table
 */

static int table_select (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	CONTROL * control = (CONTROL *)params[0];
	int af = control->actionflags;

	gui_table_select((CONTROL *)params[0], params[1]);
	if ((table->options & LIST_MULTISELECT) && params[1] >= 0)
	{
		gui_table_unselectall(control);
		gui_table_lineselected(control, params[1], 1);
	}
	control->actionflags = af;
	return table->line;
}

/** TABLE_SELECT (TABLE, LINE, FLAG)
 *	Changes the status of a line selection in a multi-selection table control
 */

static int table_select_2 (INSTANCE * my, int * params)
{
	return gui_table_lineselected ((CONTROL *)params[0], params[1], params[2]);
}

static int table_sort (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	TABLE * list = (TABLE *)params[0];
	int i = table_column (list, params[1]);
	if (i >= 0)
		gui_table_sort (control, i, params[2]);
	string_discard(params[1]);
	return 0;
}

#define TABLE_CAPTION				1
#define TABLE_RESIZABLE_COLUMNS		2
#define TABLE_MOVEABLE_COLUMNS		3
#define TABLE_SORTABLE_COLUMNS		4
#define TABLE_MULTISELECT			5

static int table_set (INSTANCE * my, int * params)
{
	TABLE * list = (TABLE *)params[0];
	switch (params[1])
	{
		case TABLE_CAPTION:
			list->title = params[2];
			list->scroll.titley = params[2];
			break;
		case TABLE_RESIZABLE_COLUMNS:
			if (params[2])
				list->options |= LIST_USER_RESIZABLE;
			else
				list->options &=~LIST_USER_RESIZABLE;
			break;
		case TABLE_MOVEABLE_COLUMNS:
			if (params[2])
				list->options |= LIST_USER_MOVEABLE;
			else
				list->options &=~LIST_USER_MOVEABLE;
			break;
		case TABLE_SORTABLE_COLUMNS:
			if (params[2])
				list->options |= LIST_USER_SORTABLE;
			else
				list->options &=~LIST_USER_SORTABLE;
			break;
		case TABLE_MULTISELECT:
			if (params[2])
				list->options |= LIST_MULTISELECT;
			else
				list->options &= ~LIST_MULTISELECT;
			break;
	}
	return 0;
}

static int table_removecolumn (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	TABLE * list = (TABLE *)params[0];
	int i = table_column(list, params[1]);
	if (i >= 0)
		gui_table_removecolumn (control, i);
	string_discard(params[1]);
	return 0;
}

static int table_library (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	TABLE * list = (TABLE *)params[0];
	int i = table_column (list, params[1]);
	if (i >= 0)
		list->column[i].library = params[2];
	string_discard(params[1]);
	return 0;
}

/** TABLE_PREVIEWMODE (TABLE, WIDTH, HEIGHT, TITLE_COLUMN, GRAPH_COLUMN)
 *	Enables preview mode in a table control
 */

static int table_enablepreview (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	TABLE   * table = (TABLE *)control;
	int i_title = -1;
	int i_graph = -1;
	const char * txt_title = string_get(params[3]);
	const char * txt_graph = string_get(params[4]);
	int i;

	for (i = 0 ; i < table->cols ; i++)
	{
		if (strcasecmp (table->column[i].name, txt_title) == 0)
			i_title = i;
		if (strcasecmp (table->column[i].name, txt_graph) == 0)
			i_graph = i;
	}

	gui_table_enablepreview (control, params[1], params[2], i_title, i_graph);
	string_discard (params[3]);
	string_discard (params[4]);
	return 0;
}

/** TABLE_NORMALMODE (TABLE)
 *	Disables preview mode in a table control
 */

static int table_disablepreview (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	gui_table_disablepreview (control);
	return 0;
}

/** TABLE_AUTOEXPAND (TABLE, STRING COLUMN_NAME)
 *	Activates automatic expansion of a column
 */

static int table_autoexpand (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	int i = table_column (table, params[1]);
	if (i >= 0 && i < table->cols)
		table->column[i].autoexpand = 1;
	string_discard(params[1]);
	gui_table_fillscroll (table);
	return 0;
}

/** TABLE_WIDTH (TABLE)
 *  Returns the sum of all columns width in a table
 */

static int table_width (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	int result = 0;
	int i;

	for (i = 0 ; i < table->cols ; i++)
		result += table->column[i].width;
	return result;
}

/** TABLE_WIDTH (TABLE, STRING NAME)
 *  Returns the sum of the given column
 */

static int table_width_2 (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	int result = 0;
	int i = table_column (table, params[1]);
	if (i >= 0 && i < table->cols)
		result = table->column[i].width;
	string_discard(params[1]);
	return result;
}

/** TABLE_WIDTH (TABLE, STRING COLUMN_NAME, NEW_WIDTH)
 *	Change the width of a table column
 */

static int table_width_3 (INSTANCE * my, int * params)
{
	TABLE * table = (TABLE *)params[0];
	int i = table_column (table, params[1]);
	if (i >= 0 && i < table->cols)
	{
		table->column[i].width = params[2];
		gui_table_fillscroll (table);
	}
	string_discard (params[1]);
	return 0;
}

/** STRING TABLE_GET (TABLE, ROW, STRING COLUMN_NAME)
 *  Returns the contents of some cell in a table
 */

static int table_get (INSTANCE * my, int * params)
{
	TABLE * list = (TABLE *)params[0];
	int i = table_column (list, params[2]);
	int result = 0;
	if (i >= 0 && params[1] >= 0 && params[1] < list->column[i].count) 
	{
		result = list->column[i].data[params[1]];
		if (list->column[i].type != LIST_STRINGS)
			result = string_itoa(result);
		string_use(result);
	}
	string_discard (params[2]);
	return result;
}

/** STRING TABLE_CHANGE (TABLE, ROW, STRING COLUMN_NAME, STRING VALUE)
 *  Change the contents of some cell in a table
 */

static int table_change (INSTANCE * my, int * params)
{
	TABLE * list = (TABLE *)params[0];
	int i = table_column (list, params[2]);
	if (i >= 0 && params[1] >= 0 && params[1] < list->column[i].count) 
	{
		if (list->column[i].type == LIST_STRINGS)
		{
			string_discard(list->column[i].data[params[1]]);
			list->column[i].data[params[1]] = params[3];
			string_use(params[3]);
		}
		else
		{
			list->column[i].data[params[1]] = atoi(string_get(params[3]));
		}
	}
	string_discard (params[2]);
	string_discard (params[3]);
	return 0;
}

/** TABLE_ALIGNMENT (TABLE, STRING COLUMN_NAME, ALIGNMENT)
 *  Changes the alignment (0, 1, 2) or a column
 */

static int table_alignment (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	TABLE * list = (TABLE *)params[0];
	int i = table_column (list, params[1]);
	if (i >= 0)
		list->column[i].align = params[2];
	string_discard(params[1]);
	return 0;
}

/* ---- Menu interface ---- */

typedef struct 
{
	int allocated;
	int count;
	MENUITEM * option;
}
FXI_MENU;

/** MENU()
 *  Creates a new menu (a list of options, not a control)
 */

static int menu_begin (INSTANCE * my, int * params)
{
	FXI_MENU * menu;

	menu = (FXI_MENU *) malloc(sizeof(FXI_MENU));
	if (menu)
	{
		menu->allocated = 0;
		menu->count = 0;
		menu->option = 0;
	}
	return (int) menu;
}

/** MENU_DESTROY (MENU)
 *  Frees a menu and all its contents. Does nothing to created menu controls.
 */

static int menu_destroy (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *)params[0];
	if (menu->option)
		free (menu->option);
	free (menu);
	return 0;
}

/** MENU_SEPARATOR (MENU)
 *  Adds a separator to a menu
 */

static int menu_separator (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];

	if (menu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count++] = gui_menuitem_separator();
	}
	return 0;
}

/** MENU_ACTION (MENU, STRING TEXT)
 *  Adds an action to a menu
 */

static int menu_action (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	if (menu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count++] = gui_menuitem(text);
	}
	string_discard(params[1]);
	return 0;
}

/** MENU_ACTION (MENU, STRING TEXT, TYPE)
 *  Adds an action to a menu. A process of the given type will be created on click.
 */

static int menu_action_2 (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	if (menu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count] = gui_menuitem(text);
		menu->option[menu->count].callback = button_callback;
		menu->option[menu->count].callbackparam[0] = params[2];
		menu->count++;
	}
	string_discard(params[1]);
	return 0;
}

/** MENU_TOGGLE (MENU, STRING TEXT, POINTER VARIABLE)
 *  Adds a toggle-option to a menu (checks TRUE/FALSE at the given variable)
 */

static int menu_toggle (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	if (menu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count++] = gui_menuitem_toggle(text, (int*)params[2]);
	}
	string_discard(params[1]);
	return 0;
}

/** MENU_RADIO (MENU, STRING TEXT, POINTER VARIABLE, VALUE)
 *  Adds a toggle-option, radio type, to a menu (checks for VALUE at the given variable)
 */

static int menu_radio (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	if (menu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count++] = gui_menuitem_radio(text, (int*)params[2], params[3]);
	}
	string_discard(params[1]);
	return 0;
}

/** MENU_SUBMENU (MENU, STRING TEXT, SUBMENU)
 *  Creates a sub-menú entry to a menu
 */

static int menu_submenu (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	FXI_MENU * submenu = (FXI_MENU *) params[2];
	if (menu && submenu)
	{
		if (menu->allocated == menu->count)
		{
			menu->allocated += 16;
			menu->option = (MENUITEM *)
				realloc (menu->option, sizeof(MENUITEM) * menu->allocated);
		}
		menu->option[menu->count++] = gui_menuitem_submenu(text, submenu->option, submenu->count);
	}
	string_discard(params[1]);
	return 0;
}

/** MENU_ENABLE (MENU, STRING TEXT)
 *  Enables an options. Use MENU_DISABLE to disable it.
 */

static int menu_enable (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	int i;

	for (i = 0 ; i < menu->count ; i++)
	{
		if (strcasecmp (menu->option[i].text, text) == 0)
			break;
	}
	if (i == menu->count)
		i = atoi(text);
	if (i >= 0 && i < menu->count)
		menu->option[i].enabled = 1;

	string_discard(params[1]);
	return 0;
}

/** MENU_DISABLE (MENU, STRING TEXT)
 *  Disables an option
 */

static int menu_disable (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	const char * text = string_get(params[1]);
	int i;

	for (i = 0 ; i < menu->count ; i++)
	{
		if (strcasecmp (menu->option[i].text, text) == 0)
			break;
	}
	if (i == menu->count)
		i = atoi(text);
	if (i >= 0 && i < menu->count)
		menu->option[i].enabled = 0;

	string_discard(params[1]);
	return 0;
}

/** POPUP_NEW (MENU)
 *  Creates a new pop-up control with the given menu
 */

static int popup_new (INSTANCE * my, int * params)
{
	FXI_MENU * menu = (FXI_MENU *) params[0];
	return (int) gui_menu_new (menu->option, menu->count);
}

/** TOOLBAR()
 *	Creates a new toolbar control
 */

static int toolbar_new (INSTANCE * my, int * params)
{
	return (int) gui_toolbar_new();
}

/** TASKBAR()
 *  Creates a new taskbar control
 */

static int taskbar_new (INSTANCE * my, int * params)
{
	return (int) gui_taskbar_new();
}

/** TOOLBAR_MENU (TOOLBAR, STRING TEXT, MENU)
 *  Adds a pull-down menu to the toolbar
 */

static int toolbar_menu (INSTANCE * my, int * params)
{
	const char * text = string_get(params[1]);
	FXI_MENU * menu = (FXI_MENU *) params[2];
	gui_toolbar_addmenu ((CONTROL *)params[0], text, menu->option, menu->count);
	string_discard (params[1]);
	return 0;
}

/** TOOLBAR_MENU (TOOLBAR, STRING TYPE, STRING TEXT, MENU)
 *  Adds a pull-down menu to the toolbar, only visible with the given type top window
 */

static int toolbar_menu_2 (INSTANCE * my, int * params)
{
	const char * type = string_get(params[1]);
	const char * text = string_get(params[2]);
	FXI_MENU * menu = (FXI_MENU *) params[3];
	gui_toolbar_addmenu ((CONTROL *)params[0], text, menu->option, menu->count);
	gui_toolbar_settype ((CONTROL *)params[0], -1, params[1]);
	string_discard (params[1]);
	string_discard (params[2]);
	return 0;
}

/** TOOLBAR_BUTTON (TOOLBAR, LIBRARY, FPG)
 *  Adds a toolbar button to the toolbar
 */

static int toolbar_button (INSTANCE * my, int * params)
{
	gui_toolbar_addbutton ((CONTROL *)params[0], bitmap_get(params[1], params[2]));
	return 0;
}

/** TOOLBAR_BUTTON (TOOLBAR, LIBRARY, FPG, STRING TOOLTIP_TEXT)
 *  Adds a toolbar button to the toolbar, including a tooltip text
 */

static int toolbar_button2 (INSTANCE * my, int * params)
{
	gui_toolbar_addbutton_tooltip ((CONTROL *)params[0],
		bitmap_get(params[1], params[2]), params[3]);
	string_use(params[3]);
	string_discard(params[3]);
	return 0;
}

/** TOOLBAR_BUTTON (TOOLBAR, LIBRARY, FPG, STRING TOOLTIP_TEXT, POINTER VAR)
 *  Adds a toolbar button to the given toolbar, including a tooltip text
 *  The toolbar button is a two-state one that checks the given variable for 1/0 values
 */

static int toolbar_button3 (INSTANCE * my, int * params)
{
	gui_toolbar_addbutton_check ((CONTROL *)params[0],
		bitmap_get(params[1], params[2]), params[3], (int *)params[4]);
	string_use(params[3]);
	string_discard(params[3]);
	return 0;
}

/** TOOLBAR_BUTTON (TOOLBAR, LIBRARY, FPG, STRING TOOLTIP_TEXT)
 *  Adds a toolbar button to the given toolbar, including a tooltip text
 *  The toolbar button is a two-state one that checks the given variable for the given value
 */

static int toolbar_button4 (INSTANCE * my, int * params)
{
	gui_toolbar_addbutton_radio ((CONTROL *)params[0],
		bitmap_get(params[1], params[2]), params[3], (int *)params[4], params[5]);
	string_use(params[3]);
	string_discard(params[3]);
	return 0;
}

/** TOOLBAR_CONTROL (TOOLBAR, CONTROL)
 *	Add a new user-defined control to the toolbar
 */

static int toolbar_control (INSTANCE * my, int * params)
{
	gui_toolbar_addcontrol ((CONTROL *)params[0], (CONTROL *)params[1]);
	return 0;
}

/** TOOLBAR_SEPARATORL (TOOLBAR)
 *	Add a separator to the toolbar
 */

static int toolbar_separator (INSTANCE * my, int * params)
{
	gui_toolbar_addseparator ((CONTROL *)params[0]);
	return 0;
}

/** TOOLBAR_PRESSED (TOOLBAR)
 *	Returns the number of the last button pressed, or -1 if none, and resets its status
 */

static int toolbar_pressed (INSTANCE * my, int * params)
{
	TOOLBAR * toolbar = (TOOLBAR *)params[0];
	int result = toolbar->pressed;
	toolbar->pressed = -1;
	return result;
}

/* ---- Drop-down class interface ---- */

static int dropdown_new (INSTANCE * my, int * params)
{
	int result = (int) gui_dropdown_new (params[0], (int *) params[1]);
	return result;
}

static int dropdown_new_2 (INSTANCE * my, int * params)
{
	CONTROL * control =  gui_dropdown_new (params[0], (int *) params[1]);
	DCB_TYPEDEF * var = (DCB_TYPEDEF *)params[3];

	if (var->BaseType[0] != TYPE_ARRAY || var->BaseType[1] != TYPE_STRING)
		gr_error ("DROPDOWN requiere un array de strings");
	else
		gui_dropdown_addoptions (control, (int *)params[2], var->Count[0]);
	return (int)control;
}

static int dropdown_addoption (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	gui_dropdown_addoption (control, params[1]);
	string_discard (params[1]);
	return 0;
}

/* ---- Window class interface ---- */

static int window_addcontrol (INSTANCE * my, int * params)
{
	gui_window_addcontrol ((WINDOW *) params[0], params[1], params[2], (CONTROL *)params[3]);
	return 0;
}

static int window_new (INSTANCE * my, int * params)
{
	return (int) gui_window_new();
}

static int window_new_2 (INSTANCE * my, int * params)
{
	WINDOW * w = gui_window_new();
	gui_window_style (w, params[0]);
	return (int) w;
}

static int window_new_3 (INSTANCE * my, int * params)
{
	WINDOW * w = gui_window_new();
	const char * text = string_get(params[0]);
	gui_window_caption (w, text);
	string_discard (params[0]);
	return (int) w;
}

static int window_new_4 (INSTANCE * my, int * params)
{
	WINDOW * w = gui_window_new();
	const char * text = string_get(params[0]);
	gui_window_caption (w, text);
	gui_window_style (w, params[1]);
	string_discard (params[0]);
	return (int) w;
}

static int window_destroy (INSTANCE * my, int * params)
{
	gui_desktop_removewindow ((WINDOW *)params[0]);
	gui_window_destroy ((WINDOW *) params[0]);
	return 0;
}

static int window_visible (INSTANCE * my, int * params)
{
	WINDOW * w = gui_desktop_firstwindow();

	while (w)
	{
		if (w == (WINDOW*)params[0])
			return 1;
		w = gui_desktop_nextwindow(w);
	}
	return 0;
}

static int window_move (INSTANCE * my, int * params)
{
	gui_window_moveto ((WINDOW *)params[0], params[1], params[2]);
	return 0;
}

static int window_resize (INSTANCE * my, int * params)
{
	gui_window_resize ((WINDOW *)params[0], params[1], params[2]);
	return 0;
}

static int window_set (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];

	switch (params[1])
	{
		case WINDOW_ALPHA:
			window->alpha = params[2];
			break;

		case WINDOW_ONTOP:
			window->stayontop = params[2];
			break;

		case WINDOW_STYLE:
			gui_window_style ((WINDOW *)params[0], params[2]);
			break;

		case WINDOW_MOVEABLE:
			if (!window->dock)
				window->moveable = params[2];
			break;

		case WINDOW_MODAL:
			window->modal = params[2];
			window->stayontop = params[2];
			break;

		case WINDOW_CAPTION:
			window->caption = params[2];
			break;

		case WINDOW_DONTFOCUS:
			window->dontfocus = params[2];
			break;

		case WINDOW_FULLSCREEN:
			window->fullscreen = params[2];
			break;

		case WINDOW_MAXIMIZED:
			if (window->maximized != params[2])
				gui_action_maximizewindow(window);
			break;
	}
	return 0;
}

/** WINDOW_CHANGED (WINDOW)
 *	Get the "changed/modified" flag of a window
 */

static int window_getchanged (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	return window->changed;
}

/** WINDOW_CHANGED (WINDOW, CHANGED )
 *	Sets the "changed/modified" window flag
 */

static int window_setchanged (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	return window->changed = params[1];
}

/** WINDOW_INFO (WINDOW, TYPE)
 *	Returns information about the given window 
 */

static int window_info (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	switch (params[1])
	{
		case WINDOW_WIDTH:
			return window->width;
		case WINDOW_HEIGHT:
			return window->height;
		case WINDOW_CAPTION:
			return window->caption;
		case WINDOW_ALPHA:
			return window->alpha;
		case WINDOW_ONTOP:
			return window->stayontop;
		case WINDOW_MOVEABLE:
			return window->moveable;
		case WINDOW_MODAL:
			return window->modal;
		case WINDOW_DONTFOCUS:
			return window->dontfocus;
		case WINDOW_FULLSCREEN:
			return window->fullscreen;
		case WINDOW_MAXIMIZED:
			return window->maximized;
	}
	return 0;
}

/** WINDOW_CONTROL (WINDOW, NUM)
 *  Returns the control number NUM of the window
 */

static int window_control (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	if (window->control_count > params[1] && params[1] >= 0)
		return (int)window->control[params[1]];
	return 0;
}

/** WINDOW_CONTROLS (WINDOW)
 *  Return the number of controls in a window
 */

static int window_controls (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	return window->control_count;
}

/** WINDOW_TITLE (WINDOW, STRING TITLE)
 *	Sets the window title
 */

static int window_settitle (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	const char * ptr = string_get(params[1]);
	strncpy (window->text, ptr, WINDOW_TEXT_MAX);
	window->text[WINDOW_TEXT_MAX-1] = 0;
	string_discard (params[1]);
	return 0;
}

/** WINDOW_TITLE (WINDOW)
 *	Returns the window title
 */

static int window_gettitle (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	int result = string_new (window->text);
	string_use (result);
	return result;
}

/** WINDOW_TYPE (WINDOW, STRING TYPE)
 *	Sets the user-defined window "type"
 */

static int window_settype (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	if (window->usertype)
		string_discard(window->usertype);
	window->usertype = params[1];
	string_use (window->usertype);
	string_discard (params[1]);
	string_use (window->usertype);
	return window->usertype;
}

/** WINDOW_TYPE (WINDOW)
 *	Returns the user-defined window "type"
 */

static int window_gettype (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	string_use (window->usertype);
	return window->usertype;
}

/** WINDOW_DATA (WINDOW, STRING DATA)
 *	Sets the user-defined window "data"
 */

static int window_setdata (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	if (window->userdata)
		string_discard(window->userdata);
	window->userdata = params[1];
	string_use (window->userdata);
	string_discard (params[1]);
	string_use (window->userdata);
	return window->userdata;
}

/** WINDOW_DATA (WINDOW)
 *	Returns the user-defined window "data"
 */

static int window_getdata (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	string_use (window->userdata);
	return window->userdata;
}

/** WINDOW_FILE (WINDOW, STRING FILE)
 *	Sets the user-defined window "file"
 */

static int window_setfile (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	if (window->userfile)
		string_discard(window->userfile);
	window->userfile = params[1];
	string_use (window->userfile);
	string_discard (params[1]);
	string_use (window->userfile);
	return window->userfile;
}

/** WINDOW_FILE (WINDOW)
 *	Returns the user-defined window "file"
 */

static int window_getfile (INSTANCE * my, int * params)
{
	WINDOW * window = (WINDOW *)params[0];
	string_use (window->userfile);
	return window->userfile;
}

/* ---- Desktop interface ---- */

/** DESKTOP_BRINGTOFRONT(WINDOW)
 *	Give the focus to this window
 */

static int desktop_bringtofront (INSTANCE * my, int * params)
{
	gui_desktop_bringtofront ((WINDOW *)params[0]);
	return 0;
}

/** DESKTOP_SENDTOBAC(WINDOW)
 *	Send this window to the backmost plane
 */

static int desktop_sendtoback (INSTANCE * my, int * params)
{
	gui_desktop_sendtoback ((WINDOW *)params[0]);
	return 0;
}

static int desktop_addwindow (INSTANCE * my, int * params)
{
	gui_desktop_addwindow ((WINDOW *) params[0]);
	return 0;
}

static int desktop_removewindow (INSTANCE * my, int * params)
{
	gui_desktop_removewindow ((WINDOW *) params[0]);
	return 0;
}

static int desktop_draw (INSTANCE * my, int * params)
{
	GRAPH * graph = bitmap_get (params[0], params[1]);
	if (graph != NULL)
		gui_desktop_draw (graph, 0);
	return 0;
}

static int desktop_findwindow (INSTANCE * my, int * params)
{
	return (int) gui_desktop_findwindow (params[0], params[1]);
}

static int desktop_addpopup (INSTANCE * my, int * params)
{
	return gui_desktop_addpopup ((WINDOW *) params[0], (CONTROL *) params[1], params[2], params[3]);
}

static int desktop_removepopup (INSTANCE * my, int * params)
{
	gui_desktop_removepopup ((CONTROL *) params[0]);
	return 0;
}

static int desktop_topwindow (INSTANCE * my, int * params)
{
	return (int)gui_desktop_lastwindow();
}

static int desktop_bottomwindow (INSTANCE * my, int * params)
{
	return (int)gui_desktop_firstwindow();
}

static int desktop_nextwindow (INSTANCE * my, int * params)
{
	return (int)gui_desktop_nextwindow((WINDOW *)params[0]);
}

/* Tabbed dialog */

/** TABS (WIDTH, HEIGHT)
 *  Create a new tabbed dialog control */

static int tabs_new (INSTANCE * my, int * params)
{
	return (int) gui_tabs_new (params[0], params[1]);
}

/** TABS_ADDPAGE (TABS, STRING TITLE)
 *  Add a new page to the tabbed dialog */

static int tabs_addpage (INSTANCE * my, int * params)
{
	gui_tabs_addpage ((CONTROL *)params[0], params[1]);
	string_discard(params[1]);
	return 0;
}

/** TABS_ADDCONTROL (TABS, X, Y, CONTROL)
 *  Add a new control to the current page of the tabbed dialog */

static int tabs_addcontrol (INSTANCE * my, int * params)
{
	gui_tabs_addcontrol ((CONTROL *)params[0], params[1], params[2], (CONTROL *)params[3]);
	return 0;
}

/** TABS_PAGE (TABS)
 *  Return the selected page of the tabbed dialog */

static int tabs_getpage (INSTANCE * my, int * params)
{
	return ((TABS *)params[0])->current_page;
}

/** TABS_PAGE (TABS, PAGE)
 *  Select a new page in a tabbed dialog */

static int tabs_setpage (INSTANCE * my, int * params)
{
	if (params[1] >= 0 && params[1] < ((TABS *)params[0])->pages_count)
		((TABS *)params[0])->current_page = params[1];
	return 0;
}

/** TABS_BORDER (TABS, BORDER)
 *  Change the border (padding) of a tabbed dialog (0 for no border and no line) */

static int tabs_setborder (INSTANCE * my, int * params)
{
	((TABS *)params[0])->border = params[1];
	return 0;
}

/* Portapapeles */

/** COPY_TEXT(STRING TEXT)
 *  Put some text on the clipboard
 */

static int copy_text (INSTANCE * my, int * params)
{
	const char * str = string_get(params[0]);
	gui_copy_text (str, -1);
	string_discard(params[0]);
	return 0;
}

/** PASTE_TEXT()
 *  Returns all text in the clipboard
 */

static int paste_text (INSTANCE * my, int * params)
{
	const char * ptr = gui_paste_text();
	int result;

	if (ptr == 0)
		return 0;
	result = string_new(ptr);
	string_use(result);
	return result;
}

/** PASTE_GRAPH()
 *  Returns the current graphic in the clipboard
 */

static int fxi_paste_graph (INSTANCE * my, int * params)
{
	GRAPH * graph = gui_paste_graph();
	if (graph == 0)
		return 0;
	return graph->code;
}

/** COPY_GRAPH (FPG, GRAPH)
 *  Put a graphic on the clipboard
 */

static int copy_graph (INSTANCE * my, int * params)
{
	GRAPH * graph = bitmap_get(params[0], params[1]);
	if (graph != NULL)
		gui_copy_graph(graph);
	return 0;
}

/* Rutinas auxiliares para controles */

static int control_resize (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	if (c->min_width != -1 && params[1] < c->min_width)
		params[1] = c->min_width;
	if (c->min_height != -1 && params[2] < c->min_height)
		params[2] = c->min_height;
	if (c->max_width != -1 && params[1] > c->max_width)
		params[1] = c->max_width;
	if (c->max_height != -1 && params[2] > c->max_height)
		params[2] = c->max_height;

	c->width = params[1];
	c->height = params[2];
	return (int)c;
}

static int control_tooltip (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	c->tooltip = params[1];
	/* Don't discard the string - control destructor does */
	return 0;
}

static int control_lock (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	if (params[1])
		c->hresizable = 0;
	if (params[2])
		c->vresizable = 0;
	return 0;
}

/** CONTROL_FOCUS (CONTROL)
 *	Gives the keyboard focus to the control (only works if the window is at top)
 */

static int control_focus (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	gui_window_setfocus (c->window, c);
	return 0;
}

/** CONTROL_FOCUSED (CONTROL)
 *	Returns 1 if the control was focused recently and resets the flag
 */

static int control_focused (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	int result = (c->actionflags & ACTION_ENTER) ? 1 : 0;
	c->actionflags &= ~ACTION_ENTER;
	return result;
}

/** CONTROL_LEAVED (CONTROL)
 *	Returns 1 if the control lost the focus recently, and resets the flag
 */

static int control_leaved (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	int result = (c->actionflags & ACTION_LEAVE) ? 1 : 0;
	c->actionflags &= ~ACTION_LEAVE;
	return result;
}

/** CONTROL_CHANGED (CONTROL)
 *  Returns 1 if the control has changed (selection or contents)
 */

static int control_changed (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	int result = (c->actionflags & ACTION_CHANGE) ? 1 : 0;
	c->actionflags &= ~ACTION_CHANGE;
	return result;
}

/** CONTROL_POINTER (CONTROL, POINTER)
 *	Sets the control mouse pointer type
 */

static int control_pointer (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	return c->pointer = params[1];
}

/** CONTROL_PRESSED (CONTROL, BUTTON)
 *	Returns 1 if the control was clicked using the given button and resets the flag
 */

static int control_pressed (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	int result = (c->actionflags & (1 << params[1])) ? 1 : 0;
	c->actionflags &= ~(1 << params[1]);
	return result;
}

/** CONTROL_DOUBLECLICK (CONTROL)
 *	Returns 1 if the control received a double click recently and resets the flag
 */

static int control_doubleclick (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	int result = (c->actionflags & ACTION_DOUBLECLICK) ? 1 : 0;
	c->actionflags &= ~ACTION_DOUBLECLICK;
	return result;
}

/** CONTROL_DRAGGABLE (CONTROL, STRING TYPE)
 *	Enables mouse dragging of some object
 */

static int control_draggable (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	if (control->drstring)
		string_discard (control->drstring);

	/* control->draggable may be 2 (valid but inactive at current position) */
	if (!params[1] || string_get(params[1])[0] == 0)
		control->draggable = 0;
	else if (control->draggable == 0)
		control->draggable = 1;

	control->drstring = params[1];
	string_use (params[1]);
	string_discard (params[1]);
	return 0;
}

/** CONTROL_DRAGGABLE (CONTROL, STRING TYPE, LIBRARY, MAP)
 *	Enables mouse dragging of some object, with an user-defined graphic
 */

static int control_draggable_2 (INSTANCE * my, int * params)
{
	CONTROL * control = (CONTROL *)params[0];
	control_draggable (my, params);
	control->drgraph = bitmap_get (params[2], params[3]);
	return 0;
}

/** GUI_DRAGGING()
 *	Returns one of the DRAG_XXX constants
 */

static int gui_dragging (INSTANCE * my, int * params)
{
	return gui_desktop_dragging();
}

/** GUI_DRAGTYPE()
 *	Returns the string associated to the type of the current dragging operation
 */

static int gui_dragtype (INSTANCE * my, int * params)
{
	string_use (gui_desktop_dragtype());
	return gui_desktop_dragtype();
}

/** GUI_DRAGDESTINATION()
 *	Returns the destination control for the dragging operation
 */

static int gui_dragdestination (INSTANCE * my, int * params)
{
	return (int)gui_desktop_dragcontrol();
}

/** GUI_ACCEPTDRAG()
 *	Enables the accept-drag flag for the next frame
 */

static int gui_acceptdrag (INSTANCE * my, int * params)
{
	gui_desktop_acceptdrag(1);
	return 1;
}

/** GUI_POINTER (POINTER)
 *	Sets the default mouse pointer
 */

static int gui_pointer_2 (INSTANCE * my, int * params)
{
	gui_desktop_setpointer(params[0]);
	return params[0];
}

/** GUI_POINTER ()
 *	Gets the default mouse pointer
 */

static int gui_pointer (INSTANCE * my, int * params)
{
	return gui_desktop_getpointer();
}

/** CONTROL_INFO (CONTROL, INFOTYPE)
 *	Returns information regarding the control
 */

static int control_info (INSTANCE * my, int * params)
{
	CONTROL * c = (CONTROL *)params[0];
	switch (params[1])
	{
		case 1:
			return c->width;
		case 2:
			return c->height;
		case 3:
			return c->x;
		case 4:
			return c->y;
		case 5:
			return c->hidden;
		case 6:
			return c->min_width;
		case 7:
			return c->max_width;
		case 8:
			return c->min_height;
		case 9:
			return c->max_height;
		case 10:
			return c->hresizable;
		case 11:
			return c->vresizable;
	}
	return 0;
}

/** Registers DLL functions with FXI and FXC
 */

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("GUI_SHOW",				"I",		TYPE_DWORD,		gui_show);
	FENIX_export ("GUI_HIDE",				"",			TYPE_DWORD,		gui_hide);
	FENIX_export ("DESKTOP_DRAW",			"II",		TYPE_DWORD,		desktop_draw);
	FENIX_export ("DESKTOP_ADDWINDOW",		"I",		TYPE_DWORD,		desktop_addwindow);
	FENIX_export ("DESKTOP_REMOVEWINDOW",	"I",		TYPE_DWORD,		desktop_removewindow);
	FENIX_export ("DESKTOP_FINDWINDOW",		"II",		TYPE_DWORD,		desktop_findwindow);
	FENIX_export ("DESKTOP_ADDPOPUP",		"IIII",		TYPE_DWORD,		desktop_addpopup);
	FENIX_export ("DESKTOP_REMOVEPOPUP",	"I",		TYPE_DWORD,		desktop_removepopup);
	FENIX_export ("DESKTOP_TOPWINDOW",		"",			TYPE_DWORD,		desktop_topwindow);
	FENIX_export ("DESKTOP_NEXTWINDOW",		"I",		TYPE_DWORD,		desktop_nextwindow);
	FENIX_export ("DESKTOP_BOTTOMWINDOW",	"",			TYPE_DWORD,		desktop_bottomwindow);
	FENIX_export ("WINDOW",					"",			TYPE_DWORD,		window_new);
	FENIX_export ("WINDOW",					"I",		TYPE_DWORD,		window_new_2);
	FENIX_export ("WINDOW",					"S",		TYPE_DWORD,		window_new_3);
	FENIX_export ("WINDOW",					"SI",		TYPE_DWORD,		window_new_4);
	FENIX_export ("WINDOW_DESTROY",			"I",		TYPE_DWORD,		window_destroy);
	FENIX_export ("WINDOW_ADDCONTROL",		"IIII",		TYPE_DWORD,		window_addcontrol);
	FENIX_export ("WINDOW_RESIZE",			"III",		TYPE_DWORD,		window_resize);
	FENIX_export ("WINDOW_MOVE",			"III",		TYPE_DWORD,		window_move);
	FENIX_export ("WINDOW_SET",	            "III",		TYPE_DWORD,		window_set);
	FENIX_export ("WINDOW_VISIBLE",         "I",		TYPE_DWORD,		window_visible);
	FENIX_export ("WINDOW_TITLE",			"I",		TYPE_STRING,	window_gettitle);
	FENIX_export ("WINDOW_TITLE",			"IS",		TYPE_DWORD,		window_settitle);
	FENIX_export ("WINDOW_TYPE",			"I",		TYPE_STRING,	window_gettype);
	FENIX_export ("WINDOW_TYPE",			"IS",		TYPE_STRING,	window_settype);
	FENIX_export ("WINDOW_DATA",			"I",		TYPE_STRING,	window_getdata);
	FENIX_export ("WINDOW_DATA",			"IS",		TYPE_STRING,	window_setdata);
	FENIX_export ("WINDOW_FILE",			"I",		TYPE_STRING,	window_getfile);
	FENIX_export ("WINDOW_FILE",			"IS",		TYPE_STRING,	window_setfile);
	FENIX_export ("BUTTON",					"S",		TYPE_DWORD,  	button_new);
	FENIX_export ("BUTTON",					"SI",		TYPE_DWORD,  	button_new_2);
	FENIX_export ("BUTTON_PRESSED",			"I",		TYPE_DWORD,  	button_pressed);
	FENIX_export ("RADIO",					"SPI",		TYPE_DWORD,  	radio_new);
	FENIX_export ("CHECKBOX",				"SP",		TYPE_DWORD,  	checkbox_new);
	FENIX_export ("INPUTLINE",				"IV++",		TYPE_DWORD,  	inputline_new);
	FENIX_export ("HSCROLLBAR",				"IP",		TYPE_DWORD,  	hscrollbar_new);
	FENIX_export ("VSCROLLBAR",				"IP",		TYPE_DWORD,  	vscrollbar_new);
	FENIX_export ("HSCROLLBAR",				"IPI",		TYPE_DWORD,  	hscrollbar_new_2);
	FENIX_export ("VSCROLLBAR",				"IPI",		TYPE_DWORD,  	vscrollbar_new_2);
	FENIX_export ("CONTAINER",				"II",		TYPE_DWORD,  	container_new);
	FENIX_export ("CONTAINER_ADD",			"IIII",		TYPE_DWORD,  	container_add);
	FENIX_export ("CONTAINER_MOVE",			"IIII",		TYPE_DWORD,  	container_move);
	FENIX_export ("CONTAINER_REMOVE",		"II",		TYPE_DWORD,  	container_remove);
	FENIX_export ("FRAME",					"III",		TYPE_DWORD,  	frame_new);
	FENIX_export ("TABLE",					"II",		TYPE_DWORD,  	table_new);
	FENIX_export ("TABLE_ADDCOLUMN",		"ISV++",	TYPE_DWORD,  	table_addcolumn);
	FENIX_export ("TABLE_ADDCOLUMN_BITMAPS","IS",		TYPE_DWORD,  	table_addcolumn_b);
	FENIX_export ("TABLE_ADDCOLUMN",		"IS",		TYPE_DWORD,  	table_addcolumn_0);
	FENIX_export ("TABLE_ADDLINE",			"IS",		TYPE_DWORD,  	table_addline_1);
	FENIX_export ("TABLE_ADDLINE",			"ISS",		TYPE_DWORD,  	table_addline_2);
	FENIX_export ("TABLE_ADDLINE",			"ISSS",	 TYPE_DWORD,  		table_addline_3);
	FENIX_export ("TABLE_ADDLINE",			"ISSSS",	TYPE_DWORD,  	table_addline_4);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSS",	TYPE_DWORD,  	table_addline_5);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSS",	TYPE_DWORD,  	table_addline_6);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSS", TYPE_DWORD,  	table_addline_7);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSS",	TYPE_DWORD, table_addline_8);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSS",	TYPE_DWORD, table_addline_9);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSS",	TYPE_DWORD, table_addline_10);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSSS", TYPE_DWORD, table_addline_11);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSSSS",	TYPE_DWORD,	table_addline_12);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSSSSS",	TYPE_DWORD,	table_addline_13);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSSSSSS",	TYPE_DWORD,	table_addline_14);
	FENIX_export ("TABLE_ADDLINE",			"ISSSSSSSSSSSSSSS",TYPE_DWORD,	table_addline_15);
	FENIX_export ("TABLE_REMOVELINES",		"III",		TYPE_DWORD,  	table_removelines);
	FENIX_export ("TABLE_MOVELINES",		"IIII",		TYPE_DWORD,  	table_movelines);
	FENIX_export ("TABLE_REMOVECOLUMN",		"IS",		TYPE_DWORD,  	table_removecolumn);
	FENIX_export ("TABLE_ALIGNMENT",		"ISI",		TYPE_DWORD,  	table_alignment);
	FENIX_export ("TABLE_SELECTEDCOUNT",	"I",		TYPE_DWORD,  	table_selectedcount);
	FENIX_export ("TABLE_SELECTED",			"I",		TYPE_DWORD,  	table_selected);
	FENIX_export ("TABLE_SELECTED",			"II",		TYPE_DWORD,  	table_selected_2);
	FENIX_export ("TABLE_SELECT",			"II",		TYPE_DWORD,  	table_select);
	FENIX_export ("TABLE_SELECT",			"III",		TYPE_DWORD,  	table_select_2);
	FENIX_export ("TABLE_LINES"	,			"I",		TYPE_DWORD,  	table_lines);
	FENIX_export ("TABLE_LIBRARY",			"ISI",		TYPE_DWORD,  	table_library);
	FENIX_export ("TABLE_SORT",				"ISI",		TYPE_DWORD,  	table_sort);
	FENIX_export ("TABLE_SET",				"III",		TYPE_DWORD,		table_set);
	FENIX_export ("TABLE_GET",				"IIS",		TYPE_STRING,	table_get);
	FENIX_export ("TABLE_CHANGE",			"IISS",		TYPE_DWORD,	    table_change);
	FENIX_export ("TABLE_PREVIEWMODE",		"IIISS",	TYPE_DWORD,		table_enablepreview);
	FENIX_export ("TABLE_NORMALMODE",		"I",		TYPE_DWORD,		table_disablepreview);
	FENIX_export ("TABLE_AUTOEXPAND",		"IS",		TYPE_DWORD,		table_autoexpand);
	FENIX_export ("TABLE_WIDTH",			"I",		TYPE_DWORD,		table_width);
	FENIX_export ("TABLE_WIDTH",			"IS",		TYPE_DWORD,		table_width_2);
	FENIX_export ("TABLE_WIDTH",			"ISI",		TYPE_DWORD,		table_width_3);
	FENIX_export ("LABEL",					"S",		TYPE_DWORD,  	label_new);
	FENIX_export ("LABEL",					"SI",		TYPE_DWORD,  	label_new_2);
	FENIX_export ("LABEL",					"SII",		TYPE_DWORD,  	label_new_3);
	FENIX_export ("LABEL_TEXT",				"IS",		TYPE_DWORD,  	label_text);
	FENIX_export ("MENU",	    			"",			TYPE_DWORD,		menu_begin);
	FENIX_export ("MENU_SEPARATOR",			"I",		TYPE_DWORD,		menu_separator);
	FENIX_export ("MENU_TOGGLE",   			"ISP", 		TYPE_DWORD,		menu_toggle);
	FENIX_export ("MENU_RADIO",    			"ISPI",		TYPE_DWORD,		menu_radio);
	FENIX_export ("MENU_ACTION",   			"IS",		TYPE_DWORD,		menu_action);
	FENIX_export ("MENU_ACTION",   			"ISI",		TYPE_DWORD,		menu_action_2);
	FENIX_export ("MENU_SUBMENU",			"ISI",		TYPE_DWORD,		menu_submenu);
	FENIX_export ("MENU_ENABLE",			"IS",		TYPE_DWORD,		menu_enable);
	FENIX_export ("MENU_DISABLE",			"IS",		TYPE_DWORD,		menu_disable);
	FENIX_export ("MENU_DESTROY",			"I",		TYPE_DWORD,		menu_destroy);
	FENIX_export ("POPUP_NEW",				"I",		TYPE_DWORD,		popup_new);
	FENIX_export ("TASKBAR",				"",			TYPE_DWORD,		taskbar_new);
	FENIX_export ("TOOLBAR",				"",			TYPE_DWORD,		toolbar_new);
	FENIX_export ("TOOLBAR_MENU",			"ISI",		TYPE_DWORD,		toolbar_menu);
	FENIX_export ("TOOLBAR_MENU",			"ISSI",		TYPE_DWORD,		toolbar_menu_2);
	FENIX_export ("TOOLBAR_CONTROL",		"II",		TYPE_DWORD,		toolbar_control);
	FENIX_export ("TOOLBAR_BUTTON",			"III",		TYPE_DWORD,		toolbar_button);
	FENIX_export ("TOOLBAR_BUTTON",			"IIIS",		TYPE_DWORD,		toolbar_button2);
	FENIX_export ("TOOLBAR_BUTTON",			"IIISP",	TYPE_DWORD,		toolbar_button3);
	FENIX_export ("TOOLBAR_BUTTON",			"IIISPI",	TYPE_DWORD,		toolbar_button4);
	FENIX_export ("TOOLBAR_SEPARATOR",		"I",		TYPE_DWORD,		toolbar_separator);
	FENIX_export ("TOOLBAR_PRESSED",		"I",		TYPE_DWORD,		toolbar_pressed);
	FENIX_export ("PALETTE_PANEL",			"II",		TYPE_DWORD,		palettepanel_new);
	FENIX_export ("PALETTE_PANEL",			"IIP",		TYPE_DWORD,		palettepanel_new_2);
	FENIX_export ("PALETTE_PANEL_SET",		"IP",		TYPE_DWORD,		palettepanel_set);
	FENIX_export ("PALETTE_PANEL_SELECTION","IP",		TYPE_DWORD,		palettepanel_selection);
	FENIX_export ("IMAGE",					"II",		TYPE_DWORD,		imageview_new);
	FENIX_export ("IMAGE_VIEW",				"II",		TYPE_DWORD,		imageview_new_2);
	FENIX_export ("IMAGE_VIEW",				"III",		TYPE_DWORD,		imageview_new_3);
	FENIX_export ("IMAGE_VIEW",				"IIII",		TYPE_DWORD,		imageview_new_4);
	FENIX_export ("IMAGE_VIEW_SET",			"III",		TYPE_DWORD,		imageview_setimage);
	FENIX_export ("IMAGE_VIEW_ZOOM",		"IIII",		TYPE_DWORD,		imageview_zoom);
	FENIX_export ("DROPDOWN",				"IP",		TYPE_DWORD,		dropdown_new);
	FENIX_export ("DROPDOWN",				"IPV++",	TYPE_DWORD,		dropdown_new_2);
	FENIX_export ("DROPDOWN_ADDOPTION",		"IS",		TYPE_DWORD,		dropdown_addoption);
	FENIX_export ("CONTROL_INFO",      		"II",		TYPE_DWORD,		control_info);
	FENIX_export ("CONTROL_RESIZE",			"III",		TYPE_DWORD,		control_resize);
	FENIX_export ("CONTROL_TOOLTIP",		"IS",		TYPE_DWORD,		control_tooltip);
	FENIX_export ("CONTROL_LOCK",			"III",		TYPE_DWORD,		control_lock);
	FENIX_export ("CONTROL_FOCUS",			"I",		TYPE_DWORD,		control_focus);
	FENIX_export ("CONTROL_FOCUSED",		"I",		TYPE_DWORD,		control_focused);
	FENIX_export ("CONTROL_LEAVED",			"I",		TYPE_DWORD,		control_leaved);
	FENIX_export ("CONTROL_PRESSED",		"II",		TYPE_DWORD,		control_pressed);
	FENIX_export ("CONTROL_POINTER",		"II",		TYPE_DWORD,		control_pointer);
	FENIX_export ("CONTROL_DOUBLECLICK",	"I",		TYPE_DWORD,		control_doubleclick);
	FENIX_export ("CONTROL_CHANGED",		"I",		TYPE_DWORD,		control_changed);
	FENIX_export ("CONTROL_DRAGGABLE",      "IS",		TYPE_DWORD,		control_draggable);
	FENIX_export ("CONTROL_DRAGGABLE",      "ISII",		TYPE_DWORD,		control_draggable_2);
	FENIX_export ("GUI_DRAGGING",			"",			TYPE_DWORD,		gui_dragging);
	FENIX_export ("GUI_DRAGTYPE",	     	"",			TYPE_STRING,	gui_dragtype);
	FENIX_export ("GUI_DRAGDESTINATION", 	"",			TYPE_DWORD,		gui_dragdestination);
	FENIX_export ("GUI_ACCEPTDRAG",			"",			TYPE_DWORD,		gui_acceptdrag);
	FENIX_export ("GUI_POINTER",		 	"I",		TYPE_DWORD,		gui_pointer_2);
	FENIX_export ("GUI_POINTER",		 	"",			TYPE_DWORD,		gui_pointer);
	FENIX_export ("DESKTOP_BRINGTOFRONT", 	"I",		TYPE_DWORD,		desktop_bringtofront);
	FENIX_export ("DESKTOP_SENDTOBACK", 	"I",		TYPE_DWORD,		desktop_sendtoback);
	FENIX_export ("WINDOW_CHANGED",			"I",		TYPE_DWORD,		window_getchanged);
	FENIX_export ("WINDOW_CHANGED",			"II",		TYPE_DWORD,		window_setchanged);
	FENIX_export ("WINDOW_INFO",			"II",		TYPE_DWORD,		window_info);
	FENIX_export ("WINDOW_CONTROL",			"II",		TYPE_DWORD,		window_control);
	FENIX_export ("WINDOW_CONTROLS",		"I",		TYPE_DWORD,		window_controls);
	FENIX_export ("COPY_GRAPH",				"II",		TYPE_DWORD,		copy_graph);
	FENIX_export ("COPY_TEXT",				"S",		TYPE_DWORD,		copy_text);
	FENIX_export ("PASTE_TEXT",				"",			TYPE_STRING,	paste_text);
	FENIX_export ("PASTE_GRAPH",			"",			TYPE_DWORD,		fxi_paste_graph);
	FENIX_export ("GREDITOR",				"IIPII",	TYPE_DWORD,		greditor_new);
	FENIX_export ("GREDITOR_INFO",			"I",		TYPE_POINTER,	greditor_info);
	FENIX_export ("GREDITOR_ACTIVE",		"I",		TYPE_DWORD,		greditor_active);
	FENIX_export ("GREDITOR_ACTIVE",		"II",		TYPE_DWORD,		greditor_active_2);
	FENIX_export ("GREDITOR_SHOWBARS",		"II",		TYPE_DWORD,		greditor_showbars);
	FENIX_export ("GREDITOR_ZOOMTO",		"IIII",		TYPE_DWORD,		greditor_zoomto);
	FENIX_export ("GREDITOR_UNDOMEM", 		"I",		TYPE_DWORD,		greditor_undomem);
	FENIX_export ("GRPALETTE",				"PI",		TYPE_DWORD,		grpalette_new);
	FENIX_export ("GRPALETTE_SET",			"IP",		TYPE_DWORD,		grpalette_set);
	FENIX_export ("COLOR_CHOOSER",			"PI",		TYPE_DWORD,		color_chooser_new);
	FENIX_export ("CLICKABLE_COLOR",		"P",		TYPE_DWORD,		clickablecolor_new);
	FENIX_export ("CLICKABLE_COLOR",		"PI",		TYPE_DWORD,		clickablecolor_new_2);
	FENIX_export ("TABS",                   "II",       TYPE_DWORD,     tabs_new);
	FENIX_export ("TABS_ADDPAGE",           "IS",       TYPE_DWORD,     tabs_addpage);
	FENIX_export ("TABS_ADDCONTROL",        "IIII",     TYPE_DWORD,     tabs_addcontrol);
	FENIX_export ("TABS_PAGE",              "I",        TYPE_DWORD,     tabs_getpage);
	FENIX_export ("TABS_PAGE",              "II",       TYPE_DWORD,     tabs_setpage);
	FENIX_export ("TABS_BORDER",            "II",       TYPE_DWORD,     tabs_setborder);

	/* TEMPORARY */
	FENIX_export ("MAP_SLICE",      		"II",		TYPE_DWORD,		fxi_map_slice);
	FENIX_export ("MAP_SETDEPTH",      		"III",		TYPE_DWORD,		fxi_map_setdepth);
}


