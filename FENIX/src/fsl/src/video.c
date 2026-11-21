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
#include <math.h>

#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 240
#define VIDEO_COLOR_BITS 16
#ifdef DREAMCAST
#define VIDEO_BITS SDL_DOUBLEBUF |SDL_HWSURFACE|SDL_HWPALETTE | SDL_FULLSCREEN
#else
#ifndef FULLSCREEN
#define VIDEO_BITS SDL_DOUBLEBUF |SDL_HWSURFACE|SDL_HWPALETTE
#else
#define VIDEO_BITS SDL_DOUBLEBUF |SDL_HWSURFACE|SDL_HWPALETTE |SDL_FULLSCREEN
#endif
#endif

#define MAX_STARS 96
#define MAX_ACCEL 5

static float star_r[MAX_STARS];
static float star_a[MAX_STARS];
static short star_col[240];


static char* image_names[]={
        DPRE "fsl.png",    // 0
	DPRE "font.png",   // 1
	DPRE "left.png",   // 2
	DPRE "right.png",  // 3
	DPRE "dc.png", 	   // 4
	DPRE "cuadro1.png",// 5
	DPRE "cuadro2.png",// 6 
	DPRE "cuadro3.png",// 7 
	DPRE "cuadro4.png",// 8 
	DPRE "cuadro5.png",// 9 
	DPRE "cuadro_1.png",//10
	DPRE "cuadro_2.png",//11
	DPRE "cuadro_3.png",//12
	DPRE "cuadro_4.png",//13
	DPRE "cuadro_5.png",//14
};

SDL_Surface *fsl_screen, *fsl_dcimage;
static SDL_Surface *background, *background_invert, *left, *right;
static SDL_Surface *cuadro_g[5], *cuadro_p[5];
static SDL_Surface **cuadro;

SFont_FontInfo fsl_font;

static void create_background_invert(void)
{
	background_invert=SDL_CreateRGBSurface(background->flags,background->w,background->h,background->format->BitsPerPixel,background->format->Rmask,background->format->Gmask,background->format->Bmask,background->format->Amask);
	if (!background_invert)
		background_invert=background;
	else
	{
		int i,j;
		unsigned pitch=background->pitch/2;
        	SDL_LockSurface(background);
		SDL_LockSurface(background_invert);
		Uint16 *s=(Uint16 *)background->pixels;
		Uint16 *d=(Uint16 *)background_invert->pixels;
		for(j=0;j<VIDEO_HEIGHT;j++)
#ifndef VERTICAL_SCROLL
			for(i=0;i<VIDEO_WIDTH;i++)
				d[i+(j*pitch)]=s[(VIDEO_WIDTH-i-1)+(j*pitch)];
#else
			memcpy(&d[j*pitch],&s[(VIDEO_HEIGHT-j-1)*pitch],VIDEO_WIDTH*2);
#endif
		SDL_UnlockSurface(background_invert);
		SDL_UnlockSurface(background);
	}
}

static int load_surfaces(void)
{
    SDL_Surface *tmp;
    int i;

    for(i=0;i<15;i++)
    {
	tmp=IMG_Load(image_names[i]);
	if (tmp==NULL)
		return -1;
	switch(i)
	{
		case 0:
			background=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			create_background_invert();
			break;
		case 1:
			fsl_font.Surface=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			SFont_InitFontInfo(&fsl_font);
			break;
		case 2:
			left=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			SDL_SetColorKey(left, SDL_SRCCOLORKEY, 0);
			break;
		case 3:
			right=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			SDL_SetColorKey(right, SDL_SRCCOLORKEY, 0);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			cuadro_g[i-5]=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			SDL_SetColorKey(cuadro_g[i-5], SDL_SRCCOLORKEY, 0);
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			cuadro_p[i-10]=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
			SDL_SetColorKey(cuadro_p[i-10], SDL_SRCCOLORKEY, 0);
			break;
		default:
			fsl_dcimage=SDL_ConvertSurface(tmp,fsl_screen->format,SDL_RLEACCEL);
	}
	SDL_FreeSurface(tmp);
    }
    for(i=0;i<MAX_STARS;i++)
    {
	    star_r[i]=(((float)(rand()%360))*M_PI)/180.0;
	    star_a[i]=((float)(rand()%100)+11)/1.2345;
    }
    for(i=0;i<240;i++)
    {
	int c0=(i*2)+32;
	int c1=(i*2)+48;
	if (c0>255) c0=255;
	if (c1>255) c1=255;
	star_col[i]=SDL_MapRGB(fsl_screen->format,c1,c0,c0);
    }
    return 0;
}

#define MAX_CUADRO 12
#define ROTADO 2
#define MASCARA 0x3
static unsigned cuenta_cuadro=0;

static inline void draw_cuadro(unsigned x, unsigned y)
{
	SDL_Rect r;
	SDL_Surface *c=cuadro[cuenta_cuadro>>ROTADO];
	r.x=x;
	r.y=y;
	r.w=c->w;
	r.h=c->h;
	SDL_BlitSurface(c, NULL, fsl_screen, &r);
}


static inline void draw_cuadros(int momento)
{
	static float cuadros_frame=0.0;
	static float cuadros_acel=0.0;
	unsigned i,iframe=(unsigned)cuadros_frame;
	for(i=0;i<MAX_CUADRO;i++)
	{
		int iangle=(iframe)+(i<<4);
		float angle=((((float)(iangle%(360*2)))*M_PI)/180.0);
		int x=iangle%(320*2);
		int y=(int)(((sin(angle))*116.0)+108.0);
		int condicion= ((angle<(M_PI*0.5)) || ((angle>(M_PI*1.5))&&(angle<(M_PI*2.5))) || (angle>(M_PI*3.5)) );
		if ((condicion && momento)||(!condicion && !momento))
{
		if (x<320)
			draw_cuadro(x-8,y+4);
		else
			draw_cuadro(320-(x-320)-8,y+4);
}
	}
	if (momento)
	{
		cuenta_cuadro++;
		cuenta_cuadro%=(5<<ROTADO);
		cuadros_acel=cuadros_acel+(M_PI/2345.678);
		if (cuadros_acel>M_PI)
			cuadros_acel-=M_PI;
		cuadros_frame+=(0.3456+sin(cuadros_acel))*(2.0*M_PI);
	}
}

static inline void draw_star(int x, int y, int col)
{
	register unsigned short *buff=fsl_screen->pixels;
	x+=160;
	y+=120;
	buff[x+y*(fsl_screen->pitch>>1)]=col;
}

static inline void draw_stars(void)
{
	unsigned i;
	static float angle=0.0;
	static int spin=0;
        SDL_LockSurface(fsl_screen);
	for(i=0;i<MAX_STARS;i++)
	{
		float r=star_r[i];
		float a=star_a[i];
		float x=cos(r)*a;
		float y=sin(r)*a;
		if (x>-160.0 && x<160.0 && y>-120.0 && y<120.0)
		{
			draw_star((int)x,(int)y,star_col[(int)a]);
			star_a[i]+=(2.312*1.45);
		}
		else star_a[i]=12.234;
		star_r[i]+=angle;
		if (star_r[i]>(2*M_PI))
			star_r[i]-=(2*M_PI);
	}
        SDL_UnlockSurface(fsl_screen);
	if (!spin)
	{
		angle+=0.000145;
		if (angle>M_PI/83.456)
			spin=1;
	}
	else
	{
		angle-=0.000145;
		if (angle<0.003)
			spin=0;
	}
}

static inline void draw_image(SDL_Surface *image, int frame)
{
	SDL_Rect r;
	float ratio=((float)image->w)/((float)image->h);
	
	if (frame>60)
		frame=60;
	else
		if (frame<2)
			frame=2;
	frame<<=1;
	r.h=frame;
	r.w=(int)(ratio*((float)frame));
	r.x=(320-r.w)>>1;
	r.y=8+((120-r.h)>>1);
	SDL_LockSurface(fsl_screen);
	fsl_stretch16(image,&r);
	SDL_UnlockSurface(fsl_screen);
}

static void draw_text_desc(char * str, int x, int y, int frame, int min)
{
	int i;
	int back_x=x;
	char t[1024];
	char *tmp=(char *)&t;
	strcpy(tmp,str);
	int len=strlen(str);

	frame-=min;

	if (frame>len)
		frame=len;

	for(i=0;i<frame;i++,tmp++)
	{
		int l;
		char c=tmp[1];
		tmp[1]=0;
		l=SFont_TextWidthInfo(&fsl_font,tmp);
		x=x+l;
		if (x>320)
		{
#ifndef NO_DESC
			back_x=32;
			x=32+l;
#else
			back_x=2;
			x=2+l;
#endif
			y+=16;
		}
		font_put(back_x,y,tmp);
		back_x=x;
		tmp[1]=c;
	}
}

static inline void draw_arrows(p_fsl_game actual)
{
	SDL_Rect r;
	if (actual->back)
	{
		r.x=0; r.y=100; r.h=left->h; r.w=left->w;
		SDL_BlitSurface(left, NULL, fsl_screen, &r);
		
	}
	if (actual->next)
	{
		r.x=320-right->w; r.y=100; r.h=right->h; r.w=right->w;
		SDL_BlitSurface(right, NULL, fsl_screen, &r);
	}
}

static inline void draw_text(p_fsl_game actual, int frame)
{
	int fname=strlen(actual->name)+16+6;
	int fauthor=fname+strlen(actual->author)+8+16;
#ifndef NO_DESC
	draw_text_desc("Name:",2,140,frame,0);
#endif
	if (frame>6)
	{
#ifndef NO_DESC
		draw_text_desc(actual->name,52,140,frame,6);
#else
		draw_text_desc(actual->name,2,140,frame,6);
#endif
		if (frame>fname)
		{
#ifndef NO_DESC
			draw_text_desc("Author:",2,160,frame,fname);
#endif
			if (frame>fname+8)
			{
#ifndef NO_DESC
				draw_text_desc(actual->author,62,160,frame,fname+8);
#else
				draw_text_desc(actual->author,2,160,frame,fname+8);
#endif
				if (frame>fauthor)
				{
#ifndef NO_DESC
					draw_text_desc("Description:",2,180,frame,fauthor);
					if (frame>fauthor+12)
						draw_text_desc(actual->description,92,180,frame,fauthor+12);
#else
					draw_text_desc(actual->description,2,180,frame,fauthor+12);
#endif
				}
			}
		}
	}
}


void fsl_draw(p_fsl_game actual, int frame)
{
	SDL_Rect r;
#ifndef VERTICAL_SCROLL
	static float cos_pos=M_PI/2.0;
	int pos=(int)(cos(cos_pos)*(((double)(VIDEO_WIDTH))*7.0/8.0));
	r.x=pos;
	r.y=0; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background, NULL, fsl_screen, &r);
	r.x=pos-background->w; r.y=0; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background_invert, NULL, fsl_screen, &r);
	r.x=pos+background->w; r.y=0; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background_invert, NULL, fsl_screen, &r);
#else
	static float cos_pos=M_PI/2.0;
	int pos=(int)(cos(cos_pos)*(((double)(VIDEO_HEIGHT))*7.0/8.0));
	r.x=0; r.y=pos; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background, NULL, fsl_screen, &r);
	r.x=0; r.y=pos-background->h; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background_invert, NULL, fsl_screen, &r);
	r.x=0; r.y=pos+background->h; r.h=background->h; r.w=background->w;
	SDL_BlitSurface(background_invert, NULL, fsl_screen, &r);
#endif
	draw_stars();
	cuadro=cuadro_p;
	draw_cuadros(0);
	if (actual!=NULL)
	{
		if (actual->image!=NULL)
			draw_image(actual->image, frame);
		if ((frame>>2)&0x7)
			draw_arrows(actual);
		draw_text(actual, frame);
	}
	cuadro=cuadro_g;
	draw_cuadros(1);
	TV_Flip(fsl_screen);
// #ifndef VERTICAL_SCROLL
//	cos_pos+=0.031415;
// #else
	cos_pos+=0.031415/2.222;
// #endif
	if (cos_pos > 2*M_PI)
		cos_pos-=(2*M_PI);
}

int fsl_init_video(void)
{
	int ret=-1;
	fsl_screen=TV_Init(VIDEO_WIDTH,VIDEO_HEIGHT,VIDEO_COLOR_BITS,VIDEO_BITS );
#ifndef DREAMCAST
	SDL_ShowCursor(SDL_FALSE);
	SDL_WM_SetCaption("","");
	SDL_WM_SetIcon(SDL_LoadBMP(DPRE "icon.bmp"),NULL);
#endif
	ret=load_surfaces();
	fsl_clean_events();
	if (!ret)
	{
		SDL_BlitSurface(background, NULL, fsl_screen, NULL);
		TV_Flip(fsl_screen);
		SDL_Delay(100);
	}
	return ret;
}

void fsl_quit_video(void)
{
	p_fsl_game actual=fsl_first;
	SDL_FreeSurface(background_invert);
	SDL_FreeSurface(background);
	SDL_FreeSurface(left);
	SDL_FreeSurface(right);
	SDL_FreeSurface(fsl_font.Surface);
	while(actual)
	{
		if (actual->image!=fsl_dcimage)
			SDL_FreeSurface(actual->image);
		actual=(p_fsl_game)actual->next;
	}
	SDL_FreeSurface(cuadro_p[0]);
	SDL_FreeSurface(cuadro_p[1]);
	SDL_FreeSurface(cuadro_p[2]);
	SDL_FreeSurface(cuadro_p[3]);
	SDL_FreeSurface(cuadro_p[4]);
	SDL_FreeSurface(cuadro_g[0]);
	SDL_FreeSurface(cuadro_g[1]);
	SDL_FreeSurface(cuadro_g[2]);
	SDL_FreeSurface(cuadro_g[3]);
	SDL_FreeSurface(cuadro_g[4]);
	SDL_FreeSurface(fsl_dcimage);
#ifdef TARGET_GP32
	SDL_FreeSurface(fsl_screen);
#endif
}

void fsl_set_window_title(void)
{
#ifndef DREAMCAST
	if (fsl_screen)
		SDL_WM_SetCaption(fsl_games_name,fsl_games_name);
#endif
}
