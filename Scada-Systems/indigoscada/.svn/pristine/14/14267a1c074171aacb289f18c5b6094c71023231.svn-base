#include "forgnuport.h"

int div1 (
     signed char x)
{
  return x / -1;
}

int div2 (
     signed short x)
{
  return x / -1;
}

int div3 (
     signed char x,
     signed char y)
{
  return x / y;
}

int div4 (
     signed short x,
     signed short y)
{
  return x / y;
}

int mod1 (
     signed char x)
{
  return x % -1;
}

int mod2 (
     signed short x)
{
  return x % -1;
}

int mod3 (
     signed char x,
     signed char y)
{
  return x % y;
}

int mod4 (
     signed short x,
     signed short y)
{
  return x % y;
}

int main (void)
{
  if (div1 (-(1 << 7)) != 1 << 7)
    abort ();
  if (div2 (-(1 << 15)) != 1 << 15)
    abort ();
  if (div3 (-(1 << 7), -1) != 1 << 7)
    abort ();
  if (div4 (-(1 << 15), -1) != 1 << 15)
    abort ();
  if (mod1 (-(1 << 7)) != 0)
    abort ();
  if (mod2 (-(1 << 15)) != 0)
    abort ();
  if (mod3 (-(1 << 7), -1) != 0)
    abort ();
  if (mod4 (-(1 << 15), -1) != 0)
    abort ();
  return 0;
}


#ifdef EiCTeStS
main();
#endif



