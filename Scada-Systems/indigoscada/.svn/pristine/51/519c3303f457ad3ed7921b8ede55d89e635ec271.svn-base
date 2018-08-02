#include <assert.h>
int a[2];

void f (int b)
{
  unsigned int i;
  for (i = 0; i < b; i++)
    a[i] = i - 2;
}

int main (void)
{
  a[0] = a[1] = 0;
  f (2);
  assert(a[0] == -2 && a[1] == -1);
  return 0;
}

#ifdef EiCTeStS
main();
#endif




