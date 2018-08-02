/*
 * This file contains statements
 *  that could  be entered directly from EiC's command line
 *  It purpose is to test this facility.
 */

#include <assert.h>

int i,x;

{ int i; for(i=0;i<10;i++) x = i;}

assert(x == 9);

{\
     int a[5], i; \
     a[0] = 1; a[1] = 2; a[2] = 3; a[3] = 4;\
     a[4] = 5;\
      for(x=i =0;i<sizeof(a)/sizeof(int);i++)
	 x+= a[i];\
}

assert(x == 1+2+3+4+5);

for(i=0;i<3;++i) { int a[5]; }  /* test export of free locals */ 
 
assert(i == 3);





