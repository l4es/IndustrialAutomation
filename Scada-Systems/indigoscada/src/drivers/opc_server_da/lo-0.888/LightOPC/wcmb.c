/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 string manipulations
 **************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "util.h"

int lo_mbstowcs(loWchar *wcs, const char *mbs, int nn)
{
 int ii;
// if (-1 == nn) nn = strlen(mbs);
#ifdef _WIN32
# if 0/* This is not quite right call but in this file 'len' is already counted */
 ii = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbs, nn, wcs, nn);
# else
 ii = mbstowcs(wcs, mbs, nn);
# endif
 if (wcs && ii <= 0)
#endif
   for(ii = 0; ii < nn; ii++)
     {
#if 0
      wcs[ii] = (unsigned char)mbs[ii];
#else
      ((char*)wcs)[ii << 1] = mbs[ii];
      ((char*)wcs)[ii << 1 | 1] = '\0';
#endif
      if (!mbs[ii]) break;
     }
/* if (wcs && ii < len) wcs[ii] = 0;*/
 return ii;
}

int lo_wcstombs(char *mbs, unsigned mb_len, const loWchar *wcs, unsigned wc_len)
{
 unsigned ii, wc;

 if (!mbs)
   {
    for(wc = ii = 0; ii < wc_len && wcs[ii]; ii++)
      {
#ifdef _WIN32  /* due to error in wcstombs(NULL... */
       char tb[16/*MB_CUR_MAX*/];
       int wcl;
       if (1 < (wcl = wctomb(tb, wcs[ii]))) wc += wcl - 1;
#else
/*     if (wcs[ii] & ~0x7f) wc++;*/
#endif
      }
    ii += wc;
   }
 else
   for(wc = ii = 0; wc < wc_len && ii < mb_len; wc++)
     {
#ifdef _WIN32  /* due to error in wcstombs(NULL... */
      char tb[16/*MB_CUR_MAX*/];
      int wcl;
      if (0 < (wcl = wctomb(tb, wcs[ii])))
	{
	 if ((unsigned)wcl > mb_len - ii) wcl = mb_len - ii;
	   memcpy(mbs + ii, tb, wcl);
	 ii += wcl;
	}
      else
#else
        mbs[ii++] = (char)wcs[wc];
#endif
      if (!wcs[wc]) break;
     }
 return (int)ii;
}

/* end of wcmb.c */
