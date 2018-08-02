/* +++Date last modified: 05-Jul-1997 */

/* ENG.C - Format floating point in engineering notation          */
/* Released to public domain by author, David Harmon, Jan. 1994   */

#include <stdio.h>
#include "snipmath.h"

char *eng(double value, int places)
{
      const char * const prefixes[] = {
            "a", "f", "p", "n", "æ", "m", "", "k", "M", "G", "T"
            };
      int p = 6;
      static char result[30];
      char *res = result;

      if (value < 0.)
      {
            *res++ = '-';
            value = -value;
      }
      while (value != 0 && value < 1. && p > 0)
      {
            value *= 1000.;
            p--;
      }
      while (value != 0 && value > 1000. && p < 10 )
      {
            value /= 1000.;
            p++;
      }
      if (value > 100.)
            places--;
      if (value > 10.)
            places--;
      sprintf(res, "%.*f %s", places-1, value, prefixes[p]);
      return result;
}

#ifdef TEST

#include <stdio.h>

int main()
{
      double w;

      for (w = 1e-19; w < 1e16; w *= 42)
            printf(" %g W = %sW\n", w, eng(w, 3));
      return 0;
}
#endif /* TEST */
