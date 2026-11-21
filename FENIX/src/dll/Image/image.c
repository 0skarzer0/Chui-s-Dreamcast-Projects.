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
 * FILE        : image.c
 * DESCRIPTION : sdlf_Image Fenix support DLL
 */

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif

#include <fxdll.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

#include <SDL_image.h>

static int sdlf_seek (SDL_RWops * context, int offset, int whence);
static int sdlf_read (SDL_RWops * context, void * ptr, int size, int count);
static int sdlf_write (SDL_RWops * context, const void * ptr, int size, int count);
static int sdlf_close (SDL_RWops * context);

enum 
{
	TYPE_BMP = 0x001,
	TYPE_GIF = 0x002,
	TYPE_JPG = 0x004,
	TYPE_LBM = 0x008,
	TYPE_PCX = 0x010,
	TYPE_PNG = 0x020,
	TYPE_PNM = 0x040,
	TYPE_TIF = 0x080,
	TYPE_XPM = 0x100,
	TYPE_XCF = 0x200
};

/*
 *  FUNCTION : file_RWops
 *
 *  Given a Fenix file object, create a SDL_RWops for SDL
 *
 *  PARAMS : 
 *		file			Pointer to the file object
 *
 *  RETURN VALUE : 
 *      Pointer to the SDL_RWops struct
 *
 */

SDL_RWops * file_RWops (file * fp)
{
	static SDL_RWops * sdlf;

	sdlf = SDL_AllocRW();
	if (sdlf == NULL) return NULL;

	sdlf->seek = sdlf_seek;
	sdlf->read = sdlf_read;
	sdlf->write = sdlf_write;
	sdlf->close = sdlf_close;
	sdlf->hidden.unknown.data1 = fp;

	return sdlf;
}

/* Stub functions for RWops */

static int sdlf_seek (SDL_RWops * context, int offset, int whence)
{
	file * fp = (file *) context->hidden.unknown.data1;
	file_seek (fp, offset, whence);
	return file_pos (fp);
}

static int sdlf_read (SDL_RWops * context, void * ptr, int size, int count)
{
	file * fp = (file *) context->hidden.unknown.data1;
	if (count == 0) return -1;
	return file_read (fp, ptr, size * count) / size;
}

static int sdlf_write (SDL_RWops * context, const void * ptr, int size, int count)
{
	file * fp = (file *) context->hidden.unknown.data1;
	if (count == 0) return -1;
	return file_write (fp, ptr, size * count) / size;
}

static int sdlf_close (SDL_RWops * context)
{
	/* We don't close the file here with file_close, because the SDL_RWops
	 * is created from an existing file * pointer that could be closed
	 * elsewhere! */
	return 1;
}

/*
 *  FUNCTION : gr_fromsurface
 *
 *  Create a GRAPH image from a SDL_Surface object
 *
 *  PARAMS : 
 *		surface			Pointer to the SDL_Surface object
 *
 *  RETURN VALUE : 
 *      Pointer to the new GRAPH object
 *
 */

GRAPH * gr_fromsurface (SDL_Surface * surface)
{
	GRAPH * gr;
	int     x, y;

	SDL_Surface * c;

	if (surface == NULL) return NULL;

	if (!scr_initialized) gr_init(320, 200);

	if (SDL_MUSTLOCK(surface))
	{
		SDL_LockSurface (surface);
		if (surface->pixels == 0)
			return NULL;
	}

	/* The surface has 8 bits depth */

	if (surface->format->BitsPerPixel == 8)
	{
		/* Load the palette if needed */

		gr = bitmap_new_syslib (surface->w, surface->h, 8);
		if (gr != NULL) 
		{
			if (!palette_loaded && surface->format->palette)
			{
				for (x = 1 ; x < surface->format->palette->ncolors ; x++)
					gpalette[x] = surface->format->palette->colors[x];
				if (surface->format->colorkey)
					gpalette[surface->format->colorkey & 0xFF] = surface->format->palette->colors[0];
				palette_loaded  = 1;
				palette_changed = 1;
			}

			/* Convert colorkey information to preserve transparency */
			
			if (surface->format->colorkey)
			{
				for (y = 0 ; y < gr->height ; y++)
				{
					Uint8 * ptr = (Uint8 *)surface->pixels + surface->pitch * y;
					
					for (x = 0 ; x < gr->width ; x++, ptr++)
					{
						if (*ptr == surface->format->colorkey)
							*ptr = 0;
						else if (*ptr == 0)
							*ptr = surface->format->colorkey;
					}
				}
			}
			else 
			{
				/* No transparency */
				Uint8 color = 255;

				if (surface->format->palette)
				{
					color = gr_find_nearest_color (
						surface->format->palette->colors[0].r,
						surface->format->palette->colors[0].g,
						surface->format->palette->colors[0].b);	
				}
				for (y = 0 ; y < gr->height ; y++)
				{
					Uint8 * ptr = (Uint8 *)surface->pixels + surface->pitch * y;
					
					for (x = 0 ; x < gr->width ; x++, ptr++)
						if (*ptr == 0) *ptr = color;
				}
			}
			
			/* Copy the data */
			
			for (y = 0 ; y < gr->height ; y++)
			{
				memcpy ((Uint8 *) gr->data + gr->pitch * y,
					(Uint8 *) surface->pixels + gr->pitch * y,
					gr->width);
			}
		}

		if (SDL_MUSTLOCK(surface))
			SDL_UnlockSurface(surface);
		return gr;
	}

	/* The surface has some other bit depth */

	if (screen->format->BitsPerPixel != 16)
	{
		gr_error ("gr_fromsurface: intento de abrir una imagen de 16 o más bits en un modo de 8");
		return NULL;
	}

	c = surface ;

	if (surface->format->BitsPerPixel != 16 ||
		surface->format->Rmask != screen->format->Rmask ||
		surface->format->Gmask != screen->format->Gmask ||
		surface->format->Bmask != screen->format->Bmask)
	{
		c = SDL_ConvertSurface (surface, screen->format, 0);
		if (c == NULL)
		{
			gr_error ("gr_fromsurface: error al convertir la imagen");
			return NULL;
		}
	}

	/* Copy the data to a new graphic */

	gr = bitmap_new_syslib (surface->w, surface->h, 16);
	if (gr != NULL) 
	{
		for (y = 0 ; y < gr->height ; y++)
		{
			memcpy ((Uint8 *) gr->data  + gr->pitch * y,
					(Uint8 *) c->pixels + c->pitch * y,
					gr->widthb);
		}

		/* Import transparency information from the original image, if any */

		if (surface->format->BitsPerPixel == 32 &&
			surface->format->Amask != 0)
		{
			/* 32 bits with alpha channel */
			int mask = 0x80000000;
			while (mask && !(surface->format->Amask & mask)) mask >>= 1;

			for (y = 0 ; y < gr->height ; y++)
			{
				Uint16 * ptr = (Uint16 *)gr->data + gr->pitch * y / 2;
				Uint32 * tex = (Uint32 *)surface->pixels + surface->pitch * y / 4;
				
				for (x = 0 ; x < gr->width ; x++, ptr++, tex++)
				{
					if (!(*tex & mask)) *ptr = 0;
					else if (*ptr == 0) *ptr = 1;
				}
			}
		}
		else if (surface->format->BitsPerPixel == 32 &&
			surface->format->Amask != 0)
		{
			/* 16 bits with alpha channel */
			int mask = 0x8000;
			while (mask && !(surface->format->Amask & mask)) mask >>= 1;

			for (y = 0 ; y < gr->height ; y++)
			{
				Uint16 * ptr = (Uint16 *)gr->data + gr->pitch * y / 2;
				Uint16 * tex = (Uint16 *)surface->pixels + surface->pitch * y / 2;
				
				for (x = 0 ; x < gr->width ; x++, ptr++, tex++)
				{
					if (!(*tex & mask)) *ptr = 0;
					else if (*ptr == 0) *ptr = 1;
				}
			}
		}
		else
		{
			/* No transparency */
			for (y = 0 ; y < gr->height ; y++)
			{
				Uint16 * ptr = (Uint16 *)gr->data + gr->pitch * y / 2;
				
				for (x = 0 ; x < gr->width ; x++, ptr++)
					if (*ptr == 0) *ptr = 1;
			}
		}
	}

	if (c != surface)
		SDL_FreeSurface(c);
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	return gr;
}

/*
 *  FUNCTION : gr_load_image
 *
 *  Load an image using SDL_image functions
 *
 *  PARAMS : 
 *		filename		File name
 *
 *  RETURN VALUE : 
 *      ID of the new image or 0 if error
 *
 */

int gr_load_image (const char * filename)
{
	file        * fp;
	GRAPH       * gr = NULL;
	SDL_RWops   * rwops;
	SDL_Surface * s;

	/* Some image loaders may not be compatible with the file_* functions
	 * (for example, back-seeks in compressed gzip files are not supported) */
	s = IMG_Load (filename);

	/* Try to open compressed/packed files */
	if (s == NULL)
	{
		fp = file_open (filename, "rb");
		if (fp == NULL)
			return 0;
		rwops = file_RWops (fp);
		if (rwops != NULL)
			s = IMG_Load_RW (rwops, 1);
		file_close(fp);
	}
	
	/* Convert to graph */
	if (s != NULL)
	{
		gr = gr_fromsurface (s);
		SDL_FreeSurface(s);
	}

	return gr != NULL ? gr->code : 0;
}

/*
 *  FUNCTION : gr_load_image_pal
 *
 *  Load the palette of an image
 *
 *  PARAMS : 
 *		filename		File name
 *
 *  RETURN VALUE : 
 *      ID of the new image or 0 if error
 *
 */

int gr_load_image_pal (const char * filename)
{
	file        * fp;
	GRAPH       * gr = NULL;
	SDL_RWops   * rwops;
	SDL_Surface * s;
	int           x;

	/* Some image loaders may not be compatible with the file_* functions
	 * (for example, back-seeks in compressed gzip files are not supported) */
	s = IMG_Load (filename);

	/* Try to open compressed/packed files */
	if (s == NULL)
	{
		fp = file_open (filename, "rb");
		if (fp == NULL)
			return 0;
		rwops = file_RWops (fp);
		if (rwops != NULL)
			s = IMG_Load_RW (rwops, 1);
		file_close(fp);
	}
	
	/* Convert to graph */
	if (s != NULL)
	{
		if (s->format->palette)
		{
			for (x = 1 ; x < s->format->palette->ncolors ; x++)
				gpalette[x] = s->format->palette->colors[x];
			if (s->format->colorkey)
				gpalette[s->format->colorkey & 0xFF] = s->format->palette->colors[0];
			palette_loaded  = 1;
			palette_changed = 1;
		}
		gr = gr_fromsurface (s);
		SDL_FreeSurface(s);
	}

	return gr != NULL ? gr->code : 0;
}

/*
 *  FUNCTION : gr_image_type
 *
 *  Check the image type of a file using the SDL_image functions
 *
 *  PARAMS : 
 *		filename		File name
 *		type			Type of the file, from the IMAGETYPES enum
 *
 *  RETURN VALUE : 
 *      1 if the image has such a type, 0 otherwise
 */

int gr_image_type (const char * filename, int type)
{
	file        * fp = file_open (filename, "rb");
	GRAPH       * gr = NULL;
	SDL_RWops   * rwops;
	int           result = 0;

	if (fp == NULL)
		return 0;
	rwops = file_RWops (fp);
	if (rwops != NULL)
	{
		if (type & TYPE_BMP) result |= IMG_isBMP(rwops) ? 1:0;
		if (type & TYPE_PNM) result |= IMG_isPNM(rwops) ? 1:0;
		if (type & TYPE_XPM) result |= IMG_isXPM(rwops) ? 1:0;
		if (type & TYPE_XCF) result |= IMG_isXCF(rwops) ? 1:0;
		if (type & TYPE_PCX) result |= IMG_isPCX(rwops) ? 1:0;
		if (type & TYPE_GIF) result |= IMG_isGIF(rwops) ? 1:0;
		if (type & TYPE_JPG) result |= IMG_isJPG(rwops) ? 1:0;
		if (type & TYPE_TIF) result |= IMG_isTIF(rwops) ? 1:0;
		if (type & TYPE_PNG) result |= IMG_isPNG(rwops) ? 1:0;
		if (type & TYPE_LBM) result |= IMG_isLBM(rwops) ? 1:0;

		SDL_FreeRW(rwops);
	}
	file_close(fp);
	return result;
}

/* --------------------------- Fenix functions --------------------------- */

/*
 *  LOAD_IMAGE (STRING file)
 */
int fxi_load_image (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_load_image(ptr);
	string_discard(params[0]);
	return result;
}

/*
 *  LOAD_IMAGE_PAL (STRING file)
 */
int fxi_load_image_pal (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_load_image_pal(ptr);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PCX (STRING file)
 */
int fxi_is_PCX (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PCX);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_GIF (STRING file)
 */
int fxi_is_GIF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_GIF);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_JPG (STRING file)
 */
int fxi_is_JPG (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_JPG);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PNG (STRING file)
 */
int fxi_is_PNG (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PNG);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_XPM (STRING file)
 */
int fxi_is_XPM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_XPM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_BMP (STRING file)
 */
int fxi_is_BMP (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_BMP);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_PNM (STRING file)
 */
int fxi_is_PNM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_PNM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_TIF (STRING file)
 */
int fxi_is_TIF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_TIF);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_LBM (STRING file)
 */
int fxi_is_LBM (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_LBM);
	string_discard(params[0]);
	return result;
}

/*
 *  IS_XCF (STRING file)
 */
int fxi_is_XCF (INSTANCE * i, int * params)
{
	const char * ptr = string_get(params[0]);
	int result = gr_image_type(ptr, TYPE_XCF);
	string_discard(params[0]);
	return result;
}


FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport
		
	FENIX_export ("LOAD_IMAGE", "S", TYPE_DWORD, fxi_load_image );
	FENIX_export ("LOAD_IMAGE_PAL", "S", TYPE_DWORD, fxi_load_image_pal );
	FENIX_export ("IS_BMP", "S", TYPE_DWORD, fxi_is_BMP);
	FENIX_export ("IS_GIF", "S", TYPE_DWORD, fxi_is_GIF);
	FENIX_export ("IS_JPG", "S", TYPE_DWORD, fxi_is_JPG);
	FENIX_export ("IS_LBM", "S", TYPE_DWORD, fxi_is_LBM);
	FENIX_export ("IS_PCX", "S", TYPE_DWORD, fxi_is_PCX);
	FENIX_export ("IS_PNG", "S", TYPE_DWORD, fxi_is_PNG);
	FENIX_export ("IS_PNM", "S", TYPE_DWORD, fxi_is_PNM);
	FENIX_export ("IS_TIF", "S", TYPE_DWORD, fxi_is_TIF);
	FENIX_export ("IS_XCF", "S", TYPE_DWORD, fxi_is_XCF);
	FENIX_export ("IS_XPM", "S", TYPE_DWORD, fxi_is_XPM);

	if (_fnc_export) 
	{
		fnc_export ("file_RWops", file_RWops);
		fnc_export ("gr_fromsurface", gr_fromsurface);
		fnc_export ("gr_load_image", gr_load_image);
	}
}
