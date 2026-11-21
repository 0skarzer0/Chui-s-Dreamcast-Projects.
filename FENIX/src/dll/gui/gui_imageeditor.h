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
 * FILE        : gui_imageeditor.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_IMAGEEDITOR_H
#define __GUI_IMAGEEDITOR_H

/* ------------------------------------------------------------------------- * 
 *  IMAGE EDITOR
 * ------------------------------------------------------------------------- */

enum TOOLTYPE
{
	TOOLTYPE_LINE		= 0x0001,
	TOOLTYPE_PENCIL		= 0x0002,
	TOOLTYPE_FILL		= 0x0003,
	TOOLTYPE_MULTILINE	= 0x0004,
	TOOLTYPE_MASK		= 0x00FF,
	TOOLTYPE_HARMLESS   = 0x0100,
	TOOLTYPE_DBLCLKADDS = 0x0200,
	TOOLTYPE_TIMER      = 0x0400
};

typedef struct _ant
{
	int		line;					/**< Line of the first point */
	int		col;					/**< Column of the first point */
	int		size;					/**< Width or height in pixels */
	int		topleft;				/**< 1 if top (or left), 0 if bottom (or right) */
}
ANT;

typedef struct _imageeditorinfo
{
	int		tool;					/**< Selected tool index */
	int		zoom;					/**< Zoom level (percentile) */
	int		fg;						/**< Foreground color */
	int		fg8;					/**< Foreground color (8 bits) */
	int		bg;						/**< Background color or -1 for none */
	int		bg8;					/**< Background color (8 bits) */
	int		cpoint;					/**< Number of currently selected cpoint */
	int		alpha;					/**< Current alpha value */
	int		file;					/**< Graph file */
	int		graph;					/**< Graph id */
	int		brush_file;				/**< Brush FPG */
	int		brush_graph;			/**< Brush graph id */
	int		texture_file;			/**< Texture FPG */
	int		texture_graph;			/**< Texture graph id */
	int		x;						/**< Cursor x */
	int		y;						/**< Cursor y */
	int		grid_visible;			/**< |0x01 if pixel grid visible w/zoom, |0x02 if normal grid visible */
	int		grid_step;				/**< Grid step in pixels */
	int		grid_color;				/**< Grid color */
	int		pixelgrid_color;		/**< Pixel grid color */
	int		paper_mode;				/**< Paper mode (0-checkboard, 1-black or 2-solid) */
	int		paper_color;			/**< Paper color (for paper mode 2) */
}
IMAGEEDITORINFO;

typedef struct _multilinepoint
{
	int		x;
	int		y;
}
MULTILINEPOINT;

typedef struct _undoslot
{
	GRAPH * undo;					/**< Graph contents at the changed area, before modifications */
	GRAPH * redo;					/**< Graph contents at the changed area, after modifications (may be NULL) */
	REGION	bbox;					/**< Bounding box of the change */
	int		resized;				/**< 1 if the graphic size was changed at this point */
}
UNDOSLOT;

typedef struct _imageeditor
{
	SCROLLABLE			scrollable;			/**< Parent class data */
	GRAPH *				graph;				/**< Graphic to edit */
	GRAPH *				selection;			/**< Selection mask */
	GRAPH *				temporary;			/**< Temporary graphic, to show tool effect before done */
	int					clickx;				/**< Mouse click stored position */
	int					clicky;				/**< Mouse click stored position */
	int					clicktox;			/**< Mouse drag stored position */
	int					clicktoy;			/**< Mouse drag stored position */
	IMAGEEDITORINFO *	info;				/**< User-modificable information */
	int					selection_dirty;	/**< 1 if the walking ants array need an update */
	int					selection_empty;	/**< 1 if the selection is empty */
	int					space_drag;			/**< 1 if the user is space-dragging the viewport */
	int					dragx;				/**< Original space-drag position */
	int					dragy;				/**< Original space-drag position */
	int					painting;			/**< 1 if painting with a tool */
	int					paintingtool;		/**< Number of tool we're painting with */
	int					lastpx;				/**< Last painted pixel (by pencil tool) */
	int					lastpy;				/**< Last painted pixel (by pencil tool) */
	int					lastfg;				/**< Color for the 0 key (conmuting between color and 0 for fg) */
	int					lastbg;				/**< Color for the ALT+0 key (conmuting between color and void for bg) */
	int					currentfg;			/**< Used to detect changes in foreground color */
	int					currentfg8;			/**< Used to detect changes in foreground color */
	int					currentbg;			/**< Used to detect changes in background color */
	int					currentbg8;			/**< Used to detect changes in background color */
	int					active;				/**< 1 if editing is active, 0 otherwise */
	int					timer;				/**< Last timer value for timer-based tools (airbrush) */

	/* Points for multilines */
	MULTILINEPOINT *	mpoints;			/**< Multiline points */
	int					mpoints_count;		/**< Count of multiline points */
	int					mpoints_allocated;	/**< Space available for multiline points */

	/* Walking ants */
	ANT *				hants;				/**< Array of selection-border strips ("walking ants") */
	ANT *				vants;				/**< Array of selection-border strips ("walking ants") */
	int					hants_count;		/**< Number of selection-border strips ("walking ants") */
	int					vants_count;		/**< Number of selection-border strips ("walking ants") */
	int					hants_allocated;	/**< Space available for selection-border strips */
	int					vants_allocated;	/**< Space available for selection-border strips */

	/* Undo */
	int					undo_memory;		/**< Measure of memory used for undo */
	int					undo_allocated;		/**< Number of slots available for undo graphs */
	int					undo_used;			/**< Real number of slots used for graphics */
	int					undo_pos;			/**< Current undo position (CTRL-Z = decrease and use previous) */
	UNDOSLOT *			undo;				/**< Array of undo data (graphics, bbox, flags...) */
}
IMAGEEDITOR;

extern CONTROL *	gui_imageeditor_new();
extern void			gui_imageeditor_init (IMAGEEDITOR * ed, IMAGEEDITORINFO * info, int width, int height);
extern void			gui_imageeditor_zoomtopoint (IMAGEEDITOR * ed, int x, int y, int zoom);
extern GRAPH *		gui_imageeditor_getselection (IMAGEEDITOR * ed);
extern void			gui_imageeditor_saveundo (IMAGEEDITOR * ed, REGION * bbox);
extern void			gui_imageeditor_undo (IMAGEEDITOR * ed);
extern void			gui_imageeditor_redo (IMAGEEDITOR * ed);
extern void			gui_imageeditor_selectcolor (IMAGEEDITOR * ed, int color);
extern void			gui_imageeditor_invertselection (IMAGEEDITOR * ed);
extern void			gui_imageeditor_removeselection (IMAGEEDITOR * ed);
extern void			gui_imageeditor_movecursor (IMAGEEDITOR * ed, int x, int y);
extern void         gui_imageeditor_destroytemporary (IMAGEEDITOR * ed);

typedef struct _imageeditortool
{
	int type;

	void (*drag)    (IMAGEEDITOR * ed, int x1, int y1, int x2, int y2, int button, int final);
	void (*click)   (IMAGEEDITOR * ed, int x, int y, int button, int first);
	int  (*pointer) (IMAGEEDITOR * ed, int x, int y);
    void (*draw)    (IMAGEEDITOR * ed, GRAPH * dest, int x, int y, int x0, int y0, int x1, int y1, REGION * clip);
    int  (*bbox)    (IMAGEEDITOR * ed, REGION * bbox);
    void (*render)  (IMAGEEDITOR * ed, GRAPH * dest);
}
IMAGEEDITORTOOL;

/* Drawing support routines */

extern int     gdr_addpixel (int x, int y);
extern int     gdr_addspan (int x, int y, int width);
extern void    gdr_reset();
extern void    gdr_truncate (int count);
extern GRAPH * gdr_renderborder (GRAPH * brush, REGION * bbox);
extern void    gdr_renderbycolor (GRAPH * graph, int fg, int bg, int alpha, GRAPH * brush);
extern void    gdr_renderbytexture (GRAPH * graph, GRAPH * texture, int alpha, GRAPH * brush);
extern void    gdr_addline (int x, int y, int x2, int y2);
extern void    gdr_addsegment (int x, int y, int x2, int y2);
extern void    gdr_addrect (int x, int y, int x2, int y2, int filled);
extern void    gdr_addcircle (int x, int y, int radius, int filled);
extern void	   gdr_addellipse (int cx, int cy, int a, int b, int filled);
extern void	   gdr_addfill (int x, int y, GRAPH * graph, int type, int border);
extern void	   gdr_addcurve (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
extern int     gdr_getboundingbox (REGION * bbox, GRAPH * dest, GRAPH * brush);

/* Editor palette (toolbox) */

typedef struct _editorpalette
{
	CONTROL				control;			/**< Parent class data */
	IMAGEEDITORINFO *	info;				/**< Editor information to show/edit in the palette */
	int					palette_file;		/**< FPG file with palette graphics */
	int					rgb;				/**< 1 to show RGBA sliders, 0 to show HSL ones */
	int					h, s, l;			/**< Last values selected by HSL sliders */
}
EDITORPALETTE ;

extern CONTROL * gui_editorpalette_new (IMAGEEDITORINFO * info, int palette_file);

extern GRAPH * paste_graph;

/* Tool array */

extern IMAGEEDITORTOOL tool_pencil;
extern IMAGEEDITORTOOL tool_line;
extern IMAGEEDITORTOOL tool_multiline;
extern IMAGEEDITORTOOL tool_rect;
extern IMAGEEDITORTOOL tool_circle;
extern IMAGEEDITORTOOL tool_cpe;
extern IMAGEEDITORTOOL tool_zoom;
extern IMAGEEDITORTOOL tool_select;
extern IMAGEEDITORTOOL tool_wand;
extern IMAGEEDITORTOOL tool_fill;
extern IMAGEEDITORTOOL tool_curve;
extern IMAGEEDITORTOOL tool_pixels;
extern IMAGEEDITORTOOL tool_polycurve;
extern IMAGEEDITORTOOL tool_paste;
extern IMAGEEDITORTOOL tool_airbrush;

extern IMAGEEDITORTOOL * imageeditor_tools[];

#define TOOL_PENCIL		0
#define TOOL_LINE		1
#define TOOL_RECT		2
#define TOOL_CIRCLE		3
#define TOOL_CPE		4
#define TOOL_ZOOM		5
#define TOOL_SELECT		6
#define TOOL_WAND  		7
#define TOOL_MULTILINE  8
#define TOOL_FILL       9
#define TOOL_POLYCURVE  10
#define TOOL_PIXEL		11
#define TOOL_CURVE		12
#define TOOL_PASTE		13
#define TOOL_AIRBRUSH	14

#define TOOL_COUNT 		15

#endif
