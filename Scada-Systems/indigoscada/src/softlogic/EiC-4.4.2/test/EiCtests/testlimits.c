#include <stdio.h>
#include <limits.h>
#include <assert.h>


void check_assigns()
{
    double d;
    float f;
    long l;
    unsigned ul;
    int i;
    unsigned int ui;
    short s;
    unsigned short us;
    char c;
    unsigned char uc;

#if 0
    /* on the alpha the double is smaller than
       long
       */
    d = LONG_MIN;
    l = d;
    assert(l==d);
    d = LONG_MAX;
    l = d;
    assert(l==d);
    d = ULONG_MAX;
    ul = d;
    assert(ul == d);
    l = ul;
    assert(l == -1);

#endif
    
    d = INT_MIN;
    i = d;
    assert(i==d);
    d = INT_MAX;
    i = d;
    assert(i==d);
    d = UINT_MAX;
    ui = d;
    assert(ui == d);
    i = ui;
    assert(i == -1);

    l = INT_MIN;
    i = l;
    assert(i==l);
    l = INT_MAX;
    i = l;
    assert(i==l);
    l = UINT_MAX;
    ui = l;
    assert(ui == l);
    i = ui;
    assert(i == -1);



    f = SHRT_MAX;
    s = f;
    assert(f == s);
    f = SHRT_MIN;
    s = f;
    assert(f == s);
    f = USHRT_MAX;
    us = f;
    assert(f == us);
    s = us;
    assert(s == -1);

    i = SHRT_MAX;
    s = i;
    assert(i == s);
    i = SHRT_MIN;
    s = i;
    assert(i == s);
    i = USHRT_MAX;
    us = i;
    assert(i == us);
    s = us;
    assert(s == -1);
    
    i = CHAR_MAX;
    c = i;
    assert(i == c);
    i = CHAR_MIN;
    c = i;
    assert(i == c);
    i = UCHAR_MAX;
    uc = i;
    assert(i == uc);
    c = uc;
    assert(c == -1);
    
}
    
int main(void)
{
    char cmax, cmin;
    short smax,smin;
    int imax,imin;
    long lmax,lmin;
    unsigned char ucmax;
    unsigned short usmax;
    unsigned int  uimax;
    unsigned long ulmax;

    
    cmax = CHAR_MAX;
    cmin = CHAR_MIN;
    smax = SHRT_MAX;
    smin = SHRT_MIN;
    imax = INT_MAX;
    imin = INT_MIN;
    lmax = LONG_MAX;
    lmin = LONG_MIN;

    ucmax = UCHAR_MAX;
    usmax = USHRT_MAX;
    uimax = UINT_MAX;
    ulmax = ULONG_MAX;

    assert( CHAR_MIN == cmin);
    assert( SHRT_MIN == smin);
    assert(  INT_MIN == imin);
    assert( LONG_MIN == lmin);
    assert( CHAR_MAX == cmax);
    assert( SHRT_MAX == smax);
    assert(  INT_MAX == imax);
    assert( LONG_MAX == lmax);
    assert(UCHAR_MAX == ucmax);
    assert(USHRT_MAX == usmax);
    assert( UINT_MAX == uimax);
    assert(ULONG_MAX == ulmax);


    check_assigns();
    return 0;
}


#ifdef EiCTeStS
main();
#endif





