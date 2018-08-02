#include <limits.h>

/* Some simple expressions for testing
 *  out the preprocessor with
 */


#if 5 + 6 > 5 + 7
#error  5 + 6 > 5 + 7
#endif

#if !(5 + 6 < 5 + 7)
#error  !(5 + 6 < 5 + 7)
#endif

#if 5*3+22-5 != 32
#error  5*3+22-5 != 32
#endif

#if 0xFFFFFFF < 33
#error  0xFFFFFFF < 33
#endif

#if SHRT_MAX < CHAR_MAX
#error  SHRT_MAX < CHAR_MAX
#endif

#if INT_MAX < SHRT_MAX
#error  INT_MAX < SHRT_MAX
#endif

#if LONG_MAX < INT_MAX
#error  LONG_MAX < INT_MAX
#endif

#if USHRT_MAX < UCHAR_MAX
#error  USHRT_MAX < UCHAR_MAX
#endif

#if UINT_MAX < USHRT_MAX
#error  UINT_MAX < USHRT_MAX
#endif

#if 0 < CHAR_MIN 
#error  0 < CHAR_MIN 
#endif 

#if 0
#error  0
#endif

#if 1<<2 != 4
#error  1<<2 != 4
#endif

#if 4 != 1<<2
#error  4 != 1<<2
#endif

#if -1 + 2 != 1
#error  -1 + 2 != 1
#endif

#if 5 < 5
#error  5 < 5
#endif

#if +5 != 5
#error +5 != 5
#endif


/*  Character constants */

#if 'a' > 'b'
#error  'a' > 'b
#endif

#if '\23' != 19 
#error '\23' != 19 
#endif

#if '\x42' != 'B'
#error '\x42' != 'B'
#endif

/* Miscellaneous tests */

#if !(helloworldnot) - 1
#error !(helloworldnot) - 1
#endif 


int main(void)
{
    return 0;
}







