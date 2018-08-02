/*
 * this file is designed to test EiC's show facility
 */

#define test  hasty tasty
#define sum(x,y) ((x) + (y))
#define empty()
#define empty1()  nothing
#define cat(x,y) x ## y
#define mcat(x,y)  F ## x ## y ## E
#define item(x)    # x
#define order(x,y,z)   z + x + y
#define xy(x,y)  y + y + x + y + x
 
typedef int *IP;          /* IP is  a pointer to int */
typedef int (*FP)();      /* FP is a pointer to function returning int */
typedef int F(int a);     /* F is function with one int parameter, */
                          /* and it returns an int */

typedef double A5[5];     /* A5 is 5-element array of double */
typedef int A[];          /* A is array of int of undefined size*/

void regline(float *x,  /* x values */
	     float *y,  /* y values; i.e., y = f(x) */
	     int n,     
             float *m,  /* slope */
	     float *b   /* offset */
	     )
{

    /* In a least square sense, find the equation:
     *  y = mx + b; Returns the slope in m and
     *  the offset in b. 
     */
    int c;
    /* some more comments */
}


void f( void x(int y, int z), int i1, int i2);
void (*f2(int f, int x)) (char * s);

#ifdef _EiC
:show test
:show sum
:show empty
:show empty1
:show cat
:show mcat
:show item
:show order
:show xy

:show IP
:show FP
:show F
:show  A5
:show A

/* functions */
:show f
:show regline
:show f2

#endif

















