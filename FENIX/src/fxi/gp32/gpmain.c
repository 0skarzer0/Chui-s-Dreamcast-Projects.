#include <gpstdio.h>
#include <gpstdlib.h>
#include <gpgraphic.h>
#include <gpfont.h>
#include <gpmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fxi.h"

extern int fenix_runtime(char *filename);

char *fenix_runtime_error_msg=NULL;

static char *welcome_str="Welcome to GP32 Fenix Runtime!";
static char *path_str="Fenix DCB Path:";
static char *exec_str="DCB File:";
static char *executing_str="Executing...";
static char *error_str="FENIX RUNTIME ERROR";
static char *error_bug_str="BUG !";
static char *press_str="Press any button to reboot";

char *dcb_path="gp:\\gpmm\\fenix\0\0\0\0\0";
char *dcb_exec="test.dat\0\0\0\0\0\0\0\0";
char loader_memtag=0;

static GPDRAWSURFACE gpDraw;
static int draw_text_y=0;

static void fenix_gp32_runtime_init(void)
{
	if (!loader_memtag)
	{
	  char *p=GpAppPathGet(NULL);

	  if (p!=NULL)
		if (strlen(p)>2)
		{
			unsigned int i,j;
			dcb_path=p;
			for(i=strlen(p)-1;i>=0;i--)
				if (p[i]=='\\')
					break;
			for(i++,j=0;i<strlen(p);i++,j++)
				dcb_exec[j]=p[i];
			dcb_exec[j++]='.';
			dcb_exec[j++]='d';
			dcb_exec[j++]='c';
			dcb_exec[j++]='b';
			dcb_exec[j]=0;
		}
	}
	GpGraphicModeSet(8, NULL);
	GpLcdSurfaceGet(&gpDraw,0);
	GpSurfaceSet(&gpDraw);
	GpLcdEnable();
	GpRectFill(NULL, &gpDraw, 0, 0, 320, 240, 1);
	draw_text_y=0;
}


static void fenix_gp32_runtime_output(char *msg)
{
	unsigned int i;
	GpRectFill(NULL, &gpDraw, 0, draw_text_y, 320, 36, 1);
	if (msg!=NULL)
		for(i=0;i<strlen(msg);i++)
		{
			char c=msg[i+1];
			msg[i+1]=0;
			GpTextOut(NULL, &gpDraw, 2, draw_text_y+2, msg, 0xFF);
			msg[i+1]=c;
			GpThreadSleep(3);
		}
	GpThreadSleep(50);
	draw_text_y+=18;
	if (draw_text_y>220)
		draw_text_y=0;
}

static void fenix_gp32_runtime_press(void)
{
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output(press_str);
	while(GpKeyGet()!=GPC_VK_NONE);
	while(GpKeyGet()==GPC_VK_NONE);
}

static char _tmp_str[200];

static void fenix_gp32_runtime_output_num(char *msg, int num)
{
	memset(&_tmp_str,100,0);
	sprintf((void *)&_tmp_str,"%s %i",msg,num);
	fenix_gp32_runtime_output((char *)&_tmp_str);
}

static void fenix_gp32_runtime_output_str(char *msg, char *str)
{
	memset(&_tmp_str,100,0);
	sprintf((void *)&_tmp_str,"%s %s",msg,str);
	fenix_gp32_runtime_output((char *)&_tmp_str);
}

static int fenix_gp32_runtime_run(void)
{
	FILE *f=fopen("kk345o","rb"); // DUMMY FAT INIT
	fclose(f);
	GpRelativePathSet(dcb_path);
	double_buffer=1;
	full_screen=0;
	GpThreadSleep(500);
	return fenix_runtime(dcb_exec);
}

void fsl_exit(int n)
{
	if (n)
	{
		fenix_gp32_runtime_init();
		fenix_gp32_runtime_output_num(error_str,n);
		if (fenix_runtime_error_msg!=NULL)
			fenix_gp32_runtime_output(fenix_runtime_error_msg);
		fenix_gp32_runtime_press();
	}
	asm("mov        r3,#0; bx       r3;");
}

static void fenix_gp32_runtime_view_error(char *str)
{
	fenix_gp32_runtime_init();
	fenix_gp32_runtime_output_num(error_str,1);
	fenix_gp32_runtime_output(str);
	fenix_gp32_runtime_press();
}

void GpMain(void *argv)
{
	fenix_gp32_runtime_init();
	fenix_gp32_runtime_output(welcome_str);
	fenix_gp32_runtime_output(VERSION);
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output_str(path_str,dcb_path);
	fenix_gp32_runtime_output_str(exec_str,dcb_exec);
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output(executing_str);
	fenix_gp32_runtime_run();
	fenix_gp32_runtime_view_error(error_bug_str);
	fsl_exit(0);
}
