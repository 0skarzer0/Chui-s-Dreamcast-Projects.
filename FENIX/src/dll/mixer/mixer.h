#include "dumb.h"





void *file_open_mixer (const char *filename);
int  file_skip_mixer (void *f, long n);
int  file_getc_mixer (void *f);
int  file_getnc_mixer (char *ptr, long n, void *f);
void file_close_mixer (void *f);
int  get_mixer_channels (void);



