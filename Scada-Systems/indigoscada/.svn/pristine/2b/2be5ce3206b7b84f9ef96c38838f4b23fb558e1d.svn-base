#include <stdio.h>
#include <assert.h>

int foo() { return 33; }

void T2()
{
    typedef struct { int (*f)(); } Test;
    int r;
    Test *x, y;
    x = &y; x->f = foo;
   
    r = foo();
    assert(x->f() == r);
    assert(x->f() == (*x->f)());
    assert(x->f() == y.f());
    assert(x->f()*2 == 2*x->f());
   
}

char *echo(char *str)
{
    return str;
}
char * m(char *argv[], char *f(char *))
{
    return f(*++argv);
}

int ss(int ss)
{
    return ss;
}

void T1(void)
{
    char *names[] = { "ED", "CLAIR"};
    assert(m(names,echo) == names[1]);

    assert(ss(3) == 3);
    assert(ss(1) == 1);

}

void SubR2(int n, int *m1, int *m2,int *m3, int *m4)
{
    *m1 = n * 5;
    *m2 = n * 6;
    *m3 = n * 7;
    *m4 = n * 8;
}

void R2(int n)
{
    int five,six,seven,eight;


    SubR2(n,&five,&six,&seven,&eight);

    if(n > 0)
	R2(n-1);
    
    assert(five  == n * 5 && six == n * 6); 
    assert(seven == n * 7 && eight == n * 8);
}

void sparm(struct s { int a, b;} ab)
{
    printf(" a = %d, b = %d\n", ab.a, ab.b);
}

void empty() { return ;} /* okay, an empty statement */
			 /*  evaluates to void */

int sfunc()  /* implicit void parameter */
{
    static int i=10;
    return i++;
}
    
int sqr(int x) { return x * x; }
int  goo(int n, int (*f)(int x), int x)
{
    return n * (*f)(n) + x;
}

int  g22(int n, int f(int x), int x)
{
    return n * f(n) + x;
}

struct s {
    int s;
    int (*f)(int y);
    int y;
} stype;

int test(int (*f)(void)) { return (*f)(); }

typedef struct {
    int a,b;
}ab_t;

ab_t * fff(void)
{
    static ab_t ab = {222,333};
    return &ab;
}

ab_t ggg(void) {
    static ab_t ab = {444,555};
    return ab;
}

int fg()
{
    int gsum(int x, int y);
    int k = gsum(5,6);
    return k;
}


int gsum(int x, int u)
{
    return x + u;
}

int Return3(void)
{
    int gsum(int x, int u);
    return gsum(1,2);
}

int ff(void) { int g2(void); return g2();} 
int g3(void) {return 3;}
int g2(void) {return 2;}
int h(int (*x)(void)) { return (*x)();}
int g(int (*f)(void)) { return f();}
int f(int g2(void)) { return g2();}

char * fooStr() { return "abcdefg";}
int fooInt1(const int a) { return a;}
int fooInt2(int * const p) { return *p;}


/* define functions returning pointer to functions */
typedef int function();
function * rtn_g3() { return g3;}

void indirect()
{
    int indirectA();
    assert(indirectA(5,6) == 11);
}
int indirectA(int a, int b) { return a + b;} 

int Proto1(int , int, char *);

int Proto1(int x, int y, char *str)
{
    int i;
    for(i=0;*str++;++i);
    return i + x + y;
    
}

int main()
{
    int (*fp)();
    int i;
    stype.f = sqr;
    assert(test(g2) == 2 && test(g3) == 3);
    assert(goo(2,stype.f,3) == 11);
    assert(fff()->a == 222 && fff()->b == 333);
    assert(ggg().a == 444 && ggg().b == 555);
    assert(ff == &ff);
    assert((*rtn_g3())() == 3); 

    fp = &g3;
    assert(g3() == fp());
    
    for(i=0;i<5;i++) 
      assert(i+10 == sfunc());

    assert(ff() == g2());
    assert(h(g2) == g(g2));
    assert(f(g3) == g3());
    assert(fg() == 11);
    assert(fooStr()[3] == 'd' && fooStr()[0] == 'a');
    assert(fooInt1(5) == 5);

    i = 15;
    assert(fooInt2(&i) == i);
    
    R2(20);

    assert(Return3() == 3);

    indirect();

    assert(Proto1(2,3,"abc") == 8);


    T1();
    T2();
    return 0;
}


#ifdef EiCTeStS
main();
#endif


















