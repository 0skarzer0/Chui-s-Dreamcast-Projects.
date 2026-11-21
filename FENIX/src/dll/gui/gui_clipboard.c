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
 *  Contains all clipboard support functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "gui.h"

char  * clipboard_text = 0;
GRAPH * clipboard_graph = 0;

/** Save some text to the clipboard
 *
 *	@param text			Text to copy
 *  @param size			Number of characters to copy or -1 to use strlen(text)
 */

void gui_copy_text (const char * text, int size)
{
#ifdef WIN32

	HGLOBAL		handle;
	char *      ptr;

	if (size < 0)
		size = strlen(text);

	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		handle = GlobalAlloc(GMEM_MOVEABLE, size+1);
		if (handle)
		{
			ptr = GlobalLock(handle);
			if (ptr)
			{
				memcpy (ptr, text, size);
				ptr[size] = 0;
				GlobalUnlock(handle);
				SetClipboardData (CF_TEXT, handle);
			}
		}
		CloseClipboard();
	}

#else

	if (size < 0)
		size = strlen(text);
	if (clipboard_text)
		free (clipboard_text);
	clipboard_text = malloc(size+1);
	memcpy (clipboard_text, text, size);
	clipboard_text[size] = 0;

#endif
}

/** Retrieve the clipboard text
 *
 *  @return		Current text at the clipboard or NULL if there is none
 */

const char * gui_paste_text()
{
#ifdef WIN32

	if (clipboard_text)
	{
		free (clipboard_text);
		clipboard_text = 0;
	}

	if (OpenClipboard(NULL))
	{
		HGLOBAL handle = GetClipboardData(CF_TEXT);
		if (handle)
		{
			char * ptr = GlobalLock(handle);
			if (ptr)
			{
				clipboard_text = strdup(ptr);
				GlobalUnlock(handle);
			}
		}
		CloseClipboard();
	}

#endif

	return clipboard_text;
}

/** Save a graphic to the clipboard
 *
 *	@param graph		Graph to copy
 */

void gui_copy_graph (GRAPH * graph)
{
#ifdef WIN32

	BITMAPINFO *bmi;
	HANDLE		global;
	int			x, y, c;
	int			r, g, b;
	int			size;
	
	unsigned char * buffer;
	unsigned char * ptr;

	/* Calculate size to allocate */

	size = sizeof(BITMAPINFO);
	if (graph->depth == 16)
		size += graph->width * graph->height * 4;
	else if (graph->depth == 8)
		size += graph->width * graph->height + 1024;
	else
		return;

	if (!OpenClipboard(NULL))
		return;

	/* Fill bitmap information */

	global = GlobalAlloc (GMEM_MOVEABLE, size);
	buffer = GlobalLock(global);
	bmi = (BITMAPINFO *)buffer;
	ptr = buffer + sizeof(BITMAPINFOHEADER);
	bmi->bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi->bmiHeader.biWidth = graph->width;
	bmi->bmiHeader.biHeight = graph->height;
	bmi->bmiHeader.biPlanes = 1;
	bmi->bmiHeader.biCompression = BI_RGB;
	bmi->bmiHeader.biSizeImage = 0;
	bmi->bmiHeader.biXPelsPerMeter = 0;
	bmi->bmiHeader.biYPelsPerMeter = 0;
	bmi->bmiHeader.biClrImportant = 0;

	/* Fill bitmap pixel data */

	if (graph->depth == 16)
	{
		bmi->bmiHeader.biBitCount = 24;
		bmi->bmiHeader.biClrUsed = 0;

		for (y = graph->height-1 ; y >= 0 ; y--)
		{
			for (x = 0 ; x < graph->width ; x++)
			{
				c = gr_get_pixel (graph, x, y);
				gr_get_rgb (c, &r, &g, &b);
				*ptr++ = g;
				*ptr++ = r;
				*ptr++ = b;
			}
			while ((3*x) & 3)
			{
				*ptr++ = 0;
				x++;
			}
		}
	}
	else if (graph->depth == 8)
	{
		bmi->bmiHeader.biBitCount = 8;
		bmi->bmiHeader.biClrUsed = 256;

		ptr += 1024-4;

		for (x = 0 ; x < 256 ; x++)
		{
			bmi->bmiColors[x].rgbBlue  = gpalette[x].b;
			bmi->bmiColors[x].rgbGreen = gpalette[x].g;
			bmi->bmiColors[x].rgbRed   = gpalette[x].r;
		}

		for (y = graph->height-1 ; y >= 0 ; y--)
		{
			for (x = 0 ; x < graph->width ; x++)
			{
				*ptr++ = gr_get_pixel (graph, x, y);
			}
			while (x & 3)
			{
				*ptr++ = 0;
				x++;
			}		
		}
	}

	/* Save to the clipboard */

	GlobalUnlock(global);
	EmptyClipboard();
	SetClipboardData (CF_DIB, global);
	CloseClipboard();

#else

	if (clipboard_graph)
		bitmap_destroy (clipboard_graph);
	clipboard_graph = bitmap_clone(graph);

#endif
}

/** Create a graphic using the clipboard data
 *
 *  @return		Pointer to the new graphic or NULL if clipboard is empty
 */

GRAPH * gui_paste_graph()
{
#ifdef WIN32

	HGLOBAL		 handle;
	BITMAPINFO * bmi;
	char *		 buffer;
	GRAPH *		 graph = 0;
	char *		 ptr;
	char *		 destptr;
	int			 x, y;
	int			 pitch;

	if (OpenClipboard(NULL))
	{
		handle = GetClipboardData(CF_DIB);
		
		if (handle)
		{
			buffer = GlobalLock(handle);
			bmi = (BITMAPINFO *)buffer;

			pitch = bmi->bmiHeader.biWidth * bmi->bmiHeader.biBitCount / 8;
			//if (pitch & 1) pitch++;

			switch (bmi->bmiHeader.biBitCount)
			{
				case 24:

					graph = bitmap_new_syslib (bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight, 16);
					destptr = (char *)graph->data;

					for (y = 0 ; y < graph->height ; y++)
					{
						ptr = buffer + bmi->bmiHeader.biSize + (graph->height-y-1) * pitch;

						for (x = 0 ; x < graph->width ; x++)
						{
							*(Uint16 *)destptr = gr_rgb (ptr[1], ptr[0], ptr[2]);
							destptr += 2;
							ptr += 3;
						}
						while ((x*3) & 3) ptr++, x++;
					}
					break;

				case 8:

					graph = bitmap_new_syslib (bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight, 8);
					destptr = (char *)graph->data;

					for (y = 0 ; y < graph->height ; y++)
					{
						ptr = buffer + bmi->bmiHeader.biSize + 1024 + (graph->height-y-1) * pitch;
						memcpy (destptr, ptr, graph->width);
						destptr += graph->pitch;
						if (graph->width & 3)
							ptr += (graph->width & ~3) + 4;
						else
							ptr += graph->width;
					}
					for (x = 0 ; x < 256 ; x++)
					{
						Uint8 pal[3] = {
							bmi->bmiColors[x].rgbRed,
							bmi->bmiColors[x].rgbGreen,
							bmi->bmiColors[x].rgbBlue
						};
						gr_set_colors (x, 1, pal);
					}
					break;
			}

			GlobalUnlock(handle);
		}
		CloseClipboard();
	}
	return graph;

#else

	if (clipboard_graph == NULL)
		return NULL;
	return bitmap_clone(clipboard_graph);

#endif
}
