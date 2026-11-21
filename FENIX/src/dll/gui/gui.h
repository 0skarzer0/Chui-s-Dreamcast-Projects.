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
 * FILE        : gui.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#ifndef __GUI_H
#define __GUI_H

/* Predefined controls */

#include "gui_control.h"
#include "gui_window.h"
#include "gui_button.h"
#include "gui_radio.h"
#include "gui_checkbox.h"
#include "gui_dropdown.h"
#include "gui_inputline.h"
#include "gui_frame.h"
#include "gui_label.h"
#include "gui_scrollbar.h"
#include "gui_scrollable.h"
#include "gui_container.h"
#include "gui_table.h"
#include "gui_menu.h"
#include "gui_tool.h"
#include "gui_imageeditor.h"
#include "gui_imageview.h"
#include "gui_palettepanel.h"
#include "gui_tabs.h"
#include "gui_toolbar.h"
#include "gui_taskbar.h"
#include "gui_colorchooser.h"

/* Drag/drop constants */

#define DRAG_INACTIVE	0
#define DRAG_ACTIVE		1
#define DRAG_DROPPED	2

/* Stock mouse pointers */

#define POINTER_ARROW		0
#define POINTER_RESIZEH		1
#define POINTER_RESIZEV		2
#define POINTER_RESIZENW	3
#define POINTER_RESIZENE    4
#define POINTER_CROSS		5
#define POINTER_IBEAM		6
#define POINTER_RESIZEH2	7
#define POINTER_HAND    	8
#define POINTER_DRAGOBJECT 	9
#define POINTER_ARROWCOPY   10
#define POINTER_BADDROP     11
#define POINTER_ZOOM		12
#define POINTER_DRAWLINE	16
#define POINTER_DRAWRECT	17
#define POINTER_DRAWCIRCLE	18
#define POINTER_DRAWSELECT	19
#define POINTER_DRAWFILL	20
#define POINTER_DROPPER		21
#define POINTER_WAND		22
#define POINTER_BUCKET		23

/* Desktop functions */

extern void		gui_desktop_addwindow (WINDOW * win);
extern void		gui_desktop_removewindow (WINDOW * win);
extern void		gui_desktop_draw (GRAPH * dest, REGION * clip);
extern void		gui_desktop_mousemove (int x, int y);
extern void		gui_desktop_mousebutton (int x, int y, int b, int pressed);
extern void		gui_desktop_key (int scancode, int key);
extern void		gui_desktop_removepopup (CONTROL * control);
extern void		gui_desktop_removepopup_p (CONTROL * control, CONTROL * parent);
extern int		gui_desktop_addpopup (WINDOW * window, CONTROL * control, int x, int y);
extern int		gui_desktop_ispopup (CONTROL * control);
extern WINDOW * gui_desktop_findwindow (int x, int y);
extern WINDOW * gui_desktop_firstwindow();
extern WINDOW * gui_desktop_lastwindow();
extern WINDOW * gui_desktop_nextwindow(WINDOW *);
extern void     gui_desktop_adjustdock();
extern void     gui_desktop_visibleregion (REGION *);
extern void		gui_desktop_bringtofront (WINDOW * w);
extern void		gui_desktop_sendtoback (WINDOW * w);
extern int      gui_desktop_mousepointer();
extern void		gui_desktop_draw_region (GRAPH * dest, REGION * region);
extern void		gui_desktop_draw_region_below (GRAPH * dest, REGION * region, WINDOW * win);
extern void		gui_desktop_adjustdock();
extern int	    gui_desktop_present (WINDOW * win);
extern void     gui_desktop_tooltip (const char * text);
extern void		gui_desktop_dragobject (int string, GRAPH * graphic);
extern void	    gui_desktop_enddrag();
extern int		gui_desktop_dragging();
extern int		gui_desktop_dragtype();
extern CONTROL *gui_desktop_dragcontrol();
extern void		gui_desktop_setpointer (int p);
extern int 		gui_desktop_getpointer ();
extern void		gui_desktop_acceptdrag (int accepted);
extern void	    gui_desktop_capturemouse (WINDOW * window);
extern void		gui_desktop_releasemouse (WINDOW * window);
extern void     gui_desktop_markpopups();

/* Utility functions */

extern void gui_text_put (GRAPH * dest, REGION * clip, int x, int y, const char * text, int alignment, int lead);
extern int  gui_text_height (const char * text, int lead);
extern int  gui_text_width (const char * text);
extern int  gui_line_width (const char * text);
extern void gui_scale_image (GRAPH * dest, int x0, int y0, int x1, int y1, int flags, REGION * clip, GRAPH * graph);

extern void rgb2hsl (int r, int g, int b, int *h, int *s, int *l);
extern void hsl2rgb (int h, int s, int l, int *r, int *g, int *b);

/* Clipboard functions */

extern GRAPH *		gui_paste_graph();
extern void			gui_copy_graph (GRAPH * graph);
extern const char * gui_paste_text();
extern void			gui_copy_text (const char * text, int size);

/* Actions */

extern void gui_action_closewindow (WINDOW *);
extern void gui_action_maximizewindow (WINDOW *);
extern void gui_action_minimizewindow (WINDOW *);

/* Colors */

extern int color_face;					/**< Color of a button face */
extern int color_highlightface;			/**< Color of a button face with a mouse-over */
extern int color_input;					/**< Color of an input line contens */
extern int color_highlight;				/**< Color of a button highlight */
extern int color_shadow;				/**< Color of a button shadow */
extern int color_tablebg;				/**< Color of even table lines */
extern int color_text;					/**< Color of a button label */
extern int color_border;				/**< Color of a button black border */
extern int color_window;				/**< Color of the window background */
extern int color_selection;				/**< Color of the selection */
extern int color_captionfg;     		/**< Color of the window caption (selected window) */
extern int color_captionbg;     		/**< Color of the window caption (others) */

/* Keyboard modifiers */

#define KEYMOD_ALT		0x80000000
#define KEYMOD_CONTROL	0x40000000
#define KEYMOD_SHIFT	0x20000000

/* Icons & Cursors */

extern GRAPH * cursors[];
extern GRAPH * cursors_black[];

extern void gui_create_cursors();

/* Bitmap utilities */

extern void    bitmap_slice (GRAPH * graph, int x, int y, int width, int height);
extern void    bitmap_getoptimalbbox (GRAPH * graph, REGION * bbox);
extern GRAPH * bitmap_extract (GRAPH * graph, REGION * bbox);
extern void    bitmap_subcopy (GRAPH * dest, GRAPH * src, REGION * bbox);
extern void    bitmap_setdepth (GRAPH * graph, int newdepth);

extern int gui_full_redraw;

#endif
