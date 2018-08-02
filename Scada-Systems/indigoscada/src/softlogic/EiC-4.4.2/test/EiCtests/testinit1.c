#include <stdio.h>
#include <assert.h>

struct S0 {int a; char b[5]; double c; };
struct S0 xs0 = {1, "abcd", 45.0};

struct S1 { int a;
	    struct S2 {double b;
		       char c;} b;
	    char c[4];
	};

struct S1 xs1 = {22, {5.5}, "ab" };



int f() {return 1;}
int i1 = 1,i2 = 2,i3 = 3;


typedef struct {
    int *v;
    int (*p)();
}arg_t;

arg_t arg[] = {&i1,f,&i2,f,&i3,f};

int foo(int * x)
{
    const int * p = (int *)x;
    return *p;
}



int  main()
{
    int k = 5;
    assert(3 == sizeof(arg)/sizeof(arg_t));
    assert(*arg[0].v == i1 && *arg[1].v == i2 && *arg[2].v == i3);
    assert(arg[0].p == f && arg[1].p == f && arg[2].p == f);
    assert(xs0.a == 1 && xs0.b[2] == 'c' && xs0.c == 45.0);
    assert(xs1.a == 22 && xs1.b.b == 5.5 && xs1.c[2] == 0);
    assert(xs1.b.c == 0 && xs1.c[1] == 'b');
    assert(foo(&k) == k);
    return 0;
}


#ifdef EiCTeStS
main();
#endif










