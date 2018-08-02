#include <assert.h>
#include <limits.h>

void T1()
{   /* signed tests */
    char  c = CHAR_MAX;
    short s = SHRT_MAX;
    int  i = INT_MAX;
    long l = LONG_MAX;

    assert(c == CHAR_MAX);
    assert(s == SHRT_MAX);
    assert(i == INT_MAX);
    assert(l == LONG_MAX);

    c = c + 1;
    s = s + 1;
    i = i + 1;
    l = l + 1;
    assert(c == CHAR_MIN);
    assert(s == SHRT_MIN);
    assert(i == INT_MIN);
    assert(l == LONG_MIN);

    c = c - 1;
    s = s - 1;
    i = i - 1;
    l = l - 1;
    assert(c == CHAR_MAX);
    assert(s == SHRT_MAX);
    assert(i == INT_MAX);
    assert(l == LONG_MAX);

    c++;s++;i++;l++;
    assert(c == CHAR_MIN);
    assert(s == SHRT_MIN);
    assert(i == INT_MIN);
    assert(l == LONG_MIN);

    c++;s++;i++;l++;
    assert(c == CHAR_MIN + 1);
    assert(s == SHRT_MIN + 1);
    assert(i == INT_MIN  + 1);
    assert(l == LONG_MIN + 1);
        
    c--;s--;i--;l--;
    assert(c == CHAR_MIN);
    assert(s == SHRT_MIN);
    assert(i == INT_MIN);
    assert(l == LONG_MIN);

    c--;s--;i--;l--;
    assert(c == CHAR_MAX);
    assert(s == SHRT_MAX);
    assert(i == INT_MAX);
    assert(l == LONG_MAX);
}

void T2()
{   /* unsigned tests */
    unsigned char  c = UCHAR_MAX;
    unsigned short s = USHRT_MAX;
    unsigned int  i = UINT_MAX;
    unsigned long l = ULONG_MAX;

    assert(c == UCHAR_MAX);
    assert(s == USHRT_MAX);
    assert(i == UINT_MAX);
    assert(l == ULONG_MAX);

    c = c + 1;
    s = s + 1;
    i = i + 1;
    l = l + 1;
    assert(c == 0);
    assert(s == 0);
    assert(i == 0);
    assert(l == 0);

    c = c - 1;
    s = s - 1;
    i = i - 1;
    l = l - 1;
    assert(c == UCHAR_MAX);
    assert(s == USHRT_MAX);
    assert(i == UINT_MAX);
    assert(l == ULONG_MAX);

    c++;s++;i++;l++;
    assert(c == 0);
    assert(s == 0);
    assert(i == 0);
    assert(l == 0);

    c++;s++;i++;l++;
    assert(c == 1);
    assert(s == 1);
    assert(i == 1);
    assert(l == 1);

    c--;s--;i--;l--;
    assert(c == 0);
    assert(s == 0);
    assert(i == 0);
    assert(l == 0);

    c--;s--;i--;l--;
    assert(c == UCHAR_MAX);
    assert(s == USHRT_MAX);
    assert(i == UINT_MAX);
    assert(l == ULONG_MAX);
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

