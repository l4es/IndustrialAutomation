#include "forgnuport.h"

struct foo
{
  int a, b, c, d;
  double doubl;
} s1, s2;

struct foo structret (struct foo s1,
		      int i1, int i2,
		      struct foo s2)
{
  if (i1 != i2)
    {
      if (i1 < i2)
        return s1;
      else
        return s2;
    }
  s2.a = 11;
  s2.b = 22;
  s2.c = s1.c;
  s2.d = s1.d;
  return s2;
}

struct foo mani (int a, int b)
{
  return structret (s1, a, b, s2);
}

void init ()
{
  s1.a = 1;
  s1.b = 2;
  s1.c = 3;
  s1.d = 4;
  s1.doubl = 3.1415;
  s2.a = -1;
  s2.b = -2;
  s2.c = -3;
  s2.d = -4;
  s2.doubl = 2.71818;
}

int  main (void)
{
  struct foo s;

  init ();
  s = mani (1, 1);
  printf ("%d, %d, %d, %d : %f\n", s.a, s.b, s.c, s.d, s.doubl);

  init ();
  s = mani (2, 1);
  printf ("%d, %d, %d, %d : %f\n", s.a, s.b, s.c, s.d, s.doubl);

  init ();
  s = mani (1, 2);
  printf ("%d, %d, %d, %d : %f\n", s.a, s.b, s.c, s.d, s.doubl);

  return 0;
}

#ifdef EiCTeStS
main();
#endif

