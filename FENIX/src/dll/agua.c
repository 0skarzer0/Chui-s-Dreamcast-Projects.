#include <fxdll.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

static int    altura = 40;
static char  *agua = NULL;
static int    altura_agua = 0;




static void x_agua (void * ptr)
{
	static int  desp    = 15000;
    static int  dec     = -1;
	static int  despx   = 0;
	int         pixelsize = 1;
    int         x, y, inc, incx;
	GRAPH *     gr = scrbitmap;

	if (gr == NULL)
		return;

	if (gr->depth == 8)
		pixelsize = 1;
	else
		pixelsize = 2;

	if (altura > gr->height/5)
		altura = gr->height/5;

    if (agua==NULL || altura > altura_agua) 
	{
		if (agua) free(agua);
		altura_agua = (altura & ~31) + 32;
        if ( ( agua = (char *) malloc(gr->pitch * altura_agua) ) == NULL ) 
            return ;
    }

    inc = (gr->height - altura - 1)*100;
	incx = 0;
    for(y = 0 ; y < altura ; y++) 
	{
		int shift = (int) ((y*16.0/altura) * cos(despx/200.0 + 32.0 * (y*2+altura)/(altura*3)));

		if (inc < 0) { 
			inc = 0; 
			altura--; 
		}
        memcpy(agua + y*gr->pitch + shift * pixelsize,
			   (char *)gr->data + (inc/100)*gr->pitch,
			   (gr->width - shift) * pixelsize);
        
		if (shift > 0)
		{
			memcpy(agua + y*gr->pitch,
			   (char *)gr->data + (inc/100)*gr->pitch + (gr->width - shift) * pixelsize,
			   shift * pixelsize);
		}
        inc -= desp/50;
        desp += dec;
		despx++;
		incx += 8;
        if (desp < 12500) dec=1;
        if (desp > 17500) dec=-1;
    }

    memset(agua, 0, gr->pitch);

	for (y = 1 ; y < altura ; y++)
	{
		Uint8 * dest = (char *)gr->data + (gr->height-altura-1+y)*gr->pitch;
		Uint8 * orig = agua + gr->pitch * y;

		if (pixelsize == 1)
		{
			gr_make_trans_table();
			for (x = 0 ; x < gr->widthb ; x++, dest++, orig++)
				*dest = trans_table[(*dest << 8) + *orig] ;
		}
		else
		{
			for (x = 0 ; x < gr->width ; x++, dest+=2, orig+=2)
				*(Uint16 *)dest = 
						colorghost[*(Uint16*)dest] + 
				        colorghost[*(Uint16*)orig];
		}
	}
}


int info_desktop (void * user, REGION * bbox)
{
	bbox->x = 0;
	bbox->y = 0;
	bbox->x2 = scr_width-1;
	bbox->y2 = scr_height-1;
	return 1;
}


void draw_desktop (void * user, REGION * clip)
{


	static int lastmx = -1;
	static int lastmy = -1;
	static int lastmleft = -1;
	static int lastmright = -1;
	static int lastmcenter = -1;
	static int lastmwup = -1;
	static int lastmwdown = -1;
	static int lastkey = -1;

	x_agua(clip);
	
	return;
}


static int fxi_agua (INSTANCE * my, int * params)
{
	static int old_object = -1;

	if (old_object != -1)
		{
			gr_destroy_object (old_object);
			old_object = -1;
		}


			altura = params[1];
			if (altura > 0) 
				{
					old_object = gr_new_object (params[0],info_desktop ,draw_desktop,NULL);
					return 1;

				} else {
					return 1;
				}


 //		z				Z value of the object to be drawn
 //		info			Pointer to the object information function
 //						(fills bounding box, returns 1 if changed since last frame)
 //		draw			Pointer to the object drawing function
 //		what			User-defined parameter that will be passed to "draw"


}

// Faltaria una funcion para liberar (automatica desde fenix)

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("FXAGUA", "II", TYPE_DWORD, fxi_agua ) ;
}
