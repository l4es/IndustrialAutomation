#include <stdio.h>

#define K 0xFFFFFFFF /* -1 in 32-bit, 2's compl. */

#define pass() printf("ISO C compliant, line %d\n",__LINE__)
#define fail() printf("Not ISO C compliant, line %d\n",__LINE__)

void T1()
{
    if(0<K)
	pass();
    else
	fail();
}

void T2()
{ 
    float f = 1;
    if(f)
	pass();
    else
	fail();
}

int main(void)
{
    T1();
    T2();
    return 0;
}


#ifdef EiCTeStS
main();
#endif



