#include <fxdll.h>

static int hello_world (INSTANCE * my, int * params)
{
	printf ("Hello, world 2 !\n") ;
	return 0 ;
}

FENIX_MainDLL RegisterFunctions (COMMON_PARAMS)
{
    FENIX_DLLImport

	FENIX_export ("HELLO2", "", TYPE_DWORD, hello_world ) ;
}
