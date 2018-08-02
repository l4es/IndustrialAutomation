#include <assert.h>


char a[] = "adsf";  
char *b = a; 
char* c = &a[0];
char *d;


assert(a == b && b == c);
d = a;

assert(d == a);
