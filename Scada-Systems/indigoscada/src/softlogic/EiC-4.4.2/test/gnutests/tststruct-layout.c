#include "forgnuport.h"


typedef struct
{
  char a;
  int b;
  char c;
  short d;
  double e;
  char f;
} T;


void f (T *x)
{
  x[0].a = 'a';
  x[0].b = 47114711;
  x[0].c = 'c';
  x[0].d = 1234;
  x[0].e = 3.141592897932;
  x[0].f = '*';

  x[1].a = 'A';
  x[1].b = 71417141;
  x[1].c = 'C';
  x[1].d = 4321;
  x[1].e = 2.718281828459;
  x[1].f = '?';
}

void g (T *x)
{
  if (x[0].a != 'a')
    abort ();
  if (x[0].b != 47114711)
    abort ();
  if (x[0].c != 'c')
    abort ();
  if (x[0].d != 1234)
    abort ();
  if (x[0].e != 3.141592897932)
    abort ();
  if (x[0].f != '*')
    abort ();

  if (x[1].a != 'A')
    abort ();
  if (x[1].b != 71417141)
    abort ();
  if (x[1].c != 'C')
    abort ();
  if (x[1].d != 4321)
    abort ();
  if (x[1].e != 2.718281828459)
    abort ();
  if (x[1].f != '?')
    abort ();
}

int main ()
{
  T x[2];
  f (x);
  g (x);
  return 0;
}


#ifdef EiCTeStS
main();
#endif




