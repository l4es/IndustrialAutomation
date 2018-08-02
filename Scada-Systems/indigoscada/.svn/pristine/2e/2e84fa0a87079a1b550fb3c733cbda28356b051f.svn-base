/* teststru2.c
 *
 *	(C) Copyright Dec 25 1996, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

/*
 * This module tests EiC's ability to pass and return
 * structures.
 */

#include <assert.h>

typedef struct { int a, b; } ab_t;

ab_t s(int a, int b)
{
    ab_t A;
    A.a = a;
    A.b = b;
    return A;
}

ab_t T_(ab_t a, ab_t b)
{
    ab_t c;
    c.a = a.a + b.a;
    c.b = a.b + b.b;

    return c;
}

ab_t T1_()
{
   return T_(s(5,7),s(10,10));
}

void T1()
{
    ab_t x = T1_();
    assert(x.a == 15 && x.b == 17);
}

ab_t T2_()
{
    return T_(T_(s(5,6),s(10,11)),s(5,6));
}

void T2()
{
    ab_t x = T2_();
    assert(x.a == 20 && x.b == 23);
}

ab_t X(void)
{
    ab_t x = {55,66};
    return x;
}

int F(ab_t a)
{
    return a.a;
}

void T3()
{
    assert(F(X()) == 55);
    assert(F(X()) + F(X()) + F(X()) == 3 * 55);
    
}


ab_t T4_(ab_t x)
{
    x.a = 20;
    x.b = 30;
    return x;
}

void T4()
{
    ab_t y = s(5,5);
    ab_t x = T4_(y);
    assert(y.a == 5 );
    assert(y.b == 5);
    assert(x.a == 20 && x.b == 30);
}

void T5()
{
    ab_t y = {5,5};
    ab_t x = T4_(y);
    assert(y.a == 5 && y.b == 5);
}

typedef struct {
    int *p;
} int_t;

int T6_(int_t p)
{
    int *q;
    q = p.p;
    return *q;
}
 
void T6()
{
    int a = 5;
    int_t t;
    t.p = &a;
    assert(T6_(t) == a);
}

int main(void)
{
    T1();
    T2();
    T3();
    T4();
    T5();
    T6();
    
    return 0;
}

#ifdef EiCTeStS
main();
#endif


