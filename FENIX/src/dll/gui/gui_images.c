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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

/** @file
 *
 *  Icon and cursor library
 */

GRAPH * cursors[32]		  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
GRAPH * cursors_black[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const char * cursor_arrow_data[] =
{
	". O . . . . . . . . .",
	". O O . . . . . . . .",
	". O + O . . . . . . .",
	". O + + O . . . . . .",
	". O + + + O . . . . .",
	". O + + + + O . . . .",
	". O + + + + + O . . .",
	". O + + + + + + O . .",
	". O + + + + + + + O .",
	". O + + + + + O O O O",
	". O + + O + + O . . .",
	". O + O O + + O . . .",
	". O O . . O + + O . .",
	". O . . . O + + O . .",
	". . . . . . O + + O .",
	". . . . . . O + + O .",
	". . . . . . . O O . .",
	". . . . . . . . . . .",
	". . . . . . . . . . .",
	0
};

static const char * cursor_zoom_data[] =
{
	". . . . . O O O . . . . .",
	". . . O O + + + O O . . .",
	". . O + + O O O + + O . .",
	". O + O O . . . O O + O .",
	". O + O . . . . . O + O .",
	"O + O . . . . . . . O + O",
	"O + O . . . . . . . O + O",
	"O + O . . . . . . . O + O",
	". O + O . . . . . O + O .",
	". O + O O . . . O O + O .",
	". . O + + O O O + + O . .",
	". . . O O + + + O O O . .",
	". . . . . O O O O + O . .",
	". . . . . . . . O + O . .",
	". . . . . . . . . O + O .",
	". . . . . . . . . O + O .",
	". . . . . . . . . . O + O",
	". . . . . . . . . . O + O",
	". . . . . . . . . . . O .",
	0
};

static const char * cursor_resizev_data[] =
{
	". . . . O O . . . .",
	". . . O + + O . . .",
	". . O + + + + O . .",
	". O + + + + + + O .",
	"O O O O + + O O O O",
	". . . O + + O . . .",
	". . . O + + O . . .",
	". . . O + + O . . .",
	"O O O O + + O O O O",
	". O + + + + + + O .",
	". . O + + + + O . .",
	". . . O + + O . . .",
	". . . . O O . . . .",
	0
};

static const char * cursor_resizenw_data[] =
{
	"O O O O O O O . . .",
	"O + + + + O . . . .",
	"O + + + O . . . . .",
	"O + + + + O . . . O",
	"O + O + + + O . O O",
	"O O . O + + + O + O",
	"O . . . O + + + + O",
	". . . . . O + + + O",
	". . . . O + + + + O",
	". . . O O O O O O O",
	0
};

static const char * cursor_resizeh_data[] =
{
	". . . . O . . . O . . . . ",
	". . . O O . . . O O . . . ",
	". . O + O . . . O + O . . ",
	". O + + O O O O O + + O . ",
	"O + + + + + + + + + + + O ",
	"O + + + + + + + + + + + O ",
	". O + + O O O O O + + O . ",
	". . O + O . . . O + O . . ",
	". . . O O . . . O O . . . ",
	". . . . O . . . O . . . . ",
	0
};

static const char * cursor_cross_data[] =
{
	". . . . . + + + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	"+ + + + + . . . + + + + +",
	"+ O O O O . . . O O O O +",
	"+ + + + + . . . + + + + +",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + O + . . . . .",
	". . . . . + + + . . . . .",
	0
};

static const char * cursor_ibeam_data[] =
{
	". O O O .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". . O . .",
	". O O O .",
	0
};

static const char * cursor_resizeh2_data[] =
{
	". . . . . . + + + . . . . . . ",
	". . . . . . + O + . . . . . . ",
	". . . + + . + O + . + + . . . ",
	". . + O + . + O + . + O + . . ",
	". + O O + + + O + + O O O + . ",
	"+ O O O O O O O O O O O O O + ",
	"+ O O O O O O O O O O O O O + ",
	". + O O + + + O + + + O O + . ",
	". . + O + . + O + . + O + . . ",
	". . . + + . + O + . + + . . . ",
	". . . . . . + O + . . . . . . ",
	". . . . . . + + + . . . . . . ",
	0
};

static const char * cursor_hand_data[] =
{
	". . . . . . . O O . . . . . . . .",
	". . . . O O O + + O O O . . . . .",
	". . . O + + O + + O + + O . . . .",
	". O O O + + O + + O + + O . . . .",
	"O + + O + + O + + O + + O . . . .",
	"O + + O + + O + + O + + O . . . .",
	"O + + O + + O + + O + + O . . . .",
	"O + + O + + O + + O + + O . . . .",
	"O + + O + + + + + + + + O . O O .",
	"O + + + + + + + + + + + + O + + O",
	"O + + + + + + + + + + + + + + + O",
	". O + + + + + + + + + + + + + O .",
	". O + + + + + + + + + + + O O . .",
	". . O + + + + + + + + + O . . . .",
	". . . O O + + + + + + O . . . . .",
	". . . . . O O O O O O . . . . . .",
	0
};

static const char * cursor_drag_data[] =
{
	"O O O O O O O O O O O O O O",
	"O + + + + + + + + + + + + O",
	"O + O O O O O O O O O O + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + O + + + + + + + + + + O",
	"O + + + + + + + + + + + + O",
	"O O O O O O O O O O O O O O",
	0
};

static const char * cursor_arrowcopy_data[] =
{
	". O . . . . . . . . . . . . . . . . . . .",
	". O O . . . . . . . . . . . . . . . . . .",
	". O + O . . . . . . . . . . . . . . . . .",
	". O + + O . . . . . . . . . . . . . . . .",
	". O + + + O . . . . . . . . . . . . . . .",
	". O + + + + O . . . . . . . . . . . . . .",
	". O + + + + + O . . . . . . . . . . . . .",
	". O + + + + + + O . . . . . . . . . . . .",
	". O + + + + + + + O . . . + + + + + . . .",
	". O + + + + + O O O O . . + O O O + . . .",
	". O + + O + + O . . . . . + O + O + . . .",
	". O + O O + + O . . + + + + O + O + + + +",
	". O O . . O + + O . + O O O O + O O O O +",
	". O . . . O + + O . + O + + + + + + + O +",
	". . . . . . O + + O + O O O O + O O O O +",
	". . . . . . O + + O + + + + O + O + + + +",
	". . . . . . . O O . . . . + O + O + . . .",
	". . . . . . . . . . . . . + O O O + . . .",
	". . . . . . . . . . . . . + + + + + . . .",
	0
};

static const char * cursor_baddrop_data[] =
{
	". . . . . . . . . + + + + + + . . . . . . . .",
	". . . . . . + + + O O O O O O + + . . . . . .",
	". . . . + + O O O O O O O O O O O + + . . . .",
	". . . + O O O O O + + + + + O O O O O + . . .",
	". . + O O O + + + . . . . . + + O O O O + . .",
	". . + O O + . . . . . . . . + O O O O O + . .",
	". + O O + . . . . . . . . + O O O + + O O + .",
	". + O O + . . . . . . . + O O O + . + O O + .",
	". + O O + . . . . . . + O O O + . . + O O + .",
	"+ O O + . . . . . . + O O O + . . . . + O O +",
	"+ O O + . . . . . + O O O + . . . . . + O O +",
	"+ O O + . . . . + O O O + . . . . . . + O O +",
	". + O O + . . + O O O + . . . . . . + O O + .",
	". + O O + . + O O O + . . . . . . . + O O + .",
	". + O O + + O O O + . . . . . . . . + O O + .",
	". . + O O O O O + . . . . . . . . + O O + . .",
	". . + O O O O + + . . . . . + + + O O O + . .",
	". . . + O O O O O + + + + + O O O O O + . . .",
	". . . . + + O O O O O O O O O O O + + . . . .",
	". . . . . . + + O O O O O O O + + . . . . . .",
	". . . . . . . . + + + + + + + . . . . . . . .",
	0
};

static const char * cursor_draw_line_data[] =
{
	". . . . . + + + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	"+ O O O O . . . O O O O + . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . O O .",
	". . . . . + + + . . . . . . . . O O + + .",
	". . . . . . . . . . . . . . O O + + . . .",
	". . . . . . . . . . . . O O + + . . . . .",
	". . . . . . . . . . O O + + . . . . . . .",
	". . . . . . . . O O + + . . . . . . . . .",
	". . . . . . O O + + . . . . . . . . . . .",
	". . . . . . + + . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};

static const char * cursor_draw_rect_data[] =
{
	". . . . . + + + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	"+ O O O O . . . O O O O + . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . + + + + + + + + + . .",
	". . . . . + O + . . + O O O O O O O + . .",
	". . . . . + + + . . + O . . . . . O + . .",
	". . . . . . . . . . + O . . . . . O + . .",
	". . . . . . . . . . + O . . . . . O + . .",
	". . . . . . . . . . + O . . . . . O + . .",
	". . . . . . . . . . + O O O O O O O + . .",
	". . . . . . . . . . + + + + + + + + + . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};

static const char * cursor_draw_circle_data[] =
{
	". . . . . + + + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	"+ O O O O . . . O O O O + . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . + + + + . . . .",
	". . . . . + O + . . . . + O O O O + . . .",
	". . . . . + O + . . . + O . . . . O + . .",
	". . . . . + + + . . + O . . . . . . O + .",
	". . . . . . . . . . + O . . . . . . O + .",
	". . . . . . . . . . + O . . . . . . O + .",
	". . . . . . . . . . + O . . . . . . O + .",
	". . . . . . . . . . . + O . . . . O + . .",
	". . . . . . . . . . . . + O O O O + . . .",
	". . . . . . . . . . . . . + + + + . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};

static const char * cursor_dropper_data[] =
{
	". . . . . . . . . . . O O O . . . . . . .",
	". . . . . . . . . . O + + + O . . . . . .",
	". . . . . . . . . . O + + + O . . . . . .",
	". . . . . . . . . O + + + + O . . . . . .",
	". . . . . . . . O + O + O O . . . . . . .",
	". . . . . . . O + + + O . . . . . . . . .",
	". . . . . . O + + + O . . . . . . . . . .",
	". . . . . O + + + O . . . . . . . . . . .",
	". . . . O + + + O . . . . . . . . . . . .",
	". . . O + + + O . . . . . . . . . . . . .",
	". . . O + + O . . . . . . . . . . . . . .",
	". . O + O O . . . . . . . . . . . . . . .",
	". . O O . . . . . . . . . . . . . . . . .",
	". O . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};

static const char * cursor_draw_select_data[] =
{
	". . . . . + + + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	"+ O O O O . . . O O O O + . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . . . . . . . . . . . .",
	". . . . . + O + . . + O + O + O + O + . .",
	". . . . . + O + . . O . . . . . . . O . .",
	". . . . . + + + . . + . . . . . . . + . .",
	". . . . . . . . . . O . . . . . . . O . .",
	". . . . . . . . . . + . . . . . . . + . .",
	". . . . . . . . . . O . . . . . . . O . .",
	". . . . . . . . . . + . . . . . . . + . .",
	". . . . . . . . . . O + O + O + O + O . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};


static const char * cursor_wand_data[] =
{
	". . . . . + + + . . . . . . . . . . . . .",
	". . + . . + O + . . + . . . . . . . . . .",
	". + O + . + O + . + O + . . . . . . . . .",
	". . + O + + O + + O + . . . . . . . . . .",
	". . . + O + O + O + . . . . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	"+ O O O O . . . O O O O + . . . . . . . .",
	"+ + + + + . . . + + + + + . . . . . . . .",
	". . . + O + O + + O . . . . . . . . . . .",
	". . + O + + O + O + O . . . . . . . . . .",
	". + O + . + O + . O + O . . . . . . . . .",
	". . + . . + O + . . O + O . . . . . . . .",
	". . . . . + + + . . . O + O . . . . . . .",
	". . . . . . . . . . . . O + O . . . . . .",
	". . . . . . . . . . . . . O + O . . . . .",
	". . . . . . . . . . . . . . O . . . . . .",
	0
};

static const char * cursor_bucket_data[] =
{

	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . O O O . . . . . .",
	". . . . . . . . . . O O + + + O . . . . .",
	". . . . . . . . O O + + + + + + O . . . .",
	". . . . . . . O + + + + + + + + + O . . .",
	". . . . . . O + + + + + + + + + + O . . .",
	". . . . . O + O O O + + + + + + + O . . .",
	". . . . O + O . . . O + + + + + + O . . .",
	". . . . O + . . . . O + + + + + + O . . .",
	". . . . . + O O . . O + + + + + O . . . .",
	". . . . . O O O O . O + + + O O . . . . .",
	". . . . O O O + O . + + O O . . . . . . .",
	". . . . O O O . + + O O . . . . . . . . .",
	". . . O O . . . O O . . . . . . . . . . .",
	". . . O . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". + O + . . . . . . . . . . . . . . . . .",
	"+ + O + + . . . . . . . . . . . . . . . .",
	"O O . O O . . . . . . . . . . . . . . . .",
	"+ + O + + . . . . . . . . . . . . . . . .",
	". + O + . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	". . . . . . . . . . . . . . . . . . . . .",
	0
};

/** Create a graphic, given its character representation 
 *
 *	@param lines	Lines of text, NULL at line end
 *	@returns		Pointer to the new graphic
 */

GRAPH * gui_graphic (const char ** lines, int allblack)
{
	int h = 0;
	int w = 0;
	GRAPH * graph;
	const char * ptr;
	short * cptr;

	int white = gr_rgb (255, 255, 255);
	int black = gr_rgb (  0,   0,   0);

	if (allblack) white = black;

	for (h = 0 ; lines[h] ; h++)
	{
		int cw = 0;
		ptr = lines[h];
		while (*ptr)
		{
			if (*ptr != ' ')
				cw++;
			ptr++;
		}
		if (cw > w)
			w = cw;
	}

	graph = bitmap_new (0, w, h, 16);
	cptr = (short *)graph->data;

	for (h = 0 ; lines[h] ; h++)
	{
		short * cptr = (short *)graph->data + h*graph->pitch/2;
		ptr = lines[h];

		while (*ptr)
		{
			if (*ptr == '.')
				*cptr = 0;
			if (*ptr == '+')
				*cptr = white;
			if (*ptr == 'O')
				*cptr = black;
			if (*ptr != ' ')
				cptr++;
			ptr++;
		}
	}
	return graph;
}

/** Create all the default library graphics
 */

void gui_create_cursors()
{
	cursors[POINTER_ARROW]= gui_graphic (cursor_arrow_data, 0);
	cursors[POINTER_RESIZEH] = gui_graphic (cursor_resizeh_data, 0);
	cursors[POINTER_RESIZEV] = gui_graphic (cursor_resizev_data, 0);
	cursors[POINTER_RESIZENW] = gui_graphic (cursor_resizenw_data, 0);
	cursors[POINTER_CROSS] = gui_graphic (cursor_cross_data, 0);
	cursors[POINTER_IBEAM] = gui_graphic (cursor_ibeam_data, 0);
	cursors[POINTER_RESIZEH2] = gui_graphic (cursor_resizeh2_data, 0);
	cursors[POINTER_HAND] = gui_graphic (cursor_hand_data, 0);
	cursors[POINTER_DRAGOBJECT] = gui_graphic (cursor_drag_data, 0);
	cursors[POINTER_ARROWCOPY] = gui_graphic (cursor_arrowcopy_data, 0);
	cursors[POINTER_BADDROP] = gui_graphic (cursor_baddrop_data, 0);
	cursors[POINTER_ZOOM] = gui_graphic (cursor_zoom_data, 0);
	cursors[POINTER_DRAWLINE] = gui_graphic (cursor_draw_line_data, 0);
	cursors[POINTER_DRAWRECT] = gui_graphic (cursor_draw_rect_data, 0);
	cursors[POINTER_DRAWCIRCLE] = gui_graphic (cursor_draw_circle_data, 0);
	cursors[POINTER_DRAWSELECT] = gui_graphic (cursor_draw_select_data, 0);
	cursors[POINTER_DROPPER] = gui_graphic (cursor_dropper_data, 0);
	cursors[POINTER_WAND] = gui_graphic (cursor_wand_data, 0);
	cursors[POINTER_BUCKET] = gui_graphic (cursor_bucket_data, 0);
	
	cursors_black[POINTER_ARROW]= gui_graphic (cursor_arrow_data, 1);
	cursors_black[POINTER_RESIZEH] = gui_graphic (cursor_resizeh_data, 1);
	cursors_black[POINTER_RESIZEV] = gui_graphic (cursor_resizev_data, 1);
	cursors_black[POINTER_RESIZENW] = gui_graphic (cursor_resizenw_data, 1);
	cursors_black[POINTER_CROSS] = gui_graphic (cursor_cross_data, 1);
	cursors_black[POINTER_IBEAM] = gui_graphic (cursor_ibeam_data, 1);
	cursors_black[POINTER_RESIZEH2] = gui_graphic (cursor_resizeh2_data, 1);
	cursors_black[POINTER_HAND] = gui_graphic (cursor_hand_data, 1);
	cursors_black[POINTER_DRAGOBJECT] = gui_graphic (cursor_drag_data, 1);
	cursors_black[POINTER_ARROWCOPY] = gui_graphic (cursor_arrowcopy_data, 1);
	cursors_black[POINTER_BADDROP] = gui_graphic (cursor_baddrop_data, 1);

	bitmap_add_cpoint (cursors[POINTER_ARROW], 0, 0);
	bitmap_add_cpoint (cursors[POINTER_ARROWCOPY], 0, 0);
	bitmap_add_cpoint (cursors[POINTER_ZOOM], 3, 3);
	bitmap_add_cpoint (cursors[POINTER_CROSS], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_DRAWLINE], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_DRAWRECT], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_DRAWCIRCLE], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_DRAWSELECT], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_DROPPER], 2, 14);
	bitmap_add_cpoint (cursors[POINTER_WAND], 5, 5);
	bitmap_add_cpoint (cursors[POINTER_BUCKET], 3, 18);
	bitmap_add_cpoint (cursors_black[POINTER_ARROW], 0, 0);
	bitmap_add_cpoint (cursors_black[POINTER_ARROWCOPY], 0, 0);
}
