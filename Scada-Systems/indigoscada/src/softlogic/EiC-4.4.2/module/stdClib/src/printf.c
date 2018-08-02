/* vprintf.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "stdliblocal.h"

static int charin_(char **buf)
{
    int ch;
    ch = **buf;
    *buf += 1;
    if(!ch)
	ch = EOF;
    return ch;
}

static int charback_(int ch, char **buf)
{
    *buf -= 1;
    return ch;
}

static int charout_(int c, char **buf)
{
    char *s;
    s = *buf;
    *s++ = c;
    *s = 0;
    *buf = s;
    return 1;
}


int _Uprintf(int (*output)(), void *arg, const char *fmt, va_list ap)
{
    /*_UPRINTF 
     * Orginally developed from James Hendrix's Small C tools.
     *
     * (c) Edmond J.Breen (March, 1995).
     *           ALL RIGHTS RESERVED.
     *
     * Purpose: To perform formatted printing to a function.
     *          It performs a superset of printf type
     *          operations.
     *
     * 		  _UPRINTF   RECOGNISES:
     * FLAGS:
     *         -, +, 0 and #.
     *    plus ! and |  (see below).
     *
     * CONVERSION CHARACTERS:
     *         d, i, o, x, X, u, c, s,
     *         f, e, E, g, G, p, n, %
     *    plus b   (see below).
     *
     * LENGTH MODIFIES:
     *          l, L, and h.
     *
     * The WIDTH and or the PRECISION can be set indirectly
     *  via * as specified by K&R.
     *
     *        _UPRINTF EXTRA FEATURES.
     * NEW FLAGS:
     *        FLAG     WHAT IT SPECIFIES
     *         |      Centre justification.
     *         !      Used with floating point numbers.
     *                   It specifies, if possible, that the
     *                   number will be centred with respect
     *                   to the decimal point.
     *                   If used with non floating point
     *                   numbers or the floating point number
     *                   does not contain a decimal point,
     *                   the ! flag is equivalent to |.
     * NEW CONVERSION CHARACTER:
     *      CHARACTER	WHAT IT SPECIFIES
     *         b      int, unsigned binary notation.
     */
  
    int v;

    static char str[128];
    char *sptr, lseen,Lseen;
  
    int  left, right, centre, maxchr,
    pad, len, width, sign, dprec,
    mod, prec,dot,gotfloat;
  
    v = 0;

    while(*fmt) {
	if(*fmt != '%') {
	    (*output)(*fmt,arg);
	    ++fmt;++v;
	    continue;
	} else if(*++fmt =='%') {
	    (*output)(*fmt++,arg);
	    ++v;
	    continue;
	}
	pad = ' ';
	centre = len = right = left = 0;
	gotfloat = dot = sign = mod = 0;
	Lseen = lseen  = dprec = 0;
	while(*fmt)  {		/* collect in any order */
	    if(*fmt == '-')  		left = 1;
	    else if(*fmt == '0')  	pad  = '0';
	    else if(*fmt == '+') 	sign = 1;
	    else if(*fmt == '#') 	mod = 1;
	    else if(*fmt == '|')        centre = 1;
	    else if(*fmt == '!')        centre = 1, dot = 1;
	    else
		break;
	    ++fmt;
	}
	if(isdigit(*fmt)) {
	    width = atoi(fmt);
	    while(isdigit(*fmt)) ++fmt;
	} else  if(*fmt == '*') {
	    width = va_arg(ap,int);
	    fmt++;
	} else
	    width = -1;
	if(*fmt == '.')  {
	    if(*++fmt == '*')  {
		maxchr = va_arg(ap,int);
		fmt++;
	    } else {
		maxchr = atoi(fmt);
		while(isdigit(*fmt)) ++fmt;
	    }
	} else
	    maxchr = -1;
    
	switch(*fmt) {		/* check for length modifier*/
	  case 'h': fmt++;break;
	  case 'l': lseen = 1; fmt++;break;
	  case 'L': Lseen = 1; fmt++;break;
	}
	sptr = str;
	switch(*fmt++) {
	  case 'c': sptr[0] = (char)va_arg(ap,int);
	    sptr[1] = 0;
	    break;
	  case 'b':
	    if(lseen) ultoa(va_arg(ap,long),sptr,2);
	    else utoa(va_arg(ap,int),sptr,2);
	    break;
	  case 'i':
	  case 'd': if(lseen) ltoa(va_arg(ap,long),sptr,10);
	  else itoa(va_arg(ap,int),sptr,10);
	  dprec=1;
	    break;
	  case 'u': if(lseen) ultoa(va_arg(ap,unsigned long),sptr,10);
	  else utoa(va_arg(ap,unsigned),sptr,10);
	  dprec=1;
	    break;
	  case 'o':	if(mod) *sptr = '0';
	    if(lseen)ltoa(va_arg(ap,long),&sptr[mod],8);
	    else itoa(va_arg(ap,int),&sptr[mod],8);
	    dprec = 1;
	    break;
	  case 's': sptr = va_arg(ap,char*); break;
	  case 'x': case 'X':
	    if(mod) {
		*sptr = '0', sptr[1] = *(fmt-1);
		mod++;
	    }
	    if(lseen)ultoa(va_arg(ap,long),&sptr[mod],16);
	    else utoa(va_arg(ap,int),&sptr[mod],16);
	    if(*(fmt-1) == 'X') {
		while(*sptr) {
		    *sptr = toupper(*sptr);
		    sptr++;
		}
		sptr = str;
	    }
	    dprec = 1;
	    break;
	    
	  case 'e': case 'E': case 'g': case 'G':
	  case 'f': {
	      int trunc = 0;
	      char type = *(fmt-1),c;
	      double d;
      
	      gotfloat = 1;
	      if(maxchr < 0) prec = 6;
	      else prec = maxchr,maxchr = -1;

	      if(Lseen)
		  d = va_arg(ap,double);
	      else
		 d = va_arg(ap,double);
      
	      if(type == 'g' || type == 'G') {
		  double ex;
		  if(d !=0)
		      ex = log10(d < 0? -d:d);
		  else
		      ex = 1;
		  if(ex < -4 || ex >= prec)
		      c = type - 2;
		  else
		      c = 'f';
		  trunc = 1;
	      } else
		  c = type;
	      if(mod) {
		  if(!prec) prec = 1;
		  trunc = 0;
	      }
	      sptr = fftoa(d,str, prec, c,trunc);
	  }
	    break;
	  case 'n': *va_arg(ap,int*) = v; continue;
	  case 'p':
	      ultoa((long)va_arg(ap,long*),sptr,16);
	    break;
	  default: (*output)(*(fmt-1),arg);
	    while(*fmt) {
		(*output)(*fmt++,arg);
		++v;
	    }
	    return v;
	}
    
	if(!len && sptr) len = strlen(sptr);
	if(sign) len += (*sptr == '-' ? 0: 1);

	if(dprec && maxchr >=0) {
	    dprec = maxchr;
	    pad = '0';
	    
	}else
	    dprec = 0;
	    
	
	if(maxchr > -1 && maxchr<len)
	    len = maxchr;
	if(width>len)
	    width = width -len;
	else if(dprec > len)
	    width = dprec - len;
	else
	    width = 0;
    
	if(centre) {
	    left = (right = width >> 1) + (width%2);
	    if(dot && gotfloat) {
		int d = 0,c;
		while(sptr[d] && sptr[d] != '.')
		    d++;
		if(sptr[d] == '.') {
		    c = (width + len)/2;
		    if(sign && sptr[0] != '-')
			d++;
		    if(c-d > 0) right = c-d;
		    else right = 0;
		    if(width - right > 0)
			left = width - right;
		    else
			left = 0;
		}
	    }
	} else 
	    if(!left)
		right = width;
	    else
		left = width;
    
	if(sign && !left && pad == '0') {
	    if(*sptr != '-') {
		(*output)('+',arg);
		++v;len--;
	    }
	    sign = 0;
	}
	while(right--) {(*output)(pad,arg); ++v;}
	if(sign && *sptr != '-') {(*output)('+',arg);len--;++v;}
	while(len--) {(*output)(*sptr++,arg);++v;}
	while(left--) {(*output)(pad,arg);++v;}
    }
    return v;
}

int printf(const char *fmt,...)
{
    int rtn;
    va_list ap;
    ap = va_start(ap,fmt);
    rtn = _Uprintf(fputc,stdout,fmt,ap);
    va_end(ap);
    return rtn;
}

int fprintf(FILE *fp, const char *fmt, ...)
{
    int rtn;
    va_list ap;
    ap = va_start(ap,fmt);
    rtn = _Uprintf(fputc,fp,fmt,ap);
    va_end(ap);
    return rtn;
}

int sprintf(char *str, const char *fmt, ...)
{
    int rtn;
    va_list ap;
    ap = va_start(ap,fmt);
    rtn = _Uprintf(charout_,&str,fmt,ap);
    va_end(ap);
    return rtn;
}

int vfprintf(FILE *fp, const char *fmt,va_list ap)
{
    int rtn;
    rtn = _Uprintf(fputc,fp,fmt,ap);
    return rtn;
}


int vsprintf(char *str, const char *fmt,va_list ap)
{
    int rtn;
    rtn = _Uprintf(charout_,&str,fmt,ap);
    return rtn;
}








