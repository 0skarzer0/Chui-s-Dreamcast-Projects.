#ifndef _DLFCN_H_
#define _DLFCN_H_

static inline void *dlopen (__const char *__file, int __mode){ return NULL; }
static inline int dlclose (void *__handle){ return 0; }
static inline char *dlerror (void){ return "NO DLL SUPPORT"; }
static inline void *dlsym (void *__restrict __handle, __const char *__restrict __name){ return NULL; }


#endif
