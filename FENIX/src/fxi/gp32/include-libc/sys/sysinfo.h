#ifndef _SYSINFO_H_
#define _SYSINFO_H_

#define RTLD_NOW    0x00000001
#define RTLD_GLOBAL 0x00000000


struct sysinfo {
	long uptime;              /* Segundos desde el arranque */
	unsigned long loads[3];   /* cargas medias en 1, 5, y 15 min */
	unsigned long totalram;   /* Mem. pral. total útil */
	unsigned long freeram;    /* Tamaño de memoria disponible */
	unsigned long sharedram;  /* Cantidad de memoria compartida */
	unsigned long bufferram;  /* Memoria empleaada por búferes */
	unsigned long totalswap;  /* Tamaño del espacio total de swap */
	unsigned long freeswap;   /* Espacio de swap aún disponible */
	unsigned short procs;     /* Nº de procesos actualmente */
	unsigned short pad;       /* explicit padding for m68k */
	unsigned long totalhigh;  /* Total high memory size */
	unsigned long freehigh;   /* Available high memory size */
	unsigned int mem_unit;    /* Memory unit size in bytes */
	char _f[8];               /* Rellena la struct a 64 bytes */
};

static inline int sysinfo(struct sysinfo *info){
	info->freeram=7*1024*1024;
	info->totalram=8*1024*1024;
	info->sharedram=0;
	info->bufferram=0;
	info->totalswap=0;
	info->freeswap=0;
	info->mem_unit=1;
	return 0;
}

#endif
