#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

void T1()
{
    int i = INT_MAX;
    short s = SHRT_MAX;
    char c = CHAR_MAX;
    long l = LONG_MAX;

    assert(c <= UCHAR_MAX);
    assert(c <= SHRT_MAX);
    assert(c <= USHRT_MAX);
    assert(c <= INT_MAX);
    assert(c <= UINT_MAX);
    assert(c <= LONG_MAX);
    assert(c <= ULONG_MAX);

    assert(s >= CHAR_MAX);
    assert(s >= UCHAR_MAX);
    assert(s <= USHRT_MAX);
    assert(s <= INT_MAX);
    assert(s <= UINT_MAX);
    assert(s <= LONG_MAX);
    assert(s <= ULONG_MAX);

    assert(i >= UCHAR_MAX);
    assert(i >= SHRT_MAX);
    assert(i >= USHRT_MAX);
    assert(i <= INT_MAX);
    assert(i <= UINT_MAX);
    assert(i <= LONG_MAX);
    assert(i <= ULONG_MAX);

    assert(l >= UCHAR_MAX);
    assert(l >= SHRT_MAX);
    assert(l >= USHRT_MAX);
    assert(l >= INT_MAX);
    /* assert(l >= UINT_MAX);  ambiguous */
    assert(l <= LONG_MAX);
    assert(l <= ULONG_MAX);

}

void T2()
{
    long l = 50;
    int i = 50;
    short s = 50;
    char c = 50;

    assert(c < UCHAR_MAX);
    assert(c < USHRT_MAX);
    assert(c < UINT_MAX);
    assert(c < ULONG_MAX);

    assert(s < UCHAR_MAX);
    assert(s < USHRT_MAX);
    assert(s < UINT_MAX);
    assert(s < ULONG_MAX);

    assert(i < UCHAR_MAX);
    assert(i < USHRT_MAX);
    assert(i < UINT_MAX);
    assert(i < ULONG_MAX);
    
    assert(l < UCHAR_MAX);
    assert(l < USHRT_MAX);
    assert(l < UINT_MAX);
    assert(l < ULONG_MAX);
}

int main()
{
    T1();
    T2();
    return 0;
}

#ifdef EiCTeStS
main();
#endif








