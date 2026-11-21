#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x_gp32.h>

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

char *dcb_path="/gpmm/fenix\0\0\0\0\0";
char *dcb_exec="test.dat\0\0\0\0\0\0\0\0";
char loader_memtag=0;
char cant_say_me=0;

static int draw_text_y=0;

static void fenix_gp32_runtime_init(void)
{
        register char *t=(char *)(0x0c77ff00);
        register int i;

        if (t[0]==123)
	{
		loader_memtag=-1;
        	for(i=0;(i<8) && (t[i]);i++)
        	{
               		dcb_path[6+i]=t[i+1];
                	dcb_exec[i]=t[i+1];
        	}
        	dcb_path[6+i]=0;
		if (!dcb_exec[i-1])
			i--;
       		dcb_exec[i]='.';
        	dcb_exec[i+1]='d';
        	dcb_exec[i+2]='c';
        	dcb_exec[i+3]='b';
        	dcb_exec[i+4]=0;
	}
	else
		if ((dcb_exec[0]!='t') || (dcb_exec[1]!='e') || (dcb_exec[2]='s') ||
		    (dcb_exec[3]!='t') || (dcb_exec[4]!='.') || (dcb_path[6]='f') ||
		    (dcb_path[7]!='e') || (dcb_path[8]!='n') || (dcb_path[9]='i') ||
		    (dcb_path[10]!='x') || (dcb_path[11]!=0))
			cant_say_me=1;
	t[0]=0;
	draw_text_y=0;
	t=(char *)(0x0c77ff11);
	if (t[0]==35)
		cant_say_me=1;
	else
	if (t[0]!=34)
		memset(SCREEN_BUFFER_1,1,320*240*2);
	else
	{
		x_gp32_initFramebuffer(SCREEN_BUFFER_1,16,85);
		memcpy(SCREEN_BUFFER_1,(0x0c500000),320*240*2);
		SDL_Delay(1876);
		cant_say_me=1;
	}
	t[0]=0;
}


static void fenix_gp32_runtime_clean(void)
{
	Uint8 *d=SCREEN_BUFFER_1+239-draw_text_y;
	int i,h;

	if (cant_say_me) return;
	for(h=draw_text_y;h<(draw_text_y+36);h++,d=SCREEN_BUFFER_1+239-h)
		for(i=0;i<320;i++,d+=240)
			*d=1;
}

static void fenix_gp32_runtime_sleep(int n)
{
	unsigned int t=x_gp32_timer_counter+n;
	while(t>x_gp32_timer_counter)
		x_gp32_NOP();
}


static void fenix_gp32_runtime_output(char *msg)
{
	unsigned int i;
	if (cant_say_me) return;
	fenix_gp32_runtime_clean();
	if (msg!=NULL)
		for(i=0;i<strlen(msg);i++)
		{
			char c=msg[i+1];
			msg[i+1]=0;
			x_gp32_TextOut (2,draw_text_y+2,msg,0xFF);
			msg[i+1]=c;
			fenix_gp32_runtime_sleep(1);
		}
	fenix_gp32_runtime_sleep(4);
	draw_text_y+=18;
	if (draw_text_y>220)
		draw_text_y=0;
}

static void fenix_gp32_runtime_press(void)
{
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output(press_str);
	do{
		x_gp32_ReadKeys();
	}while(!x_gp32_nKeys);
	while(x_gp32_nKeys)
		x_gp32_ReadKeys();
}

static char _tmp_str[200];

static void fenix_gp32_runtime_output_num(char *msg, int num)
{
	if (cant_say_me) return;
	memset(&_tmp_str,100,0);
	sprintf((void *)&_tmp_str,"%s %i",msg,num);
	fenix_gp32_runtime_output((char *)&_tmp_str);
}

static void fenix_gp32_runtime_output_str(char *msg, char *str)
{
	if (cant_say_me) return;
	memset(&_tmp_str,100,0);
	sprintf((void *)&_tmp_str,"%s %s",msg,str);
	fenix_gp32_runtime_output((char *)&_tmp_str);
}

static int fenix_gp32_runtime_run(void)
{
	cant_say_me=0;
	chdir(dcb_path);
	double_buffer=1;
	full_screen=0;
	fenix_gp32_runtime_sleep(15);
	return fenix_runtime(dcb_exec);
}

void fsl_exit(int n)
{
	if (n)
	{
		x_gp32_initFramebuffer(SCREEN_BUFFER_1,8,50);
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
	x_gp32_initFramebuffer(SCREEN_BUFFER_1,8,50);
	fenix_gp32_runtime_init();
	fenix_gp32_runtime_output_num(error_str,1);
	fenix_gp32_runtime_output(str);
	fenix_gp32_runtime_press();
}

int main(int argn, char **argv)
{
	x_gp32_SetCPUSpeed_133();
	fenix_gp32_runtime_init();
	fenix_gp32_runtime_output(welcome_str);
	fenix_gp32_runtime_output(VERSION);
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output_str(path_str,dcb_path);
	fenix_gp32_runtime_output_str(exec_str,dcb_exec);
	fenix_gp32_runtime_output(NULL);
	fenix_gp32_runtime_output(executing_str);
	fenix_gp32_runtime_run();
	x_gp32_initFramebuffer(SCREEN_BUFFER_1,8,85);
	fenix_gp32_runtime_view_error(error_bug_str);
	fsl_exit(0);
	return 0;
}
