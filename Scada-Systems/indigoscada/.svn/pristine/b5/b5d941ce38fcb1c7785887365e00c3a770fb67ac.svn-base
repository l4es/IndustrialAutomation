/* itoa.c
 *
 *	(C) Copyright March 3 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <limits.h>
#include "stdliblocal.h"

static char lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";

char *utoa(unsigned int uval, char *s, int radix)
{
    register int i;

    i=0;
    do {
      s[i++] = lower[uval % radix];
      uval /= radix;
    }while(uval>0);

    s[i] = '\0';
    return strrev(s);
}

char *ultoa(unsigned long uval, char *s, int radix)
{
    register int i;

    i=0;
    do {
      s[i++] = lower[uval % radix];
      uval /= radix;
    }while(uval>0);

    s[i] = '\0';
    return strrev(s);

    
}

char *itoa(int n, char *s, int radix)
{
    unsigned int uval;
    int i, sign;

    if((sign = (n < 0)) && radix == 10)
	uval = -n;
    else
	uval = n;

    i=0;
    do {
      s[i++] = lower[uval % radix];
      uval /= radix;
    }while(uval>0);

    if (sign)
	s[i++] = '-';
    s[i] = '\0';
    return strrev(s);
}

char *ltoa(long n, char *s, int radix)
{
    unsigned long uval;
    int i, sign;

    if((sign = (n < 0)) && radix == 10)
	uval = -n;
    else
	uval = n;

    i=0;
    do {
      s[i++] = lower[uval % radix];
      uval /= radix;
    }while(uval>0);

    if (sign)
	s[i++] = '-';
    s[i] = '\0';
    return strrev(s);
}







