/*
 * Fenix SDL Loader
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "fsl.h"
#include "math-sll.h"

#define STRECTH_INIT	\
	int x=r->x,y=r->y,h=r->h,w=r->w;	\
	sll ox,oy;	\
	if (x<0)	\
	{		\
		ox=int2sll(-x);	\
		w+=x;		\
		x=0;		\
	}			\
	else			\
		ox=0;		\
	if (x < 320)		\
	{			\
		if (x+w > 320)	\
			w=320 - x; 	\
		if (y<0)	\
		{		\
			oy=int2sll(-y);	\
			h+=y;		\
			y=0;		\
		} else			\
			oy=0;		\
		if (y < 240)		\
		{			\
			if (y+h > 240)	\
				h=240 - y;	\
			sll dx=slldiv(int2sll(source->w),int2sll(r->w));	\
			sll dy=slldiv(int2sll(source->h),int2sll(r->h));	\
			ox=sllmul(dx,ox);	\
			oy=sllmul(dy,oy);	

#define STRETCH_LOOP	\
			for(y=0;y<h;y++,dst+=dpit)	\
			{		\
				register int tmp_x;	\
				register sll oox=ox;	

#define STRETCH_END	\
				for(tmp_x=0;tmp_x<w;tmp_x++)	\
				{	\
					dst[tmp_x]=src[sll2int(oox)];	\
					oox=slladd(oox,dx);	\
				}				\
				oy=slladd(oy,dy);		\
			}		\
		}			\
	}

#if 0
void stretch8(SDL_Surface *source, SDL_Rect *r)
{			
	STRECTH_INIT
	Uint8 *dst=fsl_screen->pixels + mul320[y] + x;
	STRETCH_LOOP
	register Uint8 *src=source->pixels + (source->w * sll2int(oy));
	STRETCH_END
}
void stretch32(SDL_Surface *source, SDL_Rect *r)
{			
	STRECTH_INIT
	Uint32 *dst=fsl_screen->pixels + ((mul320[y] + x)<<2);
	STRETCH_LOOP
	register Uint32 *src=source->pixels + ((source->w * sll2int(oy))<<2);
	STRETCH_END
}
#endif




void fsl_stretch16(SDL_Surface *source, SDL_Rect *r)
{
	int pitch=source->w&0x1;
	int dpit=fsl_screen->pitch>>1;
	STRECTH_INIT
	Uint16 *dst=fsl_screen->pixels + ((y*dpit + x)<<1);
	STRETCH_LOOP
	register Uint16 *src=source->pixels + (((source->w+pitch) * sll2int(oy))<<1);
	STRETCH_END
}

