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
 * FILE        : ef_depth.c
 * DESCRIPTION : Graphic effects: depth change
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#define MULTIPLE_FILES
#include <fxdll.h>

#include <assert.h>

/** Changes the bit depth of a map. Current conversions supported
  * are 8 to 16 bits and 16 to 8 bits.
  *
  *	@param graph		Original graphic
  */

void bitmap_setdepth (GRAPH * graph, int newdepth)
{
	int x, y;
	GRAPH * result;
	void * old;

	if (graph->depth == 8 && newdepth == 16)
	{
		result = bitmap_new (0, graph->width, graph->height, 16);

		for (y = 0 ; y < graph->height ; y++)
		{
			Uint16 * dst = (Uint16*)result->data + result->pitch*y/2;
			Uint8  * src = (Uint8 *)graph->data + graph->pitch*y;

			for (x = graph->width ; x ; x--, dst++, src++)
				*dst = colorequiv[*src];
		}

		graph->pitch = result->pitch;
		graph->depth = 16;
		old = graph->data;
		graph->data = result->data;
		result->data = old;
		bitmap_destroy(result);
	}
	else if (graph->depth == 16 && newdepth == 8)
	{
		result = bitmap_new (0, graph->width, graph->height, 16);

		for (y = 0 ; y < graph->height ; y++)
		{
			Uint16 * src = (Uint16*)graph->data + graph->pitch*y/2;
			Uint8  * dst = (Uint8 *)result->data + result->pitch*y;

			for (x = graph->width ; x ; x--, dst++, src++)
			{
				int r, g, b;
				gr_get_rgb (*src, &r, &g, &b);
				*dst = gr_find_nearest_color (r, g, b);
			}
		}

		graph->pitch = result->pitch;
		graph->depth = 8;
		old = graph->data;
		graph->data = result->data;
		result->data = old;
		bitmap_destroy(result);
	}
}
