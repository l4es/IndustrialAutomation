//#!/home/edb/bin/eic -f

/* the purpose of this test is to test
 * the following declarations conflicts,
 * when f1 includes f2 and which declares
 * the same variable with different storage
 * classes:
 *                    f2
 *            int   static   extern
 *      int    N     Y         Y
 * f1 static   N     Y         N
 *    extern   Y     Y         Y
 *
 *
 *   N = error
 *   Y = allowed.
 *
 */

#include <assert.h>

int a1;   
int a2;   
int a3;
static int a4;
static int a5;
static int a6;
extern int a7;
extern int a8;
extern int a9;


#include testsclass1.aux


int a10;
static int a11;


:files testsclass.c
:files testsclass1.aux






