#include "forgnuport.h"

typedef struct { int re; int im; } T;

T f (int x, int y);

T f (int arg1, int arg2)
{
  T x;
  x.re = arg1;
  x.im = arg2;
  return x;
}


int main (void)
{
  T result;
  result = f (3, 4);
  if (result.re != 3 || result.im != 4)
    abort ();
  return 0;
}


#ifdef EiCTeStS
main();
#endif
