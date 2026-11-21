/*
 *  Fenix - Videogame compiler/interpreter
 *  Current release       : FENIX - PROJECT 1.0 - R 0.82
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
 * FILE        : ttf.c
 * DESCRIPTION : Truetype support DLL
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fxdll.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

/*
 *  FUNCTION : gr_load_ttf
 *
 *  Load a Truetype font, using Freetype. The font can be produced in
 *  1, 8 or 16 bit depth. A 8 bpp font uses the current palette.
 *
 *  PARAMS : 
 *		filename		Name of the TTF file
 *		size			Size of the required font (height in pixels)
 *      bpp             Depth of the resulting font (1, 8 or 16)
 *      fg              Foreground color (for 8 or 16 bpp)
 *      bg              Background color (for 8 or 16 bpp)
 *
 *  RETURN VALUE : 
 *      ID of the font if succeded or -1 otherwise
 *
 */

static FT_Library freetype;
static int        freetype_initialized = 0;
static FT_Face    face;
static char       facename[2048] = "";
static char *     facedata = 0;

int gr_load_ttf (const char * filename, int size, int bpp, int fg, int bg)
{
	FONT * font;
	file * fp;
	long   allocated;
	long   readed;
	char * buffer;
	int    id;
	int    i, x, y;
	int    maxyoffset = 0;
	int    error;
	int    ok = 0;

	/* Calculate the color equivalences */

	static Uint16 equiv[256];

	if (bpp == 8 || bpp == 16)
	{
		int r1, g1, b1;
		int r2, g2, b2;

		gr_get_rgb (bg, &r1, &g1, &b1);
		gr_get_rgb (fg, &r2, &g2, &b2);

		if (bpp == 8)
		{
			for (i = 0 ; i < 256 ; i++)
				equiv[i] = gr_find_nearest_color (
					r1 + ( (i*(r2-r1)) >> 8 ),
					g1 + ( (i*(g2-g1)) >> 8 ),
					b1 + ( (i*(b2-b1)) >> 8 ));
		}
		else
		{
			for (i = 0 ; i < 256 ; i++)
				equiv[i] = gr_rgb (
					r1 + ( (i*(r2-r1)) >> 8 ),
					g1 + ( (i*(g2-g1)) >> 8 ),
					b1 + ( (i*(b2-b1)) >> 8 ));
		}
	}

	/* Open the file */

	fp = file_open (filename, "rb");
	if (!fp) 
	{
		gr_error ("gr_load_ttf: imposible abrir %s", filename);
		return -1;
	}
	allocated = 4096;
	readed = 0;
	buffer = malloc(allocated);
	if (buffer == NULL)
	{
		gr_error ("gr_load_ttf: sin memoria");
		file_close (fp);
		return -1;
	}

	/* Read the entire file into memory */

	for (;;)
	{
		readed += file_read (fp, buffer+readed, allocated-readed);
		if (readed < allocated)
			break;

		allocated += 4096;
		buffer = realloc (buffer, allocated);
		if (buffer == NULL)
		{
			gr_error ("gr_load_ttf: sin memoria");
			file_close (fp);
			return -1;
		}
	}
	file_close(fp);

	/* Initialize Freetype */

	if (freetype_initialized == 0)
	{
		error = FT_Init_FreeType(&freetype);
		if (error)
		{
			gr_error ("gr_load_ttf: error al inicializar Freetype");
			free (buffer);
			return -1;
		}
		freetype_initialized = 1;
	}

	/* Load the font file */

	if (strncmp (facename, filename, 1024) != 0)
	{
		if (facedata) free(facedata);
		error = FT_New_Memory_Face (freetype, buffer, readed, 0, &face);
		if (error)
		{
			if (error == FT_Err_Unknown_File_Format)
				gr_error ("gr_load_ttf: %s no es una fuente Truetype válida", filename);
			else
				gr_error ("gr_load_ttf: error al recuperar %s", filename) ;
			return -1;
		}
		strncpy (facename, filename, 1024);
		facedata = buffer;
	}

	/* Create the Fenix font */

	id = gr_font_new ();
	if (id < 0) return -1;
	font = gr_font_get(id);
	font->bpp = bpp;
	font->charset = CHARSET_ISO8859;

	/* Retrieve the glyphs */

	FT_Set_Pixel_Sizes (face, 0, size);
	
	if (FT_Select_Charmap (face, ft_encoding_latin_1) != 0 &&
	    FT_Select_Charmap (face, ft_encoding_unicode) != 0 &&
	    FT_Select_Charmap (face, ft_encoding_none) != 0)
	
	{
		if (face->num_charmaps > 0)
			FT_Set_Charmap (face, face->charmaps[0]);
	}

	for (i = 0 ; i < 256 ; i++)
	{
		GRAPH * bitmap;
		int width, height;

		/* Render the glyph */

		error = FT_Get_Char_Index (face, i);
		if (!error) continue;

		error = FT_Load_Glyph (face, error, FT_LOAD_RENDER | 
			(bpp == 1 ? FT_LOAD_MONOCHROME : 0));
		if (error) continue;

		ok++;

		/* Create the bitmap */

		width  = face->glyph->bitmap.width;
		height = face->glyph->bitmap.rows;
		bitmap = bitmap_new (i, width, height, bpp);
		if (bitmap == 0)
			break;
		bitmap_add_cpoint (bitmap, 0, 0);

		if (bpp == 1)
		{
			for (y = 0 ; y < height ; y++)
			{
				memcpy ((Uint8 *)bitmap->data + bitmap->pitch*y,
					face->glyph->bitmap.buffer + face->glyph->bitmap.pitch*y,
					bitmap->widthb);
			}
		}
		else if (bpp == 8)
		{
			Uint8 * ptr = (Uint8 *)bitmap->data;
			Uint8 * gld = face->glyph->bitmap.buffer;

			for (y = 0 ; y < height ; y++)
			{
				for (x = 0 ; x < width ; x++)
				{
					if ((unsigned char)gld[x] > 31)
						ptr[x] = (Uint8)equiv[(unsigned char) gld[x]];
					else
						ptr[x] = 0;
				}

				ptr += bitmap->pitch;
				gld += face->glyph->bitmap.pitch;
			}
		}
		else if (bpp == 16)
		{
			Uint16* ptr = (Uint16*)bitmap->data;
			Uint8 * gld = face->glyph->bitmap.buffer;

			for (y = 0 ; y < height ; y++)
			{
				for (x = 0 ; x < width ; x++)
				{
					if ((unsigned char)gld[x] > 31)
						ptr[x] = equiv[(Uint8) gld[x]];
					else
						ptr[x] = 0;
				}

				ptr += bitmap->pitch / 2;
				gld += face->glyph->bitmap.pitch;
			}
		}

		/* Store the glyph metrics in the font */

		font->glyph[i].xoffset  = face->glyph->bitmap_left;
		font->glyph[i].yoffset  = face->glyph->bitmap_top;
		font->glyph[i].xadvance = face->glyph->advance.x >> 6;
		font->glyph[i].yadvance = face->glyph->advance.y >> 6;
		font->glyph[i].bitmap   = bitmap;

		if (maxyoffset < font->glyph[i].yoffset)
		    maxyoffset = font->glyph[i].yoffset;
	}

	if (ok == 0)
		gr_error ("El tipo de letra %s no contiene caracteres utilizables", filename);

	/* Transform yoffsets */

	for (i = 0 ; i < 256 ; i++)
	{
		if (font->glyph[i].bitmap)
			font->glyph[i].yoffset = maxyoffset - font->glyph[i].yoffset;
	}

	return id ;
}

static int fxi_load_ttf (INSTANCE * my, int * params)
{
	char * text = (char *)string_get (params[0]) ;
	int r = text ? gr_load_ttf (text, params[1], 1, 0, 0) : 0 ;
	string_discard (params[0]) ;
	return r ;
}

static int fxi_load_ttfaa (INSTANCE * my, int * params)
{
	char * text = (char *)string_get (params[0]) ;
	int r = text ? gr_load_ttf (text, params[1], params[2], params[3], params[4]) : 0 ;
	string_discard (params[0]) ;
	return r ;
}

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("LOAD_TTF",   "SI",    TYPE_DWORD, fxi_load_ttf );
	FENIX_export ("LOAD_TTFAA", "SIIII", TYPE_DWORD, fxi_load_ttfaa );

	if (&fnc_export != 0) fnc_export ("gr_load_ttf", gr_load_ttf);
}

