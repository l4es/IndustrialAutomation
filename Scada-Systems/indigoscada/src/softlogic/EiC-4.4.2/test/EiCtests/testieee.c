#include <stdio.h>
#include <assert.h>
#include <string.h>

#define fail() printf("Failed at line %d\n",__LINE__);

unsigned u=2147483839;float f0=2147483648e0,f1=2147483904e0;
void T1()
{
  float f=u;
  assert(f != f0);
}

/*---------------------------------------------------*/
double normalize(double x)
{
    if(x==0)
	x=0;
    return x;
}

void T2()
{
    char b[9];
    sprintf(b,"%g",normalize(-0.0));
    assert(strcmp(b,"0") == 0);
}
/*-------------------------------------------------*/
void T3()
{
    union {
	double d;
	unsigned char c[8];
    } d;
    
    d.d = 1.0/7.0;
    
    if (sizeof (char) * 8 == sizeof (double))
    {
	if (d.c[0] == 0x92 && d.c[1] == 0x24 && d.c[2] == 0x49 && d.c[3] == 0x92
	    && d.c[4] == 0x24 && d.c[5] == 0x49 && d.c[6] == 0xc2 && d.c[7] == 0x3f
	    )
	    return;
	if (d.c[7] == 0x92 && d.c[6] == 0x24 && d.c[5] == 0x49 && d.c[4] == 0x92
	    && d.c[3] == 0x24 && d.c[2] == 0x49 && d.c[1] == 0xc2 && d.c[0] == 0x3f
	    )
	    return;
	fail();
    }
    
}
/*----------------------------------------------*/
void T4 ()
{
  union
    {
      double d;
      unsigned short i[sizeof (double) / sizeof (short)];
    } u;
  int a = 0;
  int b = -5;
  int j;

  u.d = (double) a / b;

  /* Look for the right pattern, but be sloppy since
     we don't know the byte order.  */
  for (j = 0; j < sizeof (double) / sizeof (short); j++)
    {
      if (u.i[j] == 0x8000)
        return;
    }
  fail();
}
/*-----------------------------------------------------*/

int main(void)
{
    T1();
    T2();
    T3();
    T4();
    return 0;
}

#ifdef EiCTeStS
main();
#endif
