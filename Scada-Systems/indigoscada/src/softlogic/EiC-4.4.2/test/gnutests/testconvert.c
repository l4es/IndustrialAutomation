/* Test front-end conversions, optimizer conversions, and run-time
   conversions between different arithmetic types.

   Constants are specified in a non-obvious way to make them work for
   any word size.  Their value on a 32-bit machine is indicated in the
   comments.

   Note that this code is NOT intended for testing of accuracy of fp
   conversions.  */


#include <stdio.h>
#include <stdlib.h>

#define abort() printf("Failure on line %d\n", __LINE__); 

float u2f(unsigned int u)
{
  return u;
}

double u2d(unsigned int u)
{
  return u;
}

float s2f(int s)
{
  return s;
}

double s2d(int s)
{
  return s;
}

int fnear (float x, float y)
{
  float t = x - y;
  return t == 0 || x / t > 1000000.0;
}

int dnear (double x, double y)
{
  double t = x - y;
  return t == 0 || x / t > 100000000000000.0;
}

void test_integer_to_float()
{
  if (u2f(0U) != (float) 0U)				/* 0 */
    abort();
  if (!fnear (u2f(~0U), (float) ~0U))			/* 0xffffffff */
    abort();
  if (!fnear (u2f((~0U) >> 1), (float) ((~0U) >> 1)))	/* 0x7fffffff */
    abort();
  if (u2f(~((~0U) >> 1)) != (float) ~((~0U) >> 1))	/* 0x80000000 */
    abort();

  if (u2d(0U) != (double) 0U)				/* 0 */
    abort();
  if (!dnear (u2d(~0U), (double) ~0U))			/* 0xffffffff */
    abort();
  if (!dnear (u2d((~0U) >> 1),(double) ((~0U) >> 1)))	/* 0x7fffffff */
    abort();
  if (u2d(~((~0U) >> 1)) != (double) ~((~0U) >> 1))	/* 0x80000000 */
    abort();

  if (s2f(0) != (float) 0)				/* 0 */
    abort();
  if (!fnear (s2f(~0), (float) ~0))			/* 0xffffffff */
    abort();
  if (!fnear (s2f((int)((~0U) >> 1)), (float)(int)((~0U) >> 1))) /* 0x7fffffff */
    abort();
  if (s2f((int)(~((~0U) >> 1))) != (float)(int)~((~0U) >> 1)) /* 0x80000000 */
    abort();

  if (s2d(0) != (double) 0)				/* 0 */
    abort();
  if (!dnear (s2d(~0), (double) ~0))			/* 0xffffffff */
    abort();
  if (!dnear (s2d((int)((~0U) >> 1)), (double)(int)((~0U) >> 1))) /* 0x7fffffff */
    abort();
  if (s2d((int)~((~0U) >> 1)) != (double)(int)~((~0U) >> 1)) /* 0x80000000 */
    abort();
}

unsigned int
f2u(float f)
{
  return (unsigned) f;
}

unsigned int
d2u(double d)
{
  return (unsigned) d;
}

int
f2s(float f)
{
  return (int) f;
}

int
d2s(double d)
{
  return (int) d;
}

void test_float_to_integer()
{
  if (f2u(0.0) != 0)
    abort();
  if (f2u(0.999) != 0)
    abort();
  if (f2u(1.0) != 1)
    abort();
  if (f2u(1.99) != 1)
    abort();
  if (f2u((float) ((~0U) >> 1)) != (~0U) >> 1 &&	/* 0x7fffffff */
      f2u((float) ((~0U) >> 1)) != ((~0U) >> 1) + 1)
    abort();
  if (f2u((float) ~((~0U) >> 1)) != ~((~0U) >> 1))	/* 0x80000000 */
    abort();

  if (d2u(0.0) != 0)
    abort();
  if (d2u(0.999) != 0)
    abort();
  if (d2u(1.0) != 1)
    abort();
  if (d2u(1.99) != 1)
    abort();
  if (d2u((double) (~0U)) != ~0U)			/* 0xffffffff */
    abort();
  if (d2u((double) ((~0U) >> 1)) != (~0U) >> 1)		/* 0x7fffffff */
    abort();
  if (d2u((double) ~((~0U) >> 1)) != ~((~0U) >> 1))	/* 0x80000000 */
    abort();


  if (f2s(0.0) != 0)
    abort();
  if (f2s(0.999) != 0)
    abort();
  if (f2s(1.0) != 1)
    abort();
  if (f2s(1.99) != 1)
    abort();
  if (f2s(-0.999) != 0)
    abort();
  if (f2s(-1.0) != -1)
    abort();
  if (f2s(-1.99) != -1)
    abort();
  if (f2s((float)(int)~((~0U) >> 1)) != (int)~((~0U) >> 1)) /* 0x80000000 */
    abort();

  if (d2s(0.0) != 0)
    abort();
  if (d2s(0.999) != 0)
    abort();
  if (d2s(1.0) != 1)
    abort();
  if (d2s(1.99) != 1)
    abort();
  if (d2s(-0.999) != 0)
    abort();
  if (d2s(-1.0) != -1)
    abort();
  if (d2s(-1.99) != -1)
    abort();
  if (d2s((double) ((~0U) >> 1)) != (~0U) >> 1)		/* 0x7fffffff */
    abort();
  if (d2s((double)(int)~((~0U) >> 1)) != (int)~((~0U) >> 1)) /* 0x80000000 */
    abort();
}



int main(void)
{

    test_integer_to_float();
    test_float_to_integer();
    return 0;
}


#ifdef EiCTeStS
main();
#endif
