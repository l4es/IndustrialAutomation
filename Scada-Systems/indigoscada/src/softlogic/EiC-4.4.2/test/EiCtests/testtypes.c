#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

void T1()
{
    void *p = NULL;
    char  ca[2];
    short sa[2];
    int   ia[2];
    long  la[2];
    float fa[2];
    double da[2];
    
    char c = -6;
    int i;
    short s;
    long l;

    unsigned ui;
    unsigned short us;
    unsigned long ul;
    
    i = (unsigned char) c;
    s = (unsigned char) c;
    l = (unsigned char) c;
    ui = c;
    us = c;
    ul = c;

    ca[1] = c;
    sa[1] = c;
    ia[1] = c;
    la[1] = c;
    fa[1] = c;
    da[1] = c;


    assert(p == NULL);
    assert(NULL == p);
    assert(p >= NULL);
    
    assert(UCHAR_MAX + c + 1 == i);
    assert(UCHAR_MAX + c + 1 == s);
    assert(UCHAR_MAX + c + 1 == l);
    assert(UCHAR_MAX + c + 1 == (unsigned char)c);
    assert(USHRT_MAX + c + 1 ==  (unsigned short)c);
    assert(UINT_MAX + c + 1 ==  (unsigned int)c);
    assert(ULONG_MAX + c + 1 ==  (unsigned long)c);
    assert(-6 == (float)c);
    assert(-6 == (double)c);
    assert(USHRT_MAX + c + 1 == us);
    assert(UINT_MAX + c + 1 == ui);
    assert(ULONG_MAX + c + 1 == ul);
    assert(-6 == ca[1]);
    assert(-6 == sa[1]);
    assert(-6 == ia[1]);
    assert(-6 == la[1]);
    assert(-6 == fa[1]);
    assert(-6 == da[1]);
    
}

void T2()
{
    float f = 3.5;
    assert(f == 3.5);
}

void T3()
{
    int *p, a[2], i = 1;
    p = a;
    assert(&a[0] == p);
    assert(a == p);
    assert(p+1 == &a[1]);
    assert(1+p == &a[1]);
    assert(p+i == &a[1]);
    assert(i+p == &a[1]);
    p++;
    assert(p-1 == a);
}	

void T3a()
{
    char c = 1;
    short s = 1;
    int i = 1;
    long l = 1;
    float f = 1;
    double d = 1;

    assert(c++ == 1 && --c == 1);
    assert(s++ == 1 && --s == 1);
    assert(i++ == 1 && --i == 1);
    assert(l++ == 1 && --l == 1);
    assert(f++ == 1 && --f == 1);
    assert(d++ == 1 && --d == 1);
}
       
    



int ary[10];

void T4()
{
    typedef struct n1 { int a;} n1;
    struct n2 { int a;};
    typedef double n2;
    struct ary {int a;};

    assert(sizeof(ary) == 10 * sizeof(int));
    assert(sizeof(n2)  == sizeof(double));
    assert(sizeof(struct n1) == sizeof (n1));
    assert(sizeof(struct n2) == sizeof(int));
    assert(sizeof(struct ary) == sizeof(int));
}

char * constCharPtr() { char * const p = "and"; return p;}
int constInt() { const a = 5; return a;}

void T5()
{
    static const double x = 0.5;
    const a = '\377';
    char * p = "and";
    const char *p1 = "and const";

    assert(x == 0.5);
    assert(a == -1);

    assert(strcmp(p,"and") == 0);
    assert(strcmp(p1,"and const") == 0);
    assert(constInt() == 5);
    assert(strcmp(constCharPtr(),"and") == 0);


}

#if INT_MAX > SHRT_MAX  /* assume 32 bit integers */

#define SHRTL   32767L      /* 2^15 - 1 */
#define SHRTHEX 0x7fff      /* 2^15 - 1 */
#define INTDEC  4294967295  /* 2^32 - 1 */
#define INTHEX  0xFFFFFFFF  /* 2^32 - 1 */
#define INTU     32767U    /* 2^15 - 1 */
#define INTUL    32767UL   /* 2^15 - 1 */
#define INTULHEX 0x7fffUL  /* 2^15 - 1 */

#else  /* assume 16 bit integers  */

#error You have a problem
#define INTDEC  65535       /* 2^16 - 1 */
#define INTHEX  0xFFFF      /* 2^16 - 1 */

#endif

void T6()  
{
    /* Integer Constants Developed from K&R2 A2.5.1*/

    assert(sizeof SHRTHEX == sizeof(int));
    assert(sizeof INT_MAX == sizeof(int));
    assert(sizeof SHRTL  == sizeof(long));
    assert(sizeof INTDEC == sizeof(long));
    assert(sizeof INTHEX == sizeof(unsigned int));
    assert(sizeof INTU  == sizeof(unsigned int));
    assert(sizeof INTUL  == sizeof(unsigned long));
    assert(sizeof INTULHEX == sizeof(unsigned long));
}

void T7(void)
{
    unsigned char *a, *b, *c;
    unsigned char A, B, C;
    
    a = &A; b = &B; c = &C;

    A = 126; B = 129; C = 198;
    assert(abs(*a - *b) < *c);
}
    
int T8a = 5, * const T8p = &T8a;

void T8()
{
    int n = 333;
    int a = 5, *const p = &a;

    assert(*T8p == T8a);
    *T8p = n;
    assert(*T8p == n && T8a == n);

    assert(*p == a);
    *p = n;
    assert(*p == n && a == n);
}

char *PP = NULL;

void T9()
{
    char *p = NULL;
    assert(p == NULL);
    assert(PP == NULL);
}	


int main()
{
    T1();
    T2();
    T3();
    T3a();
    T4();
    T5();
    T6();
    T7();
    T8();
    T9();
    return 0;
}

#ifdef EiCTeStS
main();
#endif








