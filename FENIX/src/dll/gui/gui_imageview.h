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
 * FILE        : gui_imageview.h
 * DESCRIPTION : Graphic User Interface DLL header file
 */

#include "gui.h"

#ifndef __GUI_IMAGEVIEW_H
#define __GUI_IMAGEVIEW_H

/* ------------------------------------------------------------------------- * 
 *  IMAGE VIEW
 * ------------------------------------------------------------------------- */

#define IMAGEVIEWBG_TRANSPARENT		0
#define IMAGEVIEWBG_BLACK			1
#define IMAGEVIEWBG_CHESSBOARD		2
#define IMAGEVIEWBG_GRAY      		3
#define IMAGEVIEWBG_WINDOW          4

typedef struct _imageview
{
	CONTROL		control;			/*< Parent class data */
	GRAPH *		graph;				/*< Graphic to be shown */
	int			fpg;				/*< Library number of the graphic */
	int			graphid;			/*< Identifier of the graphic */
	int			aspect;				/*< 1 to always preserve aspect ratio */
	int			scale;				/*< 1 to scale the image to control size */
	int			bg;					/*< IMAGEVIEWBG_* constant */
	float		zoom;				/*< Zoom level (1.0f for no zoom) */
}
IMAGEVIEW;

extern CONTROL * gui_imageview_new (GRAPH * graph);
extern CONTROL * gui_imageview_new_fpg (int fpg, int graphic);
extern void      gui_imageview_zoom (CONTROL * imageview, float zoom);
extern void      gui_imageview_background (CONTROL * imageview, int bg);
extern void      gui_imageview_scale (CONTROL * imageview, int resize, int scale, int aspect);
extern void      gui_imageview_image (CONTROL * imageview, GRAPH * graph);
extern void      gui_imageview_image_fpg (CONTROL * imageview, int fpg, int graphid);

#endif
