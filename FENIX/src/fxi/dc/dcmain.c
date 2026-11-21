#include<kos.h>
#include<stdio.h>
#include<string.h>
#include<SDL.h>
#include<SDL_dreamcast.h>

KOS_INIT_FLAGS(INIT_DEFAULT);

int chdir(const char *path)
{
	if (path==NULL)
		return 0;
	if (!strlen(path))
		return 0;
	if (strncmp(path,DATA_PREFIX,strlen(DATA_PREFIX)))
	{
		char *tmp=calloc(strlen(path)+24,1);
		int ret;
		strcpy(tmp,DATA_PREFIX);
		strcpy(tmp+strlen(DATA_PREFIX),path);
		ret=fs_chdir(tmp);
		free(tmp);
		return ret;
	}
	return fs_chdir(path);
}

//int mkdir(const char *pathname, int mode)
int mkdir(const char *_path, mode_t __mode )
{
	return 0;
}

int rmdir(const char *pathname)
{
	return 0;
}

#ifndef NO_FSL_MAIN
char * fenix_runtime_error_msg=NULL;

int fsl_exit(int n)
{
	if (fenix_runtime_error_msg)
		printf("-- ERROR: '%s'\n",fenix_runtime_error_msg);
	printf("-- FSL_EXIT %i\n",n);
	SDL_Delay(1000);
	exit(n);
}

int main()
{
//puts("FENIX MAIN!!");fflush(stdout);
	SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_CDROM);
	return fenix_runtime("/cd/fenix.dat");
}
#endif
