#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <GL/gl.h>

#include "tvfilter.h"

#ifndef NO_USE_TV_FILTER

#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5                 0x8363
#endif

#ifndef GL_UNSIGNED_SHORT_4_4_4_4
#define GL_UNSIGNED_SHORT_4_4_4_4          	0x8033
#endif

#define SCANLINE_X 8
#define SCANLINE_Y 1024
#define SCANLINE_BLEND   0x30000000
#define SCANLINE_SCALE (((double)576)/((double)SCANLINE_Y))

static SDL_Surface *real_screen=NULL, *surface_screen=NULL;
static int opened_w=0;
static int opened_h=0;
static int opened_bpp=0;
static int opened_flags=0;
static int size_w=0;
static int size_h=0;
static int real_w=0;
static int real_h=0;
static int real_bpp=0;
static int txtr_w=0;
static int txtr_h=0;
static void *txtr_pix=NULL;
static GLint txtr=0;
static int ini_x=0;
static int ini_y=0;
static int to_resize=0;
static unsigned *surface_texture=NULL;

static unsigned scanline[SCANLINE_Y][SCANLINE_X];

static GLint scanline_txtr=0;

static SDL_bool filter=SDL_FALSE;
static SDL_bool distorsion=SDL_TRUE;
static SDL_bool scanlines=SDL_TRUE;


#ifdef DEBUG_TVFILTER
static void show_glerror(char *m, GLenum e)
{
	if (e==GL_NO_ERROR)
		fprintf(stderr,"!!-- OpenGL ERROR GL_NO_ERROR (#%i): '%s'\n",e,m);
	else if (e==GL_INVALID_ENUM)
		fprintf(stderr,"!!-- OpenGL ERROR GL_INVALID_ENUM (#%i): '%s'\n",e,m);
	else if (e==GL_INVALID_VALUE)
		fprintf(stderr,"!!-- OpenGL ERROR GL_INVALID_VALUE (#%i): '%s'\n",e,m);
	else if (e==GL_INVALID_OPERATION)
		fprintf(stderr,"!!-- OpenGL ERROR GL_INVALID_OPERATION (#%i): '%s'\n",e,m);
	else if (e==GL_STACK_OVERFLOW)
		fprintf(stderr,"!!-- OpenGL ERROR GL_STACK_OVERFLOW (#%i): '%s'\n",e,m);
	else if (e==GL_STACK_UNDERFLOW)
		fprintf(stderr,"!!-- OpenGL ERROR GL_STACK_UNDERFLOW (#%i): '%s'\n",e,m);
	else if (e==GL_OUT_OF_MEMORY)
		fprintf(stderr,"!!-- OpenGL ERROR GL_OUT_OF_MEMORY (#%i): '%s'\n",e,m);
	else
		fprintf(stderr,"!!-- OpenGL UNKNOWN ERROR (#%i): '%s'\n",e,m);
	fflush(stderr);
}
#define CHECKERROR(MSG) \
{ \
	static int yet_showed=0; \
	if (!yet_showed) \
	{ \
		GLenum chkerr=glGetError(); \
		if (chkerr!=GL_NO_ERROR) \
		{ \
			yet_showed=1;  \
			show_glerror((MSG),chkerr); \
		} \
	} \
}
#else
#define CHECKERROR(MSG)
#endif


void TV_Quit(void)
{
	if (txtr_pix)
		free(txtr_pix);
//	if (surface_screen && surface_screen!=real_screen)
//		SDL_FreeSurface(surface_screen);
	if (scanline_txtr)
		glDeleteTextures(1,(GLuint *)&scanline_txtr);
	if (txtr)
		glDeleteTextures(1,(GLuint *)&txtr);
	scanline_txtr=0;
	real_screen=NULL;
	txtr_pix=NULL;
//	surface_screen=NULL;
	if (SDL_WasInit(SDL_INIT_VIDEO))
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void TV_ToggleFullScreen(SDL_Surface *screen)
{
	if (real_screen)
		TV_Init(opened_w,opened_h,opened_bpp,opened_flags^SDL_FULLSCREEN);
	else
		SDL_WM_ToggleFullScreen(surface_screen);
}

void TV_SetFullScreen(SDL_Surface *screen, SDL_bool b)
{
	if (real_screen)
	{
		if (b)
		{
			if (!(opened_flags&SDL_FULLSCREEN))
					TV_ToggleFullScreen(screen);
		}
		else
		{
			if (opened_flags&SDL_FULLSCREEN)
					TV_ToggleFullScreen(screen);
		}
	}
}

static void real_resize(void)
{
	if (real_screen && !(opened_flags&SDL_FULLSCREEN))
	{
		TV_Init(opened_w,opened_h,opened_bpp,opened_flags);
	}
}

void TV_ResizeWindow(int w, int h)
{
	to_resize=8;
	size_w=w;
	size_h=h;
}

void TV_SetFilter(SDL_bool b)
{
	if (real_screen)
	{
		glBindTexture(GL_TEXTURE_2D,txtr);
		if (b)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//		glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE, GL_MODULATE);

	}
}

void TV_ToggleFilter(void)
{
	filter=!filter;
	TV_SetFilter(filter);
}

void TV_SetDistorsion(SDL_bool b)
{
	if (b)
		distorsion=SDL_TRUE;
	else
		distorsion=SDL_FALSE;
}

void TV_ToggleDistorsion(void)
{
	distorsion=!distorsion;
}

void TV_SetScanlines(SDL_bool b)
{
	if (b)
		scanlines=SDL_TRUE;
	else
		scanlines=SDL_FALSE;
}

void TV_ToggleScanlines(void)
{
	scanlines=!scanlines;
}


int TV_ConvertMousePosX(int x)
{
	if (!opened_w)
		return 0;
//	return ((x*real_w)/opened_w);
	return ((x*opened_w)/real_w);
}

int TV_ConvertMousePosY(int y)
{
	if (!opened_h)
		return 0;
//	return ((y*real_h)/opened_h);
	return ((y*opened_h)/real_h);
}

static float z_val=0.001f;

static void blit_txtr(int x, int y)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f); glVertex3f(ini_x+x, ini_y+y, z_val);
	glTexCoord2f(1.0f,0.0f); glVertex3f(txtr_w+ini_x+x, ini_y+y, z_val);
	glTexCoord2f(1.0f,1.0f); glVertex3f(txtr_w+ini_x+x, txtr_h+ini_y+y, z_val);
	glTexCoord2f(0.0f,1.0f); glVertex3f(ini_x+x, txtr_h+ini_y+y, z_val);
	glEnd();
	z_val+=0.001f;
}

static void flip(void)
{
	if (!real_screen)
		return;

	if (surface_screen==real_screen)
	{
		SDL_Flip(real_screen);
		return;
	}

	z_val=0.001f;

	glClear(GL_COLOR_BUFFER_BIT);
	CHECKERROR("flip glClear");

	glLoadIdentity();
	CHECKERROR("flip glLoadIdentity");

	glBindTexture(GL_TEXTURE_2D,txtr);
	CHECKERROR("flip glBindTexture");
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	CHECKERROR("flip glColor4f");
	blit_txtr(0,0);
	CHECKERROR("flip blit_txtr");

	if (distorsion)
	{
		glColor4f(1.0f,1.0f,1.0f,0.03f);
		blit_txtr(-1,-1);
		blit_txtr(-1, 0);
		blit_txtr(-1, 1);
		blit_txtr( 0,-1);
		blit_txtr( 0, 1);
		blit_txtr( 1,-1);
		blit_txtr( 1, 0);
		blit_txtr( 1, 1);
	}

	if (scanlines)
	{
		glBindTexture(GL_TEXTURE_2D,scanline_txtr);
		CHECKERROR("flip glBindTexture2");
		glColor4f(1.0f,1.0f,1.0f,1.0f);
		CHECKERROR("flip glColor4f(1.0f,1.0f,1.0f,1.0f)");
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex3f(ini_x, ini_y, z_val);
		glTexCoord2f(SCANLINE_SCALE,0.0f); glVertex3f(ini_x+txtr_w, ini_y, z_val);
		glTexCoord2f(SCANLINE_SCALE,SCANLINE_SCALE); glVertex3f(ini_x+txtr_w, ini_y+txtr_h, z_val);
		glTexCoord2f(0.0f,SCANLINE_SCALE); glVertex3f(ini_x, ini_y+txtr_h, z_val);
		glEnd();
		CHECKERROR("flip glEnd");
	}

	SDL_GL_SwapBuffers();
	CHECKERROR("flip SDL_GL_SwapBuffers");
}


static void convert_to_texture(void *src_ptr)
{
	unsigned char *dst=(unsigned char *)surface_texture;
	unsigned short *src=(unsigned short *)src_ptr;
	int i,max=txtr_h*txtr_w;
	for(i=0;i<max;i++)
	{
		unsigned short d=*src++;
		*dst++=(d&0xF800)>>(11-3);
		*dst++=(d&0xFE0)>>(5-2);
		*dst++=(d&0x1F)<<3;
		dst++;
	}
}

void TV_Flip(SDL_Surface *screen)
{
	if (!screen)
		return;

	if (to_resize)
	{
		to_resize--;
		if (!to_resize)
			real_resize();
	}
	if (surface_screen==real_screen)
		SDL_Flip(screen);
	else
	{
		glBindTexture(GL_TEXTURE_2D,txtr);
		CHECKERROR("TV_Flip glBindTexture");
		switch(screen->format->BitsPerPixel)
		{
			case 16:
				if (!surface_texture)
					glTexSubImage2D(GL_TEXTURE_2D,0,0,0,txtr_w,txtr_h,GL_RGB, GL_UNSIGNED_SHORT_5_6_5, screen->pixels);
				else
				{
					convert_to_texture(screen->pixels);
					glTexSubImage2D(GL_TEXTURE_2D,0,0,0,txtr_w,txtr_h,GL_RGBA, GL_UNSIGNED_BYTE, surface_texture);
				}
				CHECKERROR("TV_Flip glTexSubImage2D 16");
				break;
			case 32:
				glTexSubImage2D(GL_TEXTURE_2D,0,0,0,txtr_w,txtr_h,GL_RGBA, GL_UNSIGNED_BYTE, screen->pixels);
				CHECKERROR("TV_Flip glTexSubImage2D 32");
		}
		flip();
	}
}

SDL_Surface *TV_Init(int w, int h, int bpp, int flags)
{
	int i;
	const SDL_VideoInfo *info;
	SDL_Surface *back_surface_screen=surface_screen;
	if (bpp!=32)
		bpp=16;
#ifdef DEBUG_TVFILTER
	printf("TV_Init(w=%i, h=%i, bpp=%i, flags=0x%X)\n",w,h,bpp,flags);fflush(stdout);
	{
	 SDL_version ver;
	 SDL_VERSION(&ver);
	 printf(" SDL compile-time version %u.%u.%u\n", ver.major, ver.minor, ver.patch);
	 ver = *SDL_Linked_Version();
	 printf(" SDL runtime version %u.%u.%u\n", ver.major, ver.minor, ver.patch);
	}
	fflush(stdout);
#endif
	TV_Quit();
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	SDL_putenv("SDL_VIDEO_CENTERED=center");
#ifdef WIN32
//	SDL_putenv("SDL_VIDEODRIVER=directx");
#endif
	info = SDL_GetVideoInfo();
	if (info)
	{
		if (info->hw_available)
			flags|=SDL_HWSURFACE;
		else
			flags|=SDL_SWSURFACE;
		if (info->blit_hw)
			flags|=SDL_HWACCEL;
#ifdef DEBUG_TVFILTER
		printf(" Hardware Available = %i\n",info->hw_available);
		printf(" Hardware Blit = %i\n",info->blit_hw);
		fflush(stdout);
#endif
	}

	real_w=0; real_h=0;
	if (info)
	{
		SDL_Rect **modes=SDL_ListModes(info->vfmt,SDL_FULLSCREEN);
		if (modes && modes != (SDL_Rect **)-1)
		{
			for(i=0;modes[i];i++)
			{
				if (modes[i]->w>real_w && modes[i]->w>w && modes[i]->h>real_h)
				{
					real_w=modes[i]->w;
					real_h=modes[i]->h;
				}
			}
//			free(modes);
		}
	}
	if (flags&SDL_DOUBLEBUF || flags&SDL_GL_DOUBLEBUFFER)
	{
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		flags|=SDL_DOUBLEBUF|SDL_GL_DOUBLEBUFFER;
	}
	if (bpp==16)
	{
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 4 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 4 );
	}
	else
	{
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
	}

	flags|=SDL_OPENGL|SDL_RESIZABLE;

	if (flags&SDL_FULLSCREEN)
	{
#ifdef DEBUG_TVFILTER
		puts(" Fullscreen");
#endif
		real_screen=SDL_SetVideoMode(0,0,0,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(0,0,bpp,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(real_w,real_h,bpp,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(real_w,real_h,0,flags);
	}
	else
	{
		if (!size_w && !size_h)
		{
			if ((2*real_w)/3>=w*2 && (2*real_h)/3>=h*2)
			{
				size_w=w*2;
				size_h=h*2;
			}
			else
			if (real_w>=w && real_h>=h)
			{
				size_w=w;
				size_h=h;
			}
			else
			{
				size_w=real_w;
				size_h=real_h;
			}
		}
#ifdef DEBUG_TVFILTER
		printf(" Windowed %i x %i\n",size_w,size_h);
#endif
		real_screen=SDL_SetVideoMode(size_w,size_h,0,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(w,h,0,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(size_w,size_h,bpp,flags);
		if (!real_screen)
			real_screen=SDL_SetVideoMode(w,h,bpp,flags);
	}
	if (real_screen)
	{
		opened_w=w;
		opened_h=h;
		opened_bpp=bpp;
		opened_flags=flags;
		real_w=real_screen->w;
		real_h=real_screen->h;
		real_bpp=real_screen->format->BitsPerPixel;
#ifdef DEBUG_TVFILTER
		{
			char buf[1024];
			printf(" Video Driver '%s'\n",SDL_VideoDriverName(buf,1024));
		}
		i=0; SDL_GL_GetAttribute(SDL_GL_RED_SIZE,&i);
		printf(" SDL_GL_RED_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE,&i);
		printf(" SDL_GL_GREEN_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE,&i);
		printf(" SDL_GL_BLUE_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE,&i);
		printf(" SDL_GL_ALPHA_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_BUFFER_SIZE,&i);
		printf(" SDL_GL_BUFFER_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER,&i);
		printf(" SDL_GL_DOUBLEBUFFER = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE,&i);
		printf(" SDL_GL_DEPTH_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE,&i);
		printf(" SDL_GL_STENCIL_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_ACCUM_RED_SIZE,&i);
		printf(" SDL_GL_ACCUM_RED_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_ACCUM_GREEN_SIZE,&i);
		printf(" SDL_GL_ACCUM_GREEN_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_ACCUM_BLUE_SIZE,&i);
		printf(" SDL_GL_ACCUM_BLUE_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,&i);
		printf(" SDL_GL_ACCUM_ALPHA_SIZE = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_STEREO,&i);
		printf(" SDL_GL_STEREO = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS,&i);
		printf(" SDL_GL_MULTISAMPLEBUFFERS = %i\n",i);
		i=0; SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES,&i);
		printf(" SDL_GL_MULTISAMPLESAMPLES = %i\n",i);
		printf(" OpenGL Vendor '%s'\n",glGetString(GL_VENDOR));
		printf(" OpenGL Renderer '%s'\n",glGetString(GL_RENDERER));
		printf(" OpenGL Version '%s'\n",glGetString(GL_VERSION));
		printf(" OpenGL Extensions '%s'\n",glGetString(GL_EXTENSIONS));
		i=0; glGetIntegerv(GL_MAX_TEXTURE_SIZE,&i);
		printf(" GL_MAX_TEXTURE_SIZE = %i\n",i);
		i=0; glGetIntegerv(GL_RGBA_MODE,&i);
		printf(" GL_RGBA_MODE = %i\n",i);
		printf(" Screen Width=%i, Height=%i, BPP=%i\n",real_w,real_h,real_bpp);
		printf(" Screen BytesPerPixel=%i, Rmask=%X, Gmask=%X, Bmask=%X, Amask=%X (%i)\n",real_screen->format->BytesPerPixel, real_screen->format->Rmask, real_screen->format->Gmask, real_screen->format->Bmask, real_screen->format->Amask,real_screen->format->alpha);
		fflush(stdout);
#endif
		for(i=8;i<real_w*8;i*=2)
			if (w<=i)
			{
				txtr_w=i;
				break;
			}
		for(i=8;i<real_h*8;i*=2)
			if (h<=i)
			{
				txtr_h=i;
				break;
			}
		txtr_pix=calloc(txtr_w*txtr_h,(bpp/8));
#ifdef DEBUG_TVFILTER
		printf(" Texture Width=%i, Height=%i, BPP=%i\n",txtr_w,txtr_h,bpp);fflush(stdout);
#endif
		glDisable(GL_DEPTH_TEST);
		CHECKERROR("TV_Init glDisable(GL_DEPTH_TEST)");
		glClearColor(0.0, 0.0, 0.0, 0.0);
		CHECKERROR("TV_Init glClearColor");
		glViewport(0, 0, real_w, real_h);
		CHECKERROR("TV_Init glViewport");
		glMatrixMode( GL_PROJECTION );
		CHECKERROR("TV_Init glMatrixMode(GL_PROJECTION)");
		if (real_w>=real_h)
		{
			double real_ratio=(double)real_w / (double)real_h;
			double nh=(double)w / real_ratio;
			if (nh>=h)
			{
				ini_x=0;
				ini_y=(((int)nh-h)/2) / real_ratio;
				glOrtho(0.0, w, nh, 0.0, -50.0, 50.0 );
			}
			else
			{
				double nw=(double)h * real_ratio;
				ini_x=(((int)nw-w)/2);
				ini_y=0;
				glOrtho(0.0, nw, h, 0.0, -50.0, 50.0 );
			}
		}
		else
		{
			double real_ratio=(double)real_h / (double)real_w;
			double nw=(double)h / real_ratio;
			if (nw>=w)
			{
				ini_x=(((int)nw-w)/2) / real_ratio;
				ini_y=0;
				glOrtho(0.0, nw, h, 0.0, -50.0, 50.0 );
			}
			else
			{
				double nh=(double)w * real_ratio;
				ini_x=0;
				ini_y=(((int)nh-h)/2);
				glOrtho(0.0, w, nh, 0.0, -50.0, 50.0 );
			}
		}
		CHECKERROR("TV_Init glOrtho");
		glMatrixMode( GL_MODELVIEW );
		CHECKERROR("TV_Init glMatrixMode(GL_MODELVIEW)");
		glLoadIdentity();
		CHECKERROR("TV_Init glLoadIdentity");

		glEnable(GL_TEXTURE_2D);
		CHECKERROR("TV_Init glEnable(GL_TEXTURE_2D)");
		glEnable(GL_BLEND);
		CHECKERROR("TV_Init glEnable(GL_BLEND)");
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CHECKERROR("TV_Init glBlendFunc");

		glGenTextures(1,(GLuint *)&scanline_txtr);
		CHECKERROR("TV_Init glGenTextures scanline");
		glBindTexture(GL_TEXTURE_2D,scanline_txtr);
		CHECKERROR("TV_Init glBindTexture scanline");
		for(i=0;i<SCANLINE_Y;i+=2)
		{
			int j;
			for(j=0;j<SCANLINE_X;j++)
				scanline[i][j]=SCANLINE_BLEND;
			memset(&scanline[i+1][0],0,sizeof(unsigned)*SCANLINE_X);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, 4, SCANLINE_X, SCANLINE_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, scanline);
		CHECKERROR("TV_Init glTexImage2D scanline");
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) scanline");
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) scanline");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) scanline");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_TRAP_S, GL_CLAMP) scanline");
//		glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE, GL_BLEND);
//		CHECKERROR("TV_Init glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_BLEND) scanline");

		glGenTextures(1,(GLuint *)&txtr);
		CHECKERROR("TV_Init glGenTextures txtr");
		glBindTexture(GL_TEXTURE_2D,txtr);
		CHECKERROR("TV_Init glBindTexture txtr");

		if (surface_texture)
		{
			free(surface_texture);
			surface_texture=NULL;
		}

		switch(bpp)
		{
			case 16:
				surface_screen = SDL_CreateRGBSurfaceFrom(txtr_pix, w, h, 16, txtr_w*2 , 0xF800, 0x7E0, 0x1F, 0);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, txtr_w, txtr_h, 0,GL_RGB, GL_UNSIGNED_SHORT_5_6_5, txtr_pix);
				if (glGetError()==GL_NO_ERROR)
					break;
				surface_texture=calloc(txtr_w*txtr_h,(32/8));
				glTexImage2D(GL_TEXTURE_2D, 0, 3, txtr_w, txtr_h, 0,GL_RGBA, GL_UNSIGNED_BYTE, surface_texture);
				break;
			case 32:
				surface_screen = SDL_CreateRGBSurfaceFrom(txtr_pix, w, h, 32, txtr_w*4 , 0xFF, 0xFF00, 0xFF0000, 0x0);
				glTexImage2D(GL_TEXTURE_2D, 0, 3, txtr_w, txtr_h, 0,GL_RGBA, GL_UNSIGNED_BYTE, txtr_pix);
				break;
			default:
				surface_screen=NULL;		
		}
		CHECKERROR("TV_Init glTexImage2D txtr");
		if (surface_screen && back_surface_screen)
		{
			SDL_Surface tmp;
			//memcpy((void *)&tmp,back_surface_screen,sizeof(SDL_Surface));
			//memcpy(back_surface_screen,surface_screen,sizeof(SDL_Surface));
			//memcpy(surface_screen,(void *)&tmp,sizeof(SDL_Surface));
			tmp=*back_surface_screen;
			*back_surface_screen=*surface_screen;
			*surface_screen=tmp;
			SDL_FreeSurface(surface_screen);
			surface_screen=back_surface_screen;
		}
		if (filter)
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) txtr");
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
		else
		{
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) txtr");
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) txtr");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) txtr");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		CHECKERROR("TV_Init glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_TRAP_S, GL_CLAMP) txtr");
//		glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE, GL_MODULATE);
//		CHECKERROR("TV_Init glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE) txtr");
		return surface_screen;
	}
#ifdef DEBUG_TVFILTER
	puts(" NOT OPENGL!!"); fflush(stdout);
#endif
	flags&=~(SDL_OPENGL|SDL_RESIZABLE);

	if (!real_screen)
		real_screen=SDL_SetVideoMode(0,0, 0,flags);

	if (!real_screen)
		real_screen=SDL_SetVideoMode(real_w,real_h, 0,flags);


	if (real_screen)
	{
		opened_w=w;
		opened_h=h;
		opened_bpp=bpp;
		opened_flags=flags;
	}
	surface_screen=real_screen;
	return real_screen;
}

#endif

