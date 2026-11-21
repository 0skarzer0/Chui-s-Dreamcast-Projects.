#pragma comment (lib, "SDL")
#include <fxdll.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>

#define PI		3.141



typedef struct
		{
			unsigned char	cloud_count,
					frame_count,
					max_size,
					blur_steps,
					density;
			
		}
		expl_info;


typedef struct
		{
			unsigned short	x,y,
					act_radius,
					delta_radius,
					density,
					delta_density;
			short	dx, dy;
		}
		expl_particle;


GRAPH *mapa_explosion;

expl_info *options;
expl_particle	**particles;

int SCREEN_WIDTH,SCREEN_HEIGHT;
unsigned char white;
int finish=1;


void init(init) {
	srand(time(NULL));
	white=255;
	
}

expl_particle *create_particle()
{
	static int	quadrant=-1;
	int 		sign_x[4] = { 1,-1,-1, 1 },
				sign_y[4] = { 1, 1,-1,-1 };
	float		amount_x, amount_r;

	expl_particle *particle=(expl_particle *)malloc(sizeof(expl_particle));

	particle->x = SCREEN_WIDTH  /2;
	particle->y = SCREEN_HEIGHT /2;


	amount_r =  (rand()%30)/(float)40.0;
	amount_x =  (float)1.0 - amount_r;

	if (quadrant == -1)
	{
		quadrant =1;

		particle->dx = 0;
		particle->dy = 0;

		particle->act_radius	= (options->max_size-10)/(2*options->frame_count);
		particle->delta_radius	= (unsigned short)((options->max_size *0.5)- particle->act_radius) / options->frame_count;

		particle->density 		= rand()%20;
		particle->delta_density	= particle->density / options->frame_count;
	}
	else
	{
		particle->dx = (short)(((options->max_size/2)*amount_x)/options->frame_count)*sign_x[quadrant];
		particle->dy = (short)(((options->max_size/2)*amount_x)/options->frame_count)*sign_y[quadrant];

		particle->act_radius	= (options->max_size-10)/(2*options->frame_count);
		particle->delta_radius	= (unsigned short)((options->max_size/2-particle->act_radius)*amount_r)/options->frame_count;

		particle->density 		= abs(options->density + rand()%30-15);
		particle->delta_density	= particle->density / options->frame_count;
	}

	

	quadrant++;
	if (quadrant==4)
		quadrant=0;

	return particle;
}


buff_spray(	unsigned short x, unsigned short y, unsigned short r, unsigned short density, unsigned char col)
{
	int i, x_, y_;
	int r2=r*2, rr=r*r, num;
	if (density<0)
		density=0;

	num=(int)(PI *rr*density)/100;

	for (i=0; i < num; i++)
	{
		x_=(rand()%r2)-r;
		y_=(rand()%r2)-r;

		if (x_* x_ + y_* y_ <=rr)
		{
				*( (Uint8*)mapa_explosion->data+(x+x_+(y+y_)*mapa_explosion->pitch)) = col;
		}
	}
}



draw_particle(expl_particle *particle)
{
	buff_spray(particle->x, particle->y, particle->act_radius, particle->density, white);
}





void buff_blur()
{
	
	int xs,xd,ys, yd, mx, my, value;
	unsigned short x=0;
	unsigned short y=0;
	unsigned short width=SCREEN_WIDTH;
	unsigned short height=SCREEN_HEIGHT;


	for (ys =y+1, yd=1; yd< (height-1); yd++, ys++)
	{
		for (xs =x+1, xd=1; xd < (width-1); xd++, xs++)
		{
			value=0;
			for (my=-1; my <2; my++)
			{
				for (mx=-1; mx <2; mx++)
				{
						value+=*((Uint8*)mapa_explosion->data+(xs+mx+(ys+my)*mapa_explosion->pitch));
				}
			}
			value/=9;		
				*((Uint8*)mapa_explosion->data+(xd+(yd)*mapa_explosion->pitch)) = value;
		}
	}
	
}


void
update_particle(expl_particle *particle)
{
	particle->act_radius += particle->delta_radius;
	particle->x+=particle->dx;
	particle->y+=particle->dy;
	particle->density-=particle->delta_density;
}




static int move_explosion (INSTANCE * my, int * params) {

	int act_particle;
	int i;

	if (finish==0) {
		for (act_particle=0; act_particle < options->cloud_count; act_particle++)
		{
			update_particle(particles[act_particle]);
			draw_particle(particles[act_particle]);
		}

		for (i=options->blur_steps; i ; i--)
		{
			buff_blur();			
		}
		options->frame_count--;

		if (options->frame_count==0) {
			
			free(particles);
			free(options);			
			finish=1;
		}

		return 1;
	}
	return -1;
	

}


static int explosion (INSTANCE * my, int * params) {

		int	a,i;

		if (finish==1) {		

			mapa_explosion=bitmap_new_syslib (params[5], params[6], 8);
			gr_clear (mapa_explosion);

			SCREEN_WIDTH=mapa_explosion->width;
			SCREEN_HEIGHT=mapa_explosion->height;

			options=(expl_info*)malloc(sizeof(expl_info));
		
			init();
	
			options->cloud_count	= params[0];
			options->max_size		= params[1];
			options->frame_count 	= params[2];
			options->blur_steps	    = params[3];
			options->density		= params[4];

			if (options->frame_count>20) options->frame_count=20;
			if (options->blur_steps>5) options->blur_steps=5;
		    if (options->max_size>SCREEN_HEIGHT) options->max_size=SCREEN_HEIGHT;
			if (options->max_size>SCREEN_WIDTH) options->max_size=SCREEN_WIDTH;


			particles=calloc(options->cloud_count, sizeof(*particles));
		
			for (a=0; a < options->cloud_count; a++) {
				particles[a]=create_particle();
				draw_particle(particles[a]);
			}
			for (i=options->blur_steps; i ; i--)
			{
			   buff_blur();
			}
		
			options->frame_count--;
			finish=0;
			return mapa_explosion->code;
		}
		return -1;
}



FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport
		
	FENIX_export ("EXPLOSION", "IIIIIII", TYPE_DWORD, explosion );
	FENIX_export ("MOVE_EXPLOSION", "", TYPE_DWORD, move_explosion );

}
