#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>

#include <fxdll.h>

#define ALTURA 20

char    *agua=NULL;

static int fx_agua (INSTANCE * my, int * params)
{
    static int  desp    = 3000;
    static int  dec     = -1;
    int         x,
                inc;
    if(agua==NULL) {
        if ( ( agua = (char *) malloc(ALTURA*(background)->width) ) == NULL ) {
            return ( -1 ) ;
        }
    }

    inc = ((background)->height-ALTURA-1)*100;

    for(x=0;x<ALTURA;x++) {
        memcpy(agua+x*(background)->width,(char *)(background->data)+(inc/100)*(background)->width,(background)->width);
        inc-=desp/10;
        desp+=dec;
        if(desp<2500) dec=1;
        if(desp>3500) dec=-1;
    }

    memset(agua,0,(background)->width);
    memcpy((char *)(background->data)+((background)->height-ALTURA)*(background)->width,agua,(background)->width*ALTURA);

	return ( 0 ) ;
}

/*this is an auto load dll
  at very early stages it ripples/distorts the baackground layer only
*/

int     i           = 0 ;
float   sx          = 0 ;                  //variable determine where ripple is
char    *escreen    = NULL;            //extra screen
int     waves       = 15;   //controls ripple
float   step        = (float) 0.05,
        speed       = (float) 0.05;

static int fx_ripple(INSTANCE * my, int * params)   //this dumps the background screen to the video
{                                 //once its dumped we can make the ripple
    int     position;

    if ( escreen == NULL )
    {
        if((escreen = (char *) malloc ( (background)->width * (background)->height )) == NULL)
        {
            return -1 ;
        }
        memcpy ( escreen, (char *)(background->data), (background)->width * (background)->height ) ;
    }

    for(i=0;i<(background)->height;i++)
    {
        position = (int) (i+(sin(sx+i*step)*waves));                      //calculates position to copy line from
        if(position<(background)->height && position>=0)
        {
            memcpy(((char *)(background->data)+i*(background)->width),((escreen)+position*(background)->width),(background)->width);   //copys the line of the background
        }                                                               //to escreen
    }

    sx+=speed;                                                          //increments speed

    return ( 0 ) ;
}

static int fx_agua_free(INSTANCE * my, int * params)
{
    if ( agua != NULL ) {
        free ( agua ) ;
        agua = NULL ;
    }

    return ( 0 ) ;
}

static int fx_ripple_free(INSTANCE * my, int * params)
{
    if ( escreen != NULL ) {
        free ( escreen ) ;
        escreen = NULL ;
    }

    return ( 0 ) ;
}

// Faltaria una funcion para liberar (automatica desde fenix)

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("FXAGUA", "", TYPE_DWORD, fx_agua ) ;
	FENIX_export ("FXAGUARESET", "", TYPE_DWORD, fx_agua_free ) ;
    FENIX_export ("FXRIPPLE", "", TYPE_DWORD, fx_ripple ) ;
    FENIX_export ("FXRIPPLERESET", "", TYPE_DWORD, fx_ripple_free ) ;
}
