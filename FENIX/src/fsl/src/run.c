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
#include "fxi.h"
#include <unistd.h>

extern int fenix_runtime(char *filename);
char *fenix_runtime_error_msg=NULL;

/*
#ifdef TARGET_GP32
#include <gpgraphic.h>
#include <gpfont.h>
void fenix_gp32_runtime_say_error(void)
{
	GPDRAWSURFACE gpDraw;
	GpGraphicModeSet(8, NULL);
	GpLcdSurfaceGet(&gpDraw,0);
	GpSurfaceSet(&gpDraw);
	GpLcdEnable();
	GpRectFill(NULL, &gpDraw, 0, 0, 320, 240, 1);
	if (fenix_runtime_error_msg==NULL)
		fenix_runtime_error_msg="FENIX RUNTIME ERROR";
	GpTextOut(NULL, &gpDraw, 2, 2, fenix_runtime_error_msg, 0xFF);
	while(GpKeyGet()!=GPC_VK_NONE);
	while(GpKeyGet()==GPC_VK_NONE);
//	asm("mov        r3,#0; bx       r3;");
}
#endif
*/

void fsl_sayme_error(void)
{
	if (fenix_runtime_error_msg==NULL)
		fenix_runtime_error_msg="FENIX RUNTIME ERROR";
	fsl_init_display_error();
	fsl_display_error_black(fenix_runtime_error_msg);
	fsl_quit_display_error();
	free(fenix_runtime_error_msg);
	fenix_runtime_error_msg=NULL;
}

int fsl_run(char* exec)
{
	int i=0;
	p_fsl_game back, actual=fsl_first;
	char *dir=calloc(strlen(exec)+16,1);
	if (!dir)
		return -1;
	strcpy(dir,exec);
	while(actual)
	{
		if (actual->name!=fsl_unknow)
        		free(actual->name);
		if (actual->author!=fsl_anonymous)
			free(actual->author);
		if (actual->description!=fsl_no_description)
			free(actual->description);
		if (actual->exec!=fsl_empty)
			free(actual->exec);
		back=actual;
		actual=(p_fsl_game)actual->next;
		free(back);
	}
	fsl_first=NULL;

	for(i=0;i<strlen(dir);i++)
		if (dir[i]=='\\')
			dir[i]='/';
	for(i=strlen(dir)-1;i>=0;i--)
		if (dir[i]=='/')
		{
			dir[i]=0;
			break;
		}
	chdir(DATA_PREFIX);

	if (i>0)
	{
		chdir(dir);
		i++;
	}
	else i=0;
#ifdef WIN32
#define EJECUTABLE "FXI.EXE"
	{
		char const *args[] = { EJECUTABLE, dir+i, NULL };
		execv("../../" DPRE EJECUTABLE,args);
	}
	free(dir);
#else
	double_buffer=1;
        full_screen=0;
	audio_initialized=1;
	sound_active=1;
	i=fenix_runtime(dir+i);
	free(dir);
	fsl_sayme_error();
#endif
	return -1;
} 
