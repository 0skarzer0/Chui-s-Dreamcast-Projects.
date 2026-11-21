#include <stdio.h>
#include <math.h>
#include <SDL.h>
#ifdef DREAMCAST
#include <SDL_dreamcast.h>
#endif
#include <SDL_opengl.h>

unsigned short texturebuffer[16*16*16];
GLint texture;

#ifdef DREAMCAST
#define FILTER GL_FILTER_BILINEAR
#define FORMAT_SRC GL_ARGB1555
#define FORMAT_DST GL_ARGB1555
#else
#define FILTER GL_LINEAR
#define FORMAT_SRC GL_RGBA
#define FORMAT_DST GL_BGRA
#endif

void genTexture(void)
{
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,FILTER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,FILTER);
	glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexImage2D(GL_TEXTURE_2D,0,FORMAT_SRC,16,16,0,FORMAT_DST,GL_UNSIGNED_SHORT_1_5_5_5_REV,(void *)&texturebuffer);
}

void display(void)
{
	static double angle1=0.0,angle2=0.0;

	double addx1=sin(angle1);
	double addx2=sin(angle2);
	double z1=addx1/10.0;
	double z2=addx2/10.0;

	angle1+=M_PI/75.0;
	if (angle1>(M_PI*2.0))
		angle1-=(M_PI*2.0);

	angle2+=M_PI/96.0;
	if (angle2>(M_PI*2.0))
		angle2-=(M_PI*2.0);


/* ONLY FOR DREAMCAST (transparency list) */
#ifdef DREAMCAST
	glKosFinishList();
#endif

	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();


	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex3f(addx1-0.3,0.1,z1);
		glTexCoord2f(1.0,0.0);
		glVertex3f(addx1+0.1,0.1,z1);
		glTexCoord2f(1.0,1.0);
		glVertex3f(addx1+0.1,-0.2,z1);
		glTexCoord2f(0.0,1.0);
		glVertex3f(addx1-0.3,-0.2,z1);
	glEnd();


	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
		glVertex3f(addx2-0.5,0.5,z2);
		glTexCoord2f(1.0,0.0);
		glVertex3f(addx2+0.5,0.5,z2);
		glTexCoord2f(1.0,1.0);
		glVertex3f(addx2+0.5,-0.5,z2);
		glTexCoord2f(0.0,1.0);
		glVertex3f(addx2+-0.5,-0.5,z2);
	glEnd();


	glFlush();

	SDL_GL_SwapBuffers();
}

void clear_events(void)
{
	SDL_Event event;
	SDL_Delay(100);
	while(SDL_PollEvent(&event))
		SDL_Delay(10);
}

int main(int argc, char ** argv)
{
	int i,j;

	for(j=0;j<16;j++)
		for(i=0;i<16;i++)
				texturebuffer[i+(j*16)]=(i*0x11)|(j*0x0100)|0x8000;
	for(j=4;j<12;j++)
		for(i=4;i<12;i++)
				texturebuffer[i+(j*16)]=0x1; /* Transparency */

	int theend=0;
#ifdef DREAMCAST
	SDL_DC_ShowAskHz(SDL_FALSE);
#endif
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);
	SDL_SetVideoMode(640,480,16,SDL_DOUBLEBUF | SDL_SWSURFACE | SDL_OPENGL);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);
	glClearDepth(1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0,0.0,0.0,0.0);
	glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);


	genTexture();
	clear_events();

	while(!theend)
	{
		SDL_Event event;
		display();
		while(SDL_PollEvent(&event))
			switch(event.type)
			{
				case SDL_QUIT:
				case SDL_KEYDOWN:
				case SDL_JOYBUTTONDOWN:
					theend=1;
			}
#ifndef DREAMCAST
		SDL_Delay(50);
#endif
	}
	SDL_Quit();
	return 0;
}


