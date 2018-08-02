#include <string.h>
#include <assert.h>

/* test simple array initialisation */
void T1()
{
    static int c = 0;
    int a[] = {1,2};
    int b[] = {1,2,3};
    assert(sizeof(a) == sizeof(int)*2);
    assert(sizeof(b) == sizeof(int)*3);
    assert(a[0] == 1 && a[1] == 2);
    assert(b[0] == 1 && b[1] == 2 && b[2] == 3);
    a[0] += a[0]; a[1] += a[1];
    b[0] += b[0]; b[1] += b[1]; b[2] += b[2];
    if(c == 0) {
	c++;
	T1();
	c = 0;
    }
    assert(a[0] == 2 && a[1] == 4);
    assert(b[0] == 2 && b[1] == 4 && b[2] == 6);
}

/* test array of strings initialisation */
void T2()
{
    static int c = 0;
    char *names[] = { "Ed", "Claire", "Sarah", "Alexandria",
			  "Georgia"};
    int n;
    n = sizeof(names)/sizeof(char*);
    assert(n == 5);
    assert(strcmp("Ed",names[0]) == 0);
    assert(strcmp("Sarah",names[2]) == 0);
    assert(strcmp("Georgia",names[n-1]) == 0);
    names[2] = "hello World";
    if(c == 0) {
	c++;
	T2();
	c = 0;
    }
    assert(strcmp(names[2],"hello World") == 0);
}


int f() {return 1;}
int i1 = 1,i2 = 2,i3 = 3;

/* test structure initialisation */
void T3()
{
    extern int i1,i2,i3;

    struct S0 {int a; char b[5]; double c; };
    struct S0 xs0 = {1, "abcd", 45.0};

    struct S1 { int a;
		struct S2 {double b;
			   char c;} b;
		char c[4];
	    };

    struct S1 xs1 = {22, {5.5}, "ab" };


    typedef struct {
	int *v;
	int (*p)();
    }arg_t;

    arg_t arg[] = {&i1,f,&i2,f,&i3,f};

    assert(3 == sizeof(arg)/sizeof(arg_t));
    assert(*arg[0].v == i1 && *arg[1].v == i2 && *arg[2].v == i3);
    assert(arg[0].p == f && arg[1].p == f && arg[2].p == f);
    assert(xs0.a == 1 && xs0.b[2] == 'c' && xs0.c == 45.0);
    assert(xs1.a == 22 && xs1.b.b == 5.5 && xs1.c[2] == 0);
    assert(xs1.b.c == 0 && xs1.c[1] == 'b');
}

/* test union initialisation */
void T4()
{
    static int c = 0;
    union {
	char a;
	int b;
	float c;
    }un[3] = { 'a', 'b', 'c'};

    assert(un[0].a == 'a' && un[1].a == 'b' && un[2].a == 'c');

    un[1].b = 1000;
    un[2].c = 12.5;
    if(c == 0) {
	c++;
	T4();

    }
    assert(un[1].b == 1000 && un[2].c == 12.5);
}

/* test scope affects on aggregate initialisation */
void T5()
{
    int a = 5;
    while(--a) {
	int a[2] = {22,33};
	assert(a[0] == 22 && a[1] == 33);
	a[0] = 55; a[1] = 100;
	assert(a[0] == 55 && a[1] == 100);
    }
    assert(a == 0);
}


/* test array of pointers initialisation */
int x[][5] = { {1, 2, 3, 4, 0}, { 5, 6 }, { 7 } };
void T6()
{
    int *y[] = { x[0], x[1], x[2], 0 };
    assert(y[0][0] == x[0][0]);
    assert(y[1][0] == x[1][0]);
    assert(y[2][0] == x[2][0]);
}


void T7()
{
    typedef struct {int x, y;}  xy;
    xy p[] = {{1,2,},};

    assert(p[0].x == 1 && p[0].y == 2);
}
    

int main()
{
    T1();
    T2();
    T3();
    T4();
    T5();
    T6();
    T7();
    return 0;
}


#ifdef EiCTeStS
main();
#endif
