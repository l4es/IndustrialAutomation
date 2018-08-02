/*
 * The code in this modular was adopted from
 *     Harbison and Steele (1995)
 *     C a reference manual  (4th edition)
 *     Tartan, Inc.
 *     Prentice Hall , Englewood Cliffs, New Jersey
 *     ISBN 0-13-326224-3
 */
#include <assert.h>

typedef int *IP;          /* IP is  a pointer to int */
typedef int (*FP)();      /* FP is a pointer to function returning int */
typedef int F(int a);     /* F is a function with one int parameter, */
                          /* and it returns an int */

typedef double A5[5];     /* A5 is a 5-element array of double */
typedef int A[];          /* A is an array of int of undefined size*/

typedef long int bigint;  /* bigint is a long */   

IP ip;        /* ip is a pointer to an int */
IP fip();     /* fip is a function that returns a pointer to int */
FP fp;        /* fp is pointer to a function returning an  int */
F * fp2;      /* fp2 is a pointer to a function taking an
	       * int parameter and returning an int
	       */

A5 a5;       /* a5 is a 5-element double array */
A5 a25[2];   /* a25 a 2-element array of 5-element arrays of  double */
A a;         /* is an array of int of unspecified size */
A *ap3[3];   /* ap3 is a 3-elemnt array of pointers to arrays
	      * of int (with unspecified size) */

typedef struct S{int a, b;} s1type, s2type;


typedef int T;
T foo = 5;

void T1()
{
    float T;
    T = 0.5;
    assert( T == 0.5 && foo == 5);
}


T foo2 = 6;

void T2()
{
  typedef struct { char a; } c;
  typedef struct { short c; } scs;

  scs a;
  c b;

  assert(sizeof a >= sizeof b);
}

int main(void)
{
    assert(sizeof(s1type) == sizeof(s2type));
    assert(sizeof(struct S) == sizeof(s2type));
    assert(sizeof(ap3) == 3 * sizeof(void *));
    assert(sizeof(a5) == 5 * sizeof(double));
    assert(sizeof(a25) == 2 * sizeof(a25[0]));
    T1();
    assert(foo == 5 && foo2 == 6);

    T2();
    return 0;
}


#ifdef EiCTeStS
main();
#endif










