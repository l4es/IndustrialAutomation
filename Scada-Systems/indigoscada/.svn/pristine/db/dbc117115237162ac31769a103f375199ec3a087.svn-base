/* testpreproc.c
 *
 *	(C) Copyright Jan 14 1996, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <string.h>
#include <assert.h>


#define MAX 100

/*-----------------------------
 * Preprocessor command lines can be continued by
 * preceding the end-of-line marker by a backslash character, \.
 */
#define sw(a)	switch(a) {  \
			case 1: A=1;break;\
			case 2: A=2;break;\
			default: A=0;\
			 }
void T1(void)
{
    int A;
    sw(2);
    assert(A==2);
}


/*/ funny comment   /*/ 
/*
 */
/*
 *
 */
                /**/
                /***/
/*********************/
/*********************/
#define ten /*
	     * one greater than nine
	     */ (2*5)

void T2(void)
{
    assert(ten==10);
}

/*-----------------------
 * EiC also parses C++ style comments
 */
#ifdef EiCTeStS
// this is a C++ style line comment
// however, line splicing must still work at end of a line comment\
#define  Failed
#ifdef Failed
#error Preprocessor Failure with  C++ style comment
#endif
#endif

/*------------------------
 *  Once a given identifier has been replaced in a
 *  given expansion, it is not replaced if it turns
 *  up again during rescanning; instead it is left
 *  unchanged
 */
#define char  unsigned char

void T3(void)
{
    char b = 200;
    assert(b==200);
    /* test undef */
#undef char
    {
	char b = 200;
	assert(b == -56);
    }
}

/*------------------------
 * The # directive can be preceded by white space
 */
       #define _2ndmacro   ten
void T4(void)
{
    assert(_2ndmacro == 10);
}

/*------------------------
 * A line with just # is considered to be a
 * NULL directive
 */
#

/*------------------------
 * If a line ends in a backslash, then the following
 * line will never be treated as a preprocessor directive
 */
#define BACKSLASH \
#Doug Funny


/*------------------------
 *  Macros with parameters.
 */
#define max(a,b)  ((a)>(b)?(a):(b))

/*------------------------
 *  parameter lists can be empty
 */
#define N() 5

/*------------------------
 * When a macro with parameters is invoked there can be
 * whitespace between its name and the left parenthesis.
 * A macro call can also extend across  multiple lines
 * without the use of the backslash character.
 */
#define BIG    max    ( 0       ,  100 )

void T5(void)
{
     assert(BIG == 100);
     assert(7==max(5,
		   max(6,
		       7)
		   ));
     assert(N() == 5);
}

/*------------------------
 *  The # token appearing in front of a macro parameter,
 * forces both the # and the parameter to be replaced by a quoted
 * argument. A \ character is inserted before each double quote
 * or backslash appearing in the parameter. Also, all white
 * space within the replacement parameter is replaced with a single
 * space character.
 */
#define S(x)   #x

void T6(void)
{
    assert(strcmp("hello world",S(hello world))==0);
    assert(strcmp("\"hello world\\n\"",S("hello world\n"))==0);
    assert(strcmp("Its a nice day", S(Its   a   nice
				      day)) == 0);
}

/*------------------------
 * Two tokens can be merged by the presence of 
 * the preprocessor token merging operator ##.
 * After replacement of the  parameters each ## is
 * deleted, together with any white space on either side, so
 * as to concatenate adjacent tokens. The ## operator
 * may not appear at the beginning or end of a parameter list.
 */

#define TEMP(i)   12   ##  i
#define TEMP2(i)  i   ## 23
#define cat(x,y)   x ## y
#define xcat(x,y)  cat(x,y)
#define S(x)   #x
#define S1(x)   S(x)
#define M1(x)  abc ## x ## abc ## x ## abc ## x ## abc 


void T7(void)
{
    assert(TEMP(3) == 123);
    assert(TEMP2(1) == 123);
    assert(xcat(xcat(1,2),3) == 123);
    assert(strcmp("abc123abc123abc123abc",
		  S1(M1(123))) == 0);
}
    
/*------------------------
 * An argument sequence can contain parentheses
 */

#define prod(x,y)    ((x)+(y))
#define mult(x,y)    (prod(x,y)*x)

void T8(void) { assert(mult(2,1) == 6); }

/*------------------------
 * define a macro that takes an arbitrary statement
 * as an argument
 */
#define insert(stmt) stmt
#define seq10  insert({int i;for(i=0;i<10;i++); A = i;})

void T9(void)
{
    int A;
    seq10;
    assert(A==ten);
}


/*------------------------
 *  Any macro definition of a function can be suppressed locally by
 *  enclosing the name of the function in
 *  parenthesis, because the name is then not followed by a left
 *  parenthesis.
 */
   
int f(int x) { return x*x;}
#define f(x) ((x)+5)

void T10() { assert(f(5) == 10 && (f)(5) == 25); }

/*-------------------
 * The preprocessor should detect the formation of
 * new macros.
 */
#define C()  "HELLO"
#define H() C
#define X() H

void T11(void)
{
    
    assert(strcmp("HELLO",X() ()     ()    ) == 0);
    assert(strcmp("HELLO",X() ( /* they can extend across multiple lines */ 
				)
		  (
		   )
		  ) == 0);
}    

 
int main(void)
{
    T1();
    T2();
    T3();
    T4();
    T5();
    T6();
    T7();
    T8();
    T9();
    T10();
    T11();
    return 0;
}

#ifdef EiCTeStS
main();
#endif



