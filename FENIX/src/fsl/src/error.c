#include "fsl.h"

#include "font_error.h"

static SDL_Surface *font_surface;

static SFont_FontInfo font;

#define putstring(X,Y,Str) (SFont_PutStringInfo(fsl_screen,&font,X,Y,Str))

#define FADE_MIN 16 
#define FADE_MAX 64
#define FADE_STEP 2

static char error_title[]={ 150, ' ', 'E','R','R','O','R',' ','!',0 };

void fsl_init_display_error(void)
{
	font_surface=IMG_Load_RW(SDL_RWFromMem((void*)&font_png, FONT_PNG_SIZE),0);
	font.Surface=font_surface;
	SFont_InitFontInfo(&font);
}

void fsl_quit_display_error()
{
	SDL_FreeSurface(font_surface);
}

static void draw_box(int x, int y, int w, int h, Uint32 col)
{
	int rx=(fsl_screen->w - w)+fsl_screen->w;

	switch(fsl_screen->format->BitsPerPixel)
	{
		case 8:
			{
				Uint8 *buf=(Uint8 *)fsl_screen->pixels;
				buf+=x+ (y*fsl_screen->w);
				for(y=0;y<h;y+=2,buf+=rx)
				{
					Uint8 *buf2=buf+1+fsl_screen->w;
					for(x=0;x<w;x+=2,buf+=2,buf2+=2)
					{
						*buf=col;
						*buf2=col;
						
					}
				}
				
			}
			break;
		case 16:
			{
				Uint16 *buf=(Uint16 *)fsl_screen->pixels;
				buf+=x+ (y*fsl_screen->w);
				for(y=0;y<h;y+=2,buf+=rx)
				{
					Uint16 *buf2=buf+(1+fsl_screen->w);
					for(x=0;x<w;x+=2,buf+=2,buf2+=2)
					{
						*buf=col;
						*buf2=col;
						
					}
				}
				
			}
			break;
		case 32:
			{
				Uint32 *buf=(Uint32 *)fsl_screen->pixels;
				buf+=x+ (y*fsl_screen->w);
				for(y=0;y<h;y+=2,buf+=rx)
				{
					Uint32 *buf2=buf+1+fsl_screen->w;
					for(x=0;x<w;x+=2,buf+=2,buf2+=2)
					{
						*buf=col;
						*buf2=col;
						
					}
				}
				
			}
			break;
		default:
			{
				SDL_Rect r;
				r.x=x;
				r.y=y;
				r.w=w;
				r.h=h;
				SDL_FillRect(fsl_screen,&r,col);
			}
	}
}


static void draw_window(int x, int y, int w, int h, Uint32 col0, Uint32 col1, char *title, char *text)
{
	SDL_Rect r;

	if (y-3-font.h<0)
		y=3+font.h;
	if (x-3<0)
		x=3;
	if (y+h+3>fsl_screen->h)
		h=fsl_screen->h - (y+3);
	if (x+w+3>fsl_screen->w)
		w=fsl_screen->w - (x+3);

	if (h<font.h+4)
		h=font.h+4;

	draw_box(x-2,y-2,2,h+4,col0);
	draw_box(x-2,y-2-font.h,w+4,2+font.h,col0);
	draw_box(x-2,y+h,w+4,2,col0);
	draw_box(x+w,y-2,2,h+4,col0);

	r.x=x-3;
	r.y=y-3-font.h;
	r.w=w+6;
	r.h=1;
	SDL_FillRect(fsl_screen,&r,col1);
	r.y+=h+5+font.h;
	SDL_FillRect(fsl_screen,&r,col1);
	r.y-=h+5+font.h;
	r.w=1;
	r.h=h+6+font.h;
	SDL_FillRect(fsl_screen,&r,col1);
	r.x+=w+5;
	SDL_FillRect(fsl_screen,&r,col1);

	putstring(x+1,y-font.h,title);

	draw_box(x,y,w,h,col1);

	r.x=x;
	r.y=y;
	r.w=w;
	r.h=1;
	SDL_FillRect(fsl_screen,&r,col1);
	r.y+=h-1;
	SDL_FillRect(fsl_screen,&r,col1);
	r.y-=h-1;
	r.w=1;
	r.h=h;
	SDL_FillRect(fsl_screen,&r,col1);
	r.x+=w-1;
	SDL_FillRect(fsl_screen,&r,col1);

	r.x=x;
	r.y=y;
	r.w=w;
	r.h=h;
	SDL_SetClipRect(fsl_screen,&r);

	if (SFont_TextWidthInfo(&font,text)<=w)
		putstring(x+2,y+((h-font.h)>>1),text);
	else
	{
		int i;
		char *tmp=calloc(1024,1);
		strncpy(tmp,text,1023);
		while(SFont_TextWidthInfo(&font,tmp)>w)
			tmp[strlen(tmp)-1]=0;
		i=strlen(tmp);
		h=(h-font.h)/3;
		putstring(x+2,y+h,tmp);
		strncpy(tmp,text,1023);
		putstring(x+2,y+h+2+font.h,tmp+i);
		free(tmp);
	}
	SDL_SetClipRect(fsl_screen,NULL);
}

void fsl_invert_screen_color(void)
{
	int i, total=(fsl_screen->w*fsl_screen->h);
	Uint32 *buf=(Uint32 *)fsl_screen->pixels;

	switch(fsl_screen->format->BitsPerPixel)
	{
		case 24:
			total/=3;
			break;
		case 16:
			total>>=1;
		case 32:
			break;
		default:
			total>>=2;
	}
	for(i=0;i<total;i++,buf++)
		*buf^=-1;
}

void fsl_spin_screen_color(void)
{
	int i, total=(fsl_screen->w*fsl_screen->h)*fsl_screen->format->BytesPerPixel;
	Uint8 *buf=(Uint8 *)fsl_screen->pixels;
	for(i=0;i<total;i++,buf++)
	{
		register Uint16 d=*buf;
		register Uint8 h=(d>>4)&0x000F;
		register Uint8 l=(d<<4)&0x00F0;
		*buf=h|l;
	}
}

void fsl_black_screen_color(void)
{
	SDL_FillRect(fsl_screen,NULL,SDL_MapRGB(fsl_screen->format,0,8,16));
}

static void errcleanevents(void)
{
	SDL_Event event;
	SDL_Delay(100);
	while (SDL_PollEvent(&event)>0)
		SDL_Delay(10);
}

static int ifexitevent(void)
{
	int ret=0;
	SDL_Event event;
	while (SDL_PollEvent(&event)>0)
		switch(event.type)
		{
			case SDL_JOYBUTTONUP:
			case SDL_QUIT:
				ret=-1;
				break;
			case SDL_KEYUP:
				if ((event.key.keysym.sym==SDLK_ESCAPE)||(event.key.keysym.sym==SDLK_RETURN))
					ret=-1;
				break;
		}
	return ret;
}

static int errormessage(int x, int y, char *msg)
{
	int i,exitevent=0;
	int w=(fsl_screen->w)-(x<<1);
	int h=48; //(fsl_screen->h)-(y<<2);
	char *title=(char *)&error_title;
	Uint32 col1=SDL_MapRGB(fsl_screen->format,32,16,16);

	for(i=FADE_MIN;i<FADE_MAX;i+=FADE_STEP)
	{
		Uint32 col0=SDL_MapRGB(fsl_screen->format,i>>1,i>>1,i);
		draw_window(x,y,w,h, col0, col1, title,msg);
		TV_Flip(fsl_screen);
		SDL_Delay(10);
	}
	errcleanevents();
	for(i=FADE_MAX;i>FADE_MIN && !exitevent;i-=FADE_STEP)
	{
		Uint32 col0=SDL_MapRGB(fsl_screen->format,i>>1,i>>1,i);
		draw_window(x,y,w, h, col0, col1, title,msg);
		TV_Flip(fsl_screen);
		SDL_Delay(10);
		exitevent=ifexitevent();
	}
	return exitevent;
}

void fsl_display_error(char *error_display)
{
	int theend=0;

	errormessage(8,48,error_display);
	errormessage(8,48,error_display);
	while(!theend)
		theend=errormessage(8,48,error_display);
}

void fsl_display_error_invert(char *error_display)
{
	fsl_invert_screen_color();
	fsl_display_error(error_display);
	fsl_invert_screen_color();
}

void fsl_display_error_black(char *error_display)
{
	int vueltas=0, theend=0;

	while(!theend)
	{
		theend=errormessage(8,48,error_display);
		if (!theend && vueltas<2)
		{
			fsl_invert_screen_color();
			theend=errormessage(8,48,error_display);
			if (!theend)
			{
				fsl_spin_screen_color();
				theend=errormessage(8,48,error_display);
				if (!theend)
				{
					fsl_invert_screen_color();
					theend=errormessage(8,48,error_display);
					if (!theend)
					{
						if (vueltas>0) 
						{
							fsl_black_screen_color();
							theend=errormessage(8,48,error_display);
						}
						else
							fsl_spin_screen_color();
					}
				}
			}
		}
		vueltas++;
	}
}
