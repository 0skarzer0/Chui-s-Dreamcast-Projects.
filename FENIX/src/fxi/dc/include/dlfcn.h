#ifndef _DLFCN_H_
#define _DLFCN_H_

#ifdef __cplusplus
extern "C" {
#endif

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
	info->freeram=14*1024*1024;
	info->totalram=16*1024*1024;
	info->sharedram=0;
	info->bufferram=0;
	info->totalswap=0;
	info->freeswap=0;
	info->mem_unit=1;
	return 0;
}

struct utsname
  {
    /* Name of the implementation of the operating system.  */
    char sysname[8];

    /* Name of this node on the network.  */
    char nodename[8];

    /* Current release level of this implementation.  */
    char release[8];
    /* Current version level of this release.  */
    char version[8];

    /* Name of the hardware type the system is running on.  */
    char machine[8];
    char domainname[8];
  };


static inline int uname(struct utsname *name)
{
	name->sysname[0]='D';
	name->sysname[1]='C';
	name->sysname[4]=0;

	name->nodename[0]='0';
	name->nodename[1]=0;

	name->release[0]='2';
	name->release[1]='4';
	name->release[2]='6';
	name->release[3]=0;

	name->version[0]='2';
	name->version[1]=0;

	name->machine[0]='a';
	name->machine[1]=0;

	name->domainname[0]='b';
	name->domainname[1]=0;

	return 0;
}

// static inline void bzero(void *s, int n){ memset(s, 0, n);}

// static inline void *memmove(void *dest, const void *src, size_t n) { return memcpy(dest,src,n); }

static inline void *dlopen (__const char *__file, int __mode){ return NULL; }
static inline int dlclose (void *__handle){ return 0; }
static inline char *dlerror (void){ return "NO DLL SUPPORT"; }
static inline void *dlsym (void *__restrict __handle, __const char *__restrict __name){ return NULL; }

#ifdef __cplusplus
}
#endif


#endif


