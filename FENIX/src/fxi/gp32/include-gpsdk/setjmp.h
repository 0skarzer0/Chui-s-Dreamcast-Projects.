#ifndef _SETJMP_H_
#define _SETJMP_H_

#include<gpwrap.h>

#ifdef byte
#undef byte
#endif

typedef int jmp_buf;

static inline void longjmp(jmp_buf __jmpb, int __retval){}
static inline int setjmp(jmp_buf __jmpb){ return 0; }


#endif
