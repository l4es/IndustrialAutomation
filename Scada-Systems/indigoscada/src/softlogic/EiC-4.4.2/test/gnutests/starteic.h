
#ifndef GCC


#ifndef _EiC

:-I/home/edb/compilers/EiC/include
:-I/home/edb/compilers/EiC/test


#define _EiC
#define _LINUX

#endif


#include <stdio.h>
#define register

:memdump
1;  /* dummy constant */  



#else


#include <stdio.h>

void main()
{
    void c; void x[5];
}


#endif
