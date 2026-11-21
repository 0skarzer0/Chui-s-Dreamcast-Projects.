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
 * FILE        : gui_table.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_TABLE_H
#define __GUI_TABLE_H

/* ------------------------------------------------------------------------- * 
 *  TABLE CONTROL
 * ------------------------------------------------------------------------- */

#define LIST_TEXTS  		0
#define LIST_STRINGS		1
#define LIST_CUSTOMDRAW		2
#define LIST_CUSTOMTEXTS	3
#define LIST_BITMAPS		4

typedef void (*LISTDRAWPROC)(int line, GRAPH *, REGION *, int x, int y, int width, int height);
typedef const char * (*LISTTEXTPROC)(int line);

/* TABLE options flags */
#define LIST_USER_RESIZABLE			1
#define LIST_USER_SORTABLE			2
#define LIST_USER_MOVEABLE			4
#define LIST_MULTISELECT			8

#define COLUMN_NAME_MAX		64

typedef struct _listcolumn
{
	int      type;						/**< One of the LIST_XXX constants */
	int		 order;						/**< Original order, for ADDLINE */
	char     name[COLUMN_NAME_MAX];		/**< Name of the column, for the title. ASCIIZ. */
	int		 allocated;					/**< Measure of the space allocated with the data pointer */
	int      count;						/**< Number of data values present (may be less than table line count) */
	int  *   data;						/**< Pointer to the data */
	int		 lineheight;				/**< Line height */
	int		 width;						/**< Width of the column, in pixels */
	int		 align;						/**< Text alignment (0:LEFT 1:CENTER 2:RIGHT) */
	int		 library;					/**< FPG library for LIST_BITMAPS */
	int		 minwidth;					/**< Minimal width (for auto-expansion) */
	int		 autoexpand;				/**< 1 if auto-expansion is activated for this column */
}	 
LISTCOLUMN;

typedef struct _table
{
	SCROLLABLE		scroll;			/**< Parent class data */
	int				lines;			/**< Number of lines */
	int				cols;			/**< Number of columns */
	int 			line;			/**< Current line */
	int 			highlight;		/**< Current highlighted line (mouse position) */
	int				lineheight;		/**< Height in pixels of a line */
	int				title;			/**< Height of title caption */
	int				options;		/**< Bit flags with LIST_XXX constants */
	int				drag;			/**< Column currently in resizing mode or 0 for none */
	int				drag_x;			/**< Original X coordinate for drag operation */
	int				drag_width;		/**< Column width prior to drag mode */
	int				drag_column;	/**< Column currently in moving mode */
	int				hovered;		/**< Hovered column, -1 = none */
	int				pressed;		/**< Pressed column, -1 = none */
	int				sorted_by;		/**< Index of column the table is sorted by */
	int				ascendant;		/**< Direction of last sort operation */
	int				pc;				/**< 1 if preview mode is active */
	int				pc_width;		/**< Preview cell width in pixels */
	int				pc_height;		/**< Preview cell height int pixels */
	int				pc_title;		/**< Preview mode title column */
	int				pc_graph;		/**< Preview mode graph column */
	int				pc_cols;		/**< Preview mode column count */
	int				pc_padding;		/**< Preview mode inner icon border */
	int				pc_spacing;		/**< Preview mode spacing between rows/cols */
	int				alphahighlight;	/**< Use alpha blending to highlight selected line */
	unsigned char *	linesel;		/**< Pointer to the line selection buffer */
	int				linesel_alloc;	/**< Size of the line selection buffer */
	int				shiftline;		/**< First line in a shift-click range selection */
	LISTCOLUMN *	column;			/**< Column data */
}
TABLE;

extern CONTROL  * gui_table_new (int width, int height);
extern void		  gui_table_removecolumn (CONTROL * control, int index);
extern void	      gui_table_addcolumn (CONTROL * control, LISTCOLUMN * data, int index);
extern int		  gui_table_findcolumn (CONTROL * control, const char * text);
extern void		  gui_table_addline (CONTROL * control, int count, int * params);
extern void		  gui_table_removelines (CONTROL * control, int first, int count);
extern void		  gui_table_movelines (CONTROL * control, int from, int to, int count);
extern void		  gui_table_fillscroll (TABLE * list);
extern void		  gui_table_sort (CONTROL * control, int column, int ascendant);
extern LISTCOLUMN gui_table_column();
extern void		  gui_table_enablepreview (CONTROL * control, int width, int height, int title, int graph);
extern void		  gui_table_disablepreview (CONTROL * control);
extern int		  gui_table_lineselected (CONTROL * control, int line, int flag);
extern void		  gui_table_select (CONTROL * control, int line);
extern void		  gui_table_unselectall (CONTROL * control);
extern void		  gui_table_selectall (CONTROL * control);

#endif
