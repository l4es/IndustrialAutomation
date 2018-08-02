#if defined LNX                
#undef LNX
#endif

struct p {int x, int y;} point;  /* error in body of structure */
struct p points[5];

char str[20]                    /* error: expected ;           */
float x;

typdef int myint;               /* 'typdef' unknown            */
myint amy;                      /* undeclared identifier myint */
amy = 5;                        /* undeclared identifier amy   */

int a;
int a;                          /* compatible redeclarations are legal*/

int .*b;                        /* error: declaration error           */
int *0b;                        /* error: direct declarator error     */
int  b;
static register int *b;         /* error: multiple storage class used */
register int *b;                /* error: illegal storage class usage */
                                /* + ambiguous redeclaration of b     */
short double int b;             /* error: declaration type specifn err*/
int c = ;                       /* illegal cast operation             */
int d[10] = 10;                 /* error: missing {                   */
char e[4] = "abcde";            /* warn:  char array too long         */
char f[5] = 10;                 /* error: illegal initialisation      */
int g[2] = "abcde";             /* error: illegal initialisation      */
int h[2][2] = { 1,2,3,4,5 };    /* error: too many initialisers       */
int i[3] = { 1,2,,};            /* error: illegal initialisation      */
                                /*        (missing initialiser)       */
int j = f;                      /* ?????????? */

enum k;                         /* error: enumeration spec'n error    */
enum l { abc };                 /* problems galore */

void funcA(char .*s) {1;}       /* error: param declaration error     */
void funcB(char *s, ..){1;}     /* error: expected ...                */

int  funcC(int a,struct {int v, int y;} d , float x)
                                /* error in paramater list            */
{
    struct node;
    struct head { struct node *next;};
    struct node { struct head *hd;};
    return d.v + d.y * x + a;
}


int oldstyle(a,b,c)             /* error: old style C function def'n  */
  int a,b,c;
{
      return a+b+c;
}

int funcD(static int a,struct {int v, y;} d , float x)
                                /* error:  static param not allowed   */ 
{               
    return d.v + d.y * x + a;				   
}
    
void funcD(void)                /* error: ambiguous re-decl'n of func */
{
    void badfunc(void) { c; }   /* error: illegal fn defn             */
    void badfunc(int c) { }     /* warn:  empty function              */
				/* should give error - illegal func def'n  */
    /*printf("hello stupid\n");*/
}





#ifdef 33  /* error: Illegal macro identifier */
#endif
#include "stdio.h
#include <stdio.h


#define TESTY 33
#define TESTY 33
#define TESTY 34  /* error: Re-declaration of macro TESTY */
#else             /* error: Unmatched #else */

#endif            /* error: Unmatched #endif */


case 5:
switch(a) {
}

"   
/* N.B strings literals do not contain newlines  */

"  
float f1, f2;














