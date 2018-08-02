/*
   testpreperr.c
   tries to generate as many as possible errors by the preprocessor
   section of EiC - to see how robust it is !!
*/

#if defined LNX                 
#undef LNX
#endif

#include #include
#if

#define `'macro(a,b) (a) - b)

#ifdef ~#if #else
#~##ifdefine
#define #undef #define
#define #define #undef
#define

#define PI PI 3.14159

#ifdef 33		/* error: Illegal macro identifier */
#endif
#include "stdio.h
#include <stdio.h


#define TESTY 33
#define TESTY 33
#define TESTY 34	/* error: Re-declaration of macro TESTY */
#else			/* error: Unmatched #else */

#endif            	/* error: Unmatched #endif */



#define abc(x)  ## x
#define xyz(x)  x ##

"   
/* N.B strings literals do not contain newlines  */

"  

#hello    /* error: undefined or invalid # directive */















