/*
 * Fenix SDL Loader
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "fsl.h"
#include <unistd.h>


#ifdef DREAMCAST

#include<arch/exec.h>

void fsl_exec_bin(char *name)
{
	void *mem;
	long len;
	FILE *f=fopen(name,"rb");
	if (!f)
	{
		char *tmp=calloc(1024,1);
		sprintf(tmp,"/cd/%s.bin",name);
		f=fopen(tmp,"rb");
		free(tmp);
		if (!f)
		{
			tmp=calloc(1024,1);
			sprintf(tmp,"/cd%s.bin",name);
			f=fopen(tmp,"rb");
			free(tmp);
			if (!f)
				return;
		}
	}
	fseek(f,0,SEEK_END);
	len=ftell(f);
	fseek(f,0,SEEK_SET);
	mem=malloc(len);
	if (mem)
	{
		if (fread(mem,1,len,f)==len)
			arch_exec(mem,len);
		free(mem);
	}
}
#else
void fsl_exec_bin(char *name)
{
	char *fdir=calloc(1024+sizeof(DATA_PREFIX),1);
	char *fname=calloc(1024,1);
	strncpy(fname,name,1016);
//	fsl_quit_audio();
SDL_Quit();
#ifdef WIN32
	strcat(fname,".EXE");
#endif
	chdir(DATA_PREFIX);
	strcpy(fdir,"./"); //DATA_PREFIX);
	strcat(fdir,fname);
	char const *args[] = { fname, NULL };
	execv(fdir,args);
	free(fname);
	free(fdir);
}
#endif

