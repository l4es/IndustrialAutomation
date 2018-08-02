/*
 * In EiC, string literals are shared.
 */
#include <assert.h>
#include <string.h>

char *a[] = {
    "hello",
    "world",};

char *b[] = {
    "hello",
    "world",};

char *c[] = {
    "hello",
    "world",};

char *d[] = {
    "hello",
    "world",};

char *e[] = {
    "hello",
    "world",};



void T1()
{
    assert(a != b);
    assert(a != c);
    assert(a != d);
    assert(a != e);

    assert(a[0] == b[0]);
    assert(a[0] == c[0]);
    assert(a[0] == d[0]);
    assert(a[0] == e[0]);

    assert(a[1] == b[1]);
    assert(a[1] == c[1]);
    assert(a[1] == d[1]);
    assert(a[1] == e[1]);
}


void T2()
{
    assert(strcmp("hello",a[0]) == 0);
    assert(strcmp("hello",b[0]) == 0);
    assert(strcmp("hello",c[0]) == 0);
    assert(strcmp("hello",d[0]) == 0);
    assert(strcmp("hello",e[0]) == 0);


    assert(strcmp("world",a[1]) == 0);
    assert(strcmp("world",b[1]) == 0);
    assert(strcmp("world",c[1]) == 0);
    assert(strcmp("world",d[1]) == 0);
    assert(strcmp("world",e[1]) == 0);
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









