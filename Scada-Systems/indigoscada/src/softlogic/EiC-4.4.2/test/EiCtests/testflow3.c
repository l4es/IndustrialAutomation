#include <assert.h>

#define F  5


void T1()
{
  char c =F;
  unsigned char uc = F;
  short s =F;
  unsigned short us = F;
  int  i =F;
  unsigned int ui = F;
  long l = F;
  unsigned long ul = F;
  float f =F;
  double d = F;


  if(c) c --;
  if(uc) uc --;
  if(s) s --;
  if(us) us--;
  if(i) i--;
  if(ui) ui--;
  if(l) l--;
  if(ul) ul--;

  if(f) f -= 1;
  if(d) d -= 1;

  assert(c == F -1);
  assert(uc == F -1);
  assert(s == F -1);
  assert(us == F -1);
  assert(i == F -1);
  assert(ui == F -1);
  assert(l == F -1);
  assert(ul == F -1);
  assert(f == F -1);
  assert(d == F -1);

}

int main()
{
  T1();
  return 0;
}


#ifdef EiCTeStS
main();
#endif


