#include <stdio.h>
#include <assert.h>


struct s {
    int s;
    int (*f1)(char *,...);
    int (*f2)(char *,...);
} stype;

int myprint(char *fmt, ...)
{ /* very simple */
    return printf(fmt);
}

void T1(void)
{
    stype.f1 = printf;
    stype.f2 = myprint;

    assert(stype.f1("hey\n") == 4);
    assert(stype.f2("hey\n") == 4);
}

void T2(void)
{
    int (*f1)(char *,...);
    int (*f2)(char *,...);

    f1 = printf;
    f2 = myprint;

    assert(f1("hey\n") == 4);
    assert(f2("hey\n") == 4);
}


void T3(void)
{
    int (*f[2])(char *,...);

    f[0] = printf;
    f[1] = myprint;

    assert(f[0]("hey\n") == 4);
    assert(f[1]("hey\n") == 4);
}

void T4(void)
{
    int (*f)(const char *,...);

    assert(myprint == myprint);
    assert(myprint == &myprint);
    assert(&myprint == myprint);
    assert(printf == printf);
    assert(printf == &printf);
    assert(&printf == printf);

    f = printf;

    assert(f == printf);
    assert(printf == f);

    assert(f == &printf);
    assert(&printf == f);


}

int main()
{
    T1();
    T2();
    T3();
    
    T4();
    return 0;
}


#ifdef EiCTeStS
main();
#endif


















