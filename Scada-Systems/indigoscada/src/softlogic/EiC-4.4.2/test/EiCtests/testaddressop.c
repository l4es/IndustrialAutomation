#include <assert.h>
/*
  This module has several simple
  tests for the address declaration
  operator `@'. The main and really
  only purpose for this EiC feature, `@', is
  so that EiC can share data with
  builtin code. For example, from a
  a builtin function:
  
      int p;
       ...
      EiC_parseString("int p @ %ld;",(long)&p);
      
  The storage of p is within the scope of the
  the builtin function. This can be important
  when interfacing EiC to various libraries:

*/
   
void T1() 
{
    int a = 22; int b @ &a;
    assert(b == 22);
    b = 66;
    assert(a == 66);
}

void T2() 
{
    int aa[5] = {1,2,3,4,5};
    int ba[5] @ &aa;

    int i;

    for(i=0;i<sizeof(aa)/sizeof(int);++i)
	assert(ba[i] == i+1);
    
}

void T3()
{
    int *a;
    int *b @ &a;
    int c = 22;
    int d = 66;

    a = &c;

    assert(*a == c);
    assert(*b == c);

    a = &d;
    assert(*b = d);
    *b = 102;
    assert(d == 102);
}



void T7()
{
    int a = 55; int b @ &a;
    double c = 1.2; extern int b;

    assert(c == 1.2);
    assert(b == 55);

}

void T8()
{

    char *vec[] = {"hello", "world"};
    char *p[2]  @ &vec;

    assert(p[0] == vec[0]);
    assert(p[1] == vec[1]);

}

#if 0

/* the following functions at this stage fail */

int c = 55, *p = &c;
int *q @ &p;
int *arr[] = { q };

void T4()
{
    assert(arr[0] == p);
    assert(*arr[0] == c);
}



#endif

#if 0
void T5()
{
    int *arr[] = { q };
    assert(arr[0] == p);
    assert(*arr[0] == c);
}

void T6()
{

    int c, *p = &c;
    int *q @ &p;

    void *any = &q;

    assert(*(void**)any == q);


}

#endif

int main()
{
    T1();
    T2();
    T3();

    T7();
    T8();
    return 0;
}


#ifdef EiCTeStS
main();
#endif













