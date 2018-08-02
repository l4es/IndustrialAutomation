#include <stdio.h>
#include <limits.h>
#include <assert.h>

void Orig()
{


    char c; short s; int i; long l;  float f; double d;
    unsigned char uc; short unsigned us; unsigned ui; unsigned long
	ul;
    
    printf("\t--- EiC test signed values ---\n");
    
    c = -6;
    s = 0;
    i = 5;
    f = 2.6;
    l = -100;
    d = 0.9;
    printf("char c; short s; int i; long l;  float f; double d;\n\n"
	   "c = %d s = %d i = %d l = %ld f = %f d = %lf :->\n"
	   "c = -6 s = 0 i = 5 l = -100 f = 2.6 d = 0.9\n\n",c,s,i,l,f,d);
    
    printf("-c  = %d :-> -c = 6\n",-c);
    printf("~s  = %d :-> ~s = -1\n",~s);
    printf("f * c = %g  :-> f * c = -15.6\n", f * c);
    printf("f / d = %f  :-> f / d = 2.88889\n", f / d);
    printf("l / f = %f  :-> l / f = -38.4615\n", l / f);
    printf("i << 2 = %d :-> i << 2 = 20\n", i << 2);
    

    printf("\t--- EiC test unsigned values ---\n");


    uc = 0;
    us = 0;
    ui = 0;
    ul = 0;
    printf("unsigned char uc; short unsigned us; unsigned ui; unsigned long ul;\n\n"
	   "uc = %d us = %d  ui = %d  ul = %ld:->\n"
	   "uc = 0 us = 0  ui = 0 ul = 0\n\n",uc,us,ui,ul);

    /*    assert(~uc == UCHAR_MAX);
     *	  assert(~us == USHRT_MAX);
     */	  
    assert(~ui == UINT_MAX);
    assert(~ul == ULONG_MAX);
}

void T()
{
    assert('\377' == -1);
}

int main()
{
    Orig();
    T();
    return 0;
}


#ifdef EiCTeStS
main();
#endif 










