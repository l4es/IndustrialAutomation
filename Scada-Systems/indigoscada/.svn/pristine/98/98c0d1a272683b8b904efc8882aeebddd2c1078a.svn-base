#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

void T1()
{
#define max(a,b)    ((a)>(b)?(a):(b))
#define min(a,b)    ((a)<(b)?(a):(b))
#define cat(a,b)    a##b
#define xcat(a,b)   cat(a,b)
    
    float a = 6;
    assert(max(-5,7) == 7);
    assert(min(-5.2,6) == -5.2);
    assert(a * max(-5,7) == 42);
    assert(max(-5,7) * a == 42);
    assert(min(-5.2,6) * max(-5,7) == -36.4);
    assert(max(max(3,4),max(6.5,-3.2)) == 6.5);

    assert(cat(1,2) == 12);
    assert(xcat(xcat(1,2),3) == 123);

#undef cat
#undef xcat    
#undef max
#undef min    
}

void T2()
{

    char c; short s; int i; long l;  float f; double d;
    
    c = -6;
    s = 0;
    i = 5;
    f = 7;
    l = -70;
    d = 2;

    assert((unsigned char) c == 250);

    assert(c == -6);
    assert(s == 0);
    assert(i == 5);
    assert(l == -70);
    assert(d == 2);
    
    assert(-c == 6);
    assert(~s == -1);
    assert(f * c == -42);
    assert(f / d == 3.5);
    assert(l / f == -10);
    assert(i << 2 == 20);
    	
}

void T3()
{
    char str[20];
    unsigned char uc;
    short unsigned us;
    unsigned ui;
    unsigned long ul;
    
    char sc; short ss; int si; long sl;

    char __;
    __ = 'A';
    
    assert(__ == 'A');
    
    sc = uc = 0;
    ss = us = 0;
    si = ui = 0;
    sl = ul = 0;
    
    assert(~uc ==  UINT_MAX);
    assert(~us ==  UINT_MAX);
    assert(~ui ==  UINT_MAX);
    assert(~ul == ULONG_MAX);
    
    assert(~sc ==  UINT_MAX);
    assert(~ss ==  UINT_MAX);
    assert(~si ==  UINT_MAX);
    assert(~sl ==  ULONG_MAX);

    sprintf(str,"%d%o%x%c",44,44,44,44);
    assert(strcmp("44542c,",str) == 0);

}

int fact(int n) { return n == 1 ? 1: n * fact(n-1); }

float test_sum(float x)
{
    return x + x;
}

void T4()
{
    float (*f)(float x);
    f = test_sum;
    assert((*f)(1.5) == 3);
    assert(fact(6) == 720);
}


void T5()
{
    assert(1.0 * 8 / 16 == 0.5);
    assert(8 * 1.0 / 16 == 0.5);
}

int main()
{
    T1();
    T2();
    T3();
    T4();
    T5();
    return 0;
}

#ifdef EiCTeStS
main();
#endif














