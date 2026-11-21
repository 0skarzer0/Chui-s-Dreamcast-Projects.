#include <stdlib.h>
#include <fxdll.h>

static int hello_world (INSTANCE * my, int * params)
{
	printf ("Hello, world!\n") ;
	return 0 ;
}

static int print (INSTANCE * my, int * params)
{
	printf ( "%s\n", string_get(params[0]) ) ;
	string_discard(params[0]) ;

	return ( 0 ) ;
}

static int put_star (INSTANCE * my, int * params)
{
	background_is_black = 0 ;
	gr_put_pixel(background, (int) (rand() % scr_width), (int) (rand() % scr_height), rand()%256) ;

	return ( 0 ) ;
}

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("HELLO", "", TYPE_DWORD, hello_world ) ;
	FENIX_export ("PRINT", "S", TYPE_DWORD, print ) ;
	FENIX_export ("PUTSTAR", "", TYPE_DWORD, put_star ) ;
}
