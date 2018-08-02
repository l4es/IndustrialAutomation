#include "forgnuport.h"

void f(int *x)
{
  *x = 0;
}

char a[] = "c";
void  main()
{
  int s, c, x;

  f(&s);
  a[c = 0] = s == 0 ? (x=1, 'a') : (x=2, 'b');
  if (a[c] != 'a')
      abort();
  else
      printf("Passed\n");
}

#ifdef EiCTeStS
main();
#endif
