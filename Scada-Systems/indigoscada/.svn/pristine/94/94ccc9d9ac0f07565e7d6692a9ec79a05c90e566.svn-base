

char str[20] /* missing semi-colon */
float x;
struct p {int x, int y;} point;  /* error in body of structure */

#if defined LNX  
#undef LNX
#endif


int aarr[2][3] = { 1,2,3,4,5,6,7 };     /* error  Too many initialisers */
int carr[2][3] = { {1,2,,}, {3,4,5},};  /* error  illegal initialiser  */

int a;
struct p points[5];

int  hvd(int a,struct {int v, y;} d , float x)  
{
    return d.v + d.y * x + a;
}

void hvd(void)  /* error: Ambiguous re-declaration of hvd */
{
    int i;
}

int  silly(int a,struct {int v, int y;} d , float x)
 /* error in paramater list */
{
    struct node;
    struct head { struct node *next;};
    struct node { struct head *hd;};
    return d.v + d.y * x + a;    /* error: Illegal structure operation */
}

float a;  /* error: Re-declaration of identifier a */

int b;

int oldstyle(a,b,c)  /* error: Illegal: old style C function */
		     /* definition */
 int a,b,c;
{

    return a+b+c;
}

int barr[2][3] = { {1,2}, {3,4,5,6},};  /* error  Too many initialisers */


#ifdef 33  /* error: Illegal macro identifier */
#endif
#include "stdio.h      
#include <stdio.h      

typdef int myint;   /* unknown typdef */

myint amy;          /* Undeclared identifier myint */

amy = 5;            /* Undeclared identifier amy */

#define TESTY 33
#define TESTY 33
#define TESTY 34  /* error: Re-declaration of macro TESTY */
#else             /* error: Unmatched #else */

#endif            /* error: Unmatched #endif */

;;
case 5:           /*error: Illegal label */
switch(a) {
}

"   /* error:  unbalanced */
/* N.B strings literals do not contain newlines  */

"    /* error:  unbalanced */ 
float f1, f2;   /* EiC will not recover from the previous error fast enough to
                 * record these variables.
                 */

int int a;    /* declaration error: invalid specification */

int i1,i2,f2;
int i1,i2,f2;  /* redeclarations are legal if the types remain */
	       /* compatible */

;;    /* force EiC to recover */

int f3args(int x, int y, int z);

int f3args(int x, int y)
{                        /* Ambiguous re-declaration of `f3args' */
    return x +y;
}

void *p;
&p[1];     /* error: Array size unknown */
p[1];      /* error: Illegal lvalue */

while();  /* error: void expression */
for(;;);  /* void expression in for loop OKay */
if();     /* error: void expression */

union {        /* can only initialise first member of a union */
    char c;
    float f;
} uerr = {12,5.5};    /* error: Expected  }*/

void T()
{
    int a; int *p[] = {&a};  /* error: Illegal initialization: illegal */
			     /* address operation */
}

void Tconstraint()
{   int j;
    static int i = 1 ? 1 : (++j);  /* error: Illegal initialization: illegal */
                                   /* address operation */
}


int foo()[];              /*  error: Illegal return type for foo */
int (* foo1[10])()[];      /*  error:Illegal return type for foo1 */

 /* If a function has a return type of void, it is illegal to
  * supply a return type of any kind other than void
  */
void foo2() { return 4;}  /* error:  Illegal return type, expected void */ 

void foo3() { ; }       /* warning:Empty function definition */ 

int k = foo3();  /* error: Illegal cast operation */

const kk = 5;
5++;            /* error: Illegal unary operation */
++5;            /* error: Illegal unary operation */
++kk;           /* error: Illegal unary operation */
kk++;           /* error: Illegal unary operation */
kk = 20;        /* error: Illegal assignment operation */


void Tconst()
{
    /*
     * code developed/pilfered from H&S pg 82.
     */
    const int *pc;         /* pointer to a constant integer */
    const int ic = 25;
    int *p, i;
    
    pc = p = &i;
    *pc = 6;          /* error: Illegal assignment */
    pc = &ic;
    *pc = 5;          /* error: Illegal assignment */


    pc = &i;
    pc = p;

    p = &ic;          /* error: Assignment loses a const qualifier */
    p = pc;           /* error: Assignment loses a const qualifier */
}

int * test2(const int *p)
{
    return p;          /* error: Cast loses const qualifier */
}
int * test3(const int *p)
{
    int * q;
    q = p;           /* error: Assignment loses a const qualifier */
    return q;
}
int test4(const *p)
{
    *p = 5;         /* error: Illegal assignment operation */
    return *p;
} 

typedef struct { int a, b;} ab_t;

void Tconst4()
{
    const ab_t a = { 15,16};
    a.a = 15;           /* error: Illegal assignment operation */
}


void Tconst5()
{
    ab_t a1, b1;
    const ab_t  a;
    ab_t  b;

    a = a1;    /* Illegal assignment operation   */
    a.a = 5;   /* Illegal assignment operation   */
    b = b1;
    b.b = 10;
}

void Tconst6()
{
    struct { const a; int b; } a, b;
    a = b;                /* error: Illegal assignment operation */
    a.b = 1;        
    a.a = 5;              /* error: Illegal assignment operation */ 
}    


int SS(void) { return 5;}

SS(5);     /*error: Illegal parameter no. [1]*/

void  f3(void)
{
    int x;
    x = 3;
    int y;  /* error: Unexpected declaration */
}

int OneParm(int x) { return x;}
 
OneParm();  /* error: Wrong number of arguments for OneParm*/

void xxx()
{
    struct {int a;} a, b = {69};
    b.a = (a=b).a;
    (a=b).a = 5;  /* Illegal assignment operation */
}

void xx2()
{
    int i = 2; struct {int a;} a = {15}, b = {69};
    (i,b).a = 6;  /*Error: Illegal assignment operation */
    (i == 2 ? a:b).a = 5;  /* Error: Illegal assignment operation */
    (a=b).a = 5;        /* Error: Illegal assignment operation */
}

float confict_of_types(float **x)
{
    float yyy[2][2];
    confict_of_types(yyy); /* Suspicious pointer conversion */
}








