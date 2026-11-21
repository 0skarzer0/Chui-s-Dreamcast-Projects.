#ifndef _GPWRAP_H_
#define _GPWRAP_H_

#ifdef __cplusplus
extern "C" {
#endif


// #warning GPWRAP.........................

#include <gpdef.h>
#include <gpstdio.h>
#include <gpstdlib.h>
#include <gpmem.h>
#include <gpos_def.h>


#define EOF     (-1)
#define RAND_MAX 0x7fffffff
#define stdin NULL
#define stdout NULL
#define stderr NULL

static inline void *calloc(size_t nmemb, size_t size) { return gm_zi_malloc(nmemb*size); }
static inline void free(void *ptr){ gm_free(ptr); }
static inline void *malloc(size_t size){ return gm_zi_malloc(size);}
static inline void memset(void *ptr, unsigned char val, unsigned int size){ gm_memset(ptr,val,size); }
static inline void *memcpy(void *s1, const void *s2, unsigned int size){ return gm_memcpy(s1,s2,size); }
static inline char *strcpy(char *s1, const char *s2){ return gm_strcpy(s1,s2); }
static inline char *strncpy(char *s1, const char *s2, unsigned int size){ return gm_strncpy(s1,s2,size); }
static inline char *strcat(char *s1, const char *s2){ return gm_strcat(s1,s2); }
static inline size_t strlen(const char *s) { return ((size_t)gm_lstrlen(s)); };
static inline int strcmp(const char *s1, const char *s2){ return gm_compare(s1,s2); }
static inline int strncmp(const char *s1, const char *s2, size_t n)
{
	int ret;
	char *tmp_s1=(char *)calloc(1,n+16);
	char *tmp_s2=(char *)calloc(1,n+16);
	strncpy(tmp_s1,s1,n);
	strncpy(tmp_s2,s2,n);
	ret=strcmp(tmp_s1,tmp_s2);
	free(tmp_s2);
	free(tmp_s1);
	return ret;
}
static inline int memcmp(const void *s1, const void *s2, size_t n){ return strncmp((const char *)s1,(const char *)s2,n); }


extern int ___sdl__gp32_fat_initialize__;

typedef F_HANDLE FILE;

extern FILE __sdl__gp32_files__[];
extern int __sdl__gp32_files_pointer__;

static inline FILE *fopen(char *path, const char *mode)
{
	int i,j;
	FILE *ret=(FILE *)&__sdl__gp32_files__[__sdl__gp32_files_pointer__];
	ulong fopen_mode=OPEN_R;
	if (!___sdl__gp32_fat_initialize__)
	{
		___sdl__gp32_fat_initialize__=-1;
		GpFatInit();
	}
	for(i=0;i<strlen(path);i++)
		if (path[i]=='/')
			path[i]='\\';
		else if (path[i]>='A' && path[i]<='Z')
			path[i]+='a'-'A';
	j=strlen(mode);
	for(i=0;i<j;i++)
		if (mode[i]=='w')
		{
			if (GpFileCreate(path,ALWAYS_CREATE,ret)!=SM_OK)
				return NULL;
			else
			{
				__sdl__gp32_files_pointer__++;
				return ret;
			}
		}
	if (GpFileOpen(path,fopen_mode,ret)!=SM_OK)
		return NULL;
	__sdl__gp32_files_pointer__++;
	return ret;	
}

static inline int fclose(FILE *stream){ GpFileClose((*stream)); return 0; }

static inline size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	ulong buf_size=size*nmemb;
	ulong p_read_count=0;
	if (stream!=NULL)
		GpFileRead((*stream),ptr,buf_size,&p_read_count);
	return (p_read_count/size);
}

static inline size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	ulong count=size*nmemb;
	if (stream!=NULL)
		if (GpFileWrite((*stream),ptr,count)!=SM_OK)
			return 0;
	return (count/size);
}

#define SEEK_SET FROM_BEGIN
#define SEEK_CUR FROM_CURRENT
#define SEEK_END FROM_END

static inline int fseek(FILE *stream, long offset, int whence)
{
	if (GpFileSeek((*stream),whence,offset,NULL)!=SM_OK)
		return -1;
	return 0;
}

static inline long ftell(FILE *stream)
{
	long p_old_offset=0;
	
	if (GpFileSeek((*stream),FROM_CURRENT,0,&p_old_offset)!=SM_OK)
		return -1;
	return p_old_offset;
}

static inline char *getenv(const char *name){ return NULL; }

#define sprintf gm_sprintf

static inline char *strchr(const char *s, int c)
{
	char *ret=(char *)s;
	size_t i,l=strlen(s);
	for(i=0;i<l;i++,ret++)
		if ((*ret)==c)
			return (char *)ret;
	return NULL;
}

static inline char *strrchr(const char *s, int c)
{
	size_t i,l=strlen(s);
	char *ret=(char *)s;
	if (l==0)
		return NULL;
	l--;
	ret+=l;
	for(i=l;i>=0;i--,ret--)
		if ((*ret)==c)
			return (char *)ret;
	return NULL;
}

static inline int fgetc(FILE *stream)
{
	int ret=0;
	if (fread((void *)&ret,1,1,stream)!=1)
		return EOF;
	return ret;
}

static inline int feof(FILE *stream)
{
	long end,pos=ftell(stream);
	fseek(stream,0,SEEK_END);
	end=ftell(stream);
	fseek(stream,pos,SEEK_SET);
	if (pos!=end)
		return 0;
	return EOF;
}

static inline char *strdup(const char *s)
{
	char *ret=NULL;
	size_t l=strlen(s);
	if (l>0)
	{
		ret=(char *)calloc(l+1,1);
		strcpy(ret,s);
	}
	return ret;
}


static inline void *realloc(void *ptr, size_t size)
{
	void *ret=NULL;
	if (ptr==NULL)
		return malloc(size);
	if (size>0)
	{
		ret=malloc(size);
		if (ret==NULL)
			return NULL;
		memcpy(ret,ptr,size);
	}
	free(ptr);
	return ret;
}

static inline int printf(const char *format, ...){ return strlen(format);}
static inline int fprintf(FILE *stream, const char *format, ...){ return strlen(format);}

static inline void clearerr( FILE *stream){ ;}

static inline int strcasecmp(const char *s1, const char *s2)
{
	int ret=0;
	int ls1=strlen(s1);
	int ls2=strlen(s2);
	if ((ls1>0)&&(ls2>0))
	{
		char *ns1=(char *)calloc(1,ls1+16);
		char *ns2=(char *)calloc(1,ls2+16);
		memcpy(ns1,s1,ls1);
		memcpy(ns2,s2,ls2);
		gm_lowercase(ns1,ls1);
		gm_lowercase(ns2,ls2);
		ret=strcmp(ns1,ns2);
		free(ns2);
		free(ns1);
	}
	return ret;
}

static inline void srand(unsigned int seed){ GpSrand(seed); }

static inline int rand(void){ return GpRand();}

static inline int abs(int j)
{
	if (j<0)
		return -j;
	return j;
}

static inline int fputc(int c, FILE *stream)
{
	unsigned char ret=c;
	if (fwrite((void *)&ret,1,1,stream)!=1)
		return EOF;
	return ret;
}

static inline int fputs(const char *s, FILE *stream)
{
	unsigned int l=strlen(s);
	if (l>0)
		if (fwrite(s,1,l,stream)!=l)
			return EOF;
	return l;
}

static inline int ferror(FILE *stream){ return 0; }
static inline int fflush(FILE *stream){ return 0; }
static inline int rewind(FILE *stream){ return fseek(stream,0,SEEK_SET);}

extern int sscanf(const char *str, const char *format, ...);
//static inline int vsprintf(char *str, const char *format, ...) {return strlen(format);}
extern int vsprintf(char *str, const char *format, ...);

static inline int fileno(FILE *stream){ return -1; }

static inline char *fgets(char *s, int size, FILE *stream)
{
	int i,tmp;
	if (s==NULL)
		return NULL;
	memset(s,0,size);
	for(i=0;i<size;i++)
	{
		tmp=fgetc(stream);
		if (tmp!=EOF)
		{
			if (tmp=='\n')
				return s;
			else
				s[i]=tmp;
		}
		else
			if (i==0)
				return NULL;
			else
				return s;
	}
	return s;
}

static inline int putchar(int c){ return 0;}

static inline char *getcwd(char *buf, size_t size)
{
	GpRelativePathGet(buf);
	return buf;
}

extern int atoi(const char *nptr);
extern char *strtok(char *s, const char *delim);

#define RTLD_NOW    0x00000001
#define RTLD_GLOBAL 0x00000000

extern double atof(const char *nptr);

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
	info->freeram=gm_availablesize();
	info->totalram=8*1024*1024;
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
	name->sysname[0]='G';
	name->sysname[1]='P';
	name->sysname[2]='3';
	name->sysname[3]='2';
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

static inline void bzero(void *s, int n){ memset(s, 0, n);}

static inline void *memmove(void *dest, const void *src, size_t n) { return memcpy(dest,src,n); }

static inline void *dlopen (__const char *__file, int __mode){ return NULL; }
static inline int dlclose (void *__handle){ return 0; }
static inline char *dlerror (void){ return "NO DLL SUPPORT"; }
static inline void *dlsym (void *__restrict __handle, __const char *__restrict __name){ return NULL; }

static inline int chdir(char *path)
{
	int i;
	if (path==NULL)
		return 0;
	if (path[0]==0)
		return 0;
	for(i=0;i<strlen(path);i++)
		if (path[i]=='/')
			path[i]='\\';
		else if (path[i]>='A' && path[i]<='Z')
			path[i]+='a'-'A';
	if (strncmp(path,"gp:\\",4))
	{
		int ret;
		char *tmp=calloc(1024,1);
		GpRelativePathGet(tmp);
		strcpy(tmp+strlen(tmp),path);
		ret=GpRelativePathSet(tmp);
		free(tmp);
		return ret;
	}
	return GpRelativePathSet(path);
}

static inline int mkdir(char *path)
{
	int i;
	if (path==NULL)
		return 0;
	if (path[0]==0)
		return 0;
	for(i=0;i<strlen(path);i++)
		if (path[i]=='/')
			path[i]='\\';
		else if (path[i]>='A' && path[i]<='Z')
			path[i]+='a'-'A';
	if (strncmp(path,"gp:\\",4))
	{
		int ret;
		char *tmp=calloc(1024,1);
		GpRelativePathGet(tmp);
		strcpy(tmp+strlen(tmp),path);
		ret=GpDirCreate(tmp,0);
		free(tmp);
		return ret;
	}
	return GpDirCreate(path,0);
}

static inline int rmdir(char *path)
{
	int i;
	if (path==NULL)
		return 0;
	if (path[0]==0)
		return 0;
	for(i=0;i<strlen(path);i++)
		if (path[i]=='/')
			path[i]='\\';
		else if (path[i]>='A' && path[i]<='Z')
			path[i]+='a'-'A';
	if (strncmp(path,"gp:\\",4))
	{
		int ret;
		char *tmp=calloc(1024,1);
		GpRelativePathGet(tmp);
		strcpy(tmp+strlen(tmp),path);
		ret=GpDirRemove(tmp,ALWAYS_DELETE);
		free(tmp);
		return ret;
	}
	return GpDirRemove(path,ALWAYS_DELETE);
}

/*
extern void __fenix__qsort(void *base, size_t nmiemb, size_t tam, int (*compar)(const void *, const void *));

#define qsort(BASE,NUM,SIZE,COMP) (__fenix__qsort(BASE,NUM,SIZE,COMP))
*/
extern void qsort(void *base, size_t nmiemb, size_t tam, int (*compar)(const void *, const void *));


#ifdef __cplusplus
}
#endif


#endif


