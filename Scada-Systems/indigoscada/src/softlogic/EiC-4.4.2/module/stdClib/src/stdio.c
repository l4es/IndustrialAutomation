/* stdio.c
 *
 *	(C) Copyright Dec 19 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

/* This file is broken into 2 parts
 * the first part defines the interface routines
 * and the 2nd part adds the interface routine
 * to EiC's look up tables.
 */


#ifndef  _USE_POSIX
#define  _USE_POSIX
#endif

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif


#include <stdio.h>

#if 0
#ifdef _IRIX
extern FILE *popen(const char *, const char *);
extern int pclose(FILE *);
#endif
#endif


#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include "eic.h"
#include "stdliblocal.h"


#ifdef PPCLIB
void outbyte(int ch);
#endif


/* The following code provides for an open FILE cleanup
mechanism.  EiC will call _ffexit, cleaning up all open
FILEs which were opened in the side effects being discarded.
	*/
static size_t NextFopenEntry = 4;

static size_t book1[FOPEN_MAX] = {1,2,3};
static FILE * book2[FOPEN_MAX];

size_t fopen_NextEntryNum(void) {
	return NextFopenEntry;
}

/* We assume that eic_fopen() is always closed by eic_fclose()
and eic_popen() is aways closed by eic_pclose() so that book[i]
being non-zero implies that the associated FILE is still open.
This ignores the possibility that a file opened in interpreted
code can be closed in compiled code, but such a situation is
ill advised in any event.
	*/
void _ffexit(size_t Entry) {
	int i;
	for(i = 0; i < FOPEN_MAX; i += 1) {
	  if(book1[i] >= Entry) {
	    fclose(stdin + i);
	    book1[i] = 0;
	    book2[i] = NULL;
	  }
	}
	return;
}

/*  INTERFACE FUNCTIONS */

int charin_(char **buf)
{
    int ch;
    ch = **buf;
    *buf += 1;
    if(!ch)
	ch = EOF;
    return ch;
}

int charback_(int ch, char **buf)
{
    *buf -= 1;
    return ch;
}

int charout_(int c, char **buf)
{
    char *s;
    s = *buf;
    *s++ = c;
    *s = 0;
    *buf = s;
    return 1;
}

int _eicUprintf(int (*output)(), void *arg, char *fmt, arg_list ap)
{
    /* copy of _UPRINTF that works with EiC's run time stack.
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
	    width = nextarg(ap,int);
	    fmt++;
	} else
	    width = -1;
	if(*fmt == '.')  {
	    if(*++fmt == '*')  {
		maxchr = nextarg(ap,int);
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
	  case 'c': sptr[0] = (char)nextarg(ap,int);
	    sptr[1] = 0;
	    break;
	  case 'b':
	    if(lseen) ultoa(nextarg(ap,long),sptr,2);
	    else utoa(nextarg(ap,int),sptr,2);
	    break;
	  case 'i':
	  case 'd': if(lseen) ltoa(nextarg(ap,long),sptr,10);
	  else itoa(nextarg(ap,int),sptr,10);
	  dprec=1;
	    break;
	  case 'u': if(lseen) ultoa(nextarg(ap,unsigned long),sptr,10);
	  else utoa(nextarg(ap,unsigned),sptr,10);
	  dprec=1;
	    break;
	  case 'o':	if(mod) *sptr = '0';
	    if(lseen)ltoa(nextarg(ap,long),&sptr[mod],8);
	    else itoa(nextarg(ap,int),&sptr[mod],8);
	    dprec = 1;
	    break;
	  case 's': sptr = nextarg(ap,ptr_t).p; break;
	  case 'x': case 'X':
	    if(mod) {
		*sptr = '0', sptr[1] = *(fmt-1);
		mod++;
	    }
	    if(lseen)ultoa(nextarg(ap,long),&sptr[mod],16);
	    else utoa(nextarg(ap,int),&sptr[mod],16);
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
		  d = nextarg(ap,double);
	      else
		 d = nextarg(ap,double);
      
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
	  case 'n': *(int*)nextarg(ap,ptr_t).p = v; continue;
	  case 'p':
	      ultoa((long)nextarg(ap,ptr_t).p,sptr,16);
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



int _eicUscanf(int (*input)(), int (*uget)(),
	    void *arg, const char *fmt, arg_list ap)
{
    /*_USCANF
     *
     * (C) May 10 1995 Edmond J.Breen.
     *           ALL RIGHTS RESERVED.
     *
     * Purpose: To perform formatted reading from a function.
     *          _Uscanf performs a superset of scanf type
     *          operations.
     *
     * 		  _USCANF   RECOGNISES:
     *    
     *     %[*] [width] [h|l|L] [b|d|i|o|x|X|u|c|s|f|e|E|g|G|p|n|%] 
     *     %[*] [width] [scan-set]
     *
     * CONVERSION CHARACTERS:
     *         d, i, o, x, X, u, c, s
     *         f, e, E, g, G, p, n, %
     *    plus b   (see below).
     *
     * LENGTH MODIFIES:
     *          l, L, and h.
     *
     *     *    optional assignment-suppression character
     * WIDTH    optional integer width of field specifier
     *
     * SCAN-SET
     *      [...]   matches the longest non-emtpy string
     *              of input from the set between the brackects.
     *      [^...]  matches the longest non-emtpy string
     *              of input NOT from the set between the brackects.
     *      []...] and [^]...]
     *                  includes ] as part of the scan-set.
     *
     * NEW CONVERSION CHARACTER:
     *      CHARACTER	WHAT IT SPECIFIES
     *         b      int, unsigned binary notation.
     *
     * STANDARD C AUXILIARY FUNCTIONS REQUIRED 
     *    strtol, strtod, strtoul
     *
     * LIMITATIONS:
     *       (1) long double handled as double
     *       (2) error checking could be improved
     */

    static char field[256], *p;
    
    char *carg, sbuf;
    int type;
    int  wast, width,cwidth, ch,  base,cnt;
    void *vptr;
    int v;

    
    cnt = v = 0;
    
    while(*fmt != '\0') {
	if(isspace(*fmt)) {  /* match white space */
	    while(*fmt && isspace(*fmt)) ++fmt;
	    if(*fmt) {
		while(isspace((ch=(*input)(arg))))
		    ++cnt;
		(*uget)(ch,arg);
	    }
	    continue;
	}
	if(*fmt != '%') { /* match literal text */
	    while(*fmt && !isspace(*fmt) && *fmt != '%')
		if((ch=(*input)(arg)) == *fmt) 
		    ++fmt,++cnt;
		else
		    return v;
	    continue;
	}
	    
	if(*++fmt == '*') {
	    vptr = &sbuf;
	    wast = 1;
	    ++fmt;
	} else if(*fmt == '%') {
	    cnt++;
	    fmt++;
	    continue;
	} else  {
	    wast = 0;
	    vptr = nextarg(ap,ptr_t).p;
	}

	for(width = 0;isdigit(*fmt);fmt++)
	    width = *fmt - '0' + width * 10;
	if(!width)
	    width = INT_MAX, cwidth = 1;
	else
	    cwidth = width;

	if(*fmt != 'c' && *fmt != '[' && *fmt != 'n') {
	    /* strip leading white space */
	    while(isspace(ch = (*input)(arg)))
		++cnt;
	    if(ch == EOF) {
		if(v)
		    break;
		else
		    return EOF;
	    }
	    (*uget)(ch,arg);
	}

	
	switch(*fmt) {
	  case 'h': type = 'h'; ++fmt; break;
	  case 'l': type = 'l'; ++fmt; break;
	  case 'L': type = 'L'; ++fmt; break;
	  default:  type = 0;
	}
	    
	switch(*fmt) {
	  case 'c': carg = vptr;
	    while(cwidth--) {
		*carg = (*input)(arg);
		++cnt;
		if(carg != &sbuf)
		    ++carg;
	    }
	    break;
	  case 's':
	    carg = vptr;
	    while(width--) {
		if((*carg = (*input)(arg)) == EOF)
		    break;
		++cnt;
		if(isspace(*carg)) {
		    (*uget)(*carg,arg);
		    --cnt;
		    break;
		} if(carg != &sbuf)
		    ++carg;
	    }
	    *carg = '\0';
	    break;
	  case '[':
	    ++fmt;
	    
	    if(*fmt == '^') {++fmt; type = 0;}
	    else type = 1;
	    
	    p = (char*)fmt;
	    if(*p == ']') ++p;
	    
	    while(*p && *p != ']') ++p;
	    cwidth = p - fmt;
	    if(cwidth == 0) return EOF;
	    carg = vptr;
	    while((ch = (*input)(arg)) != EOF) 
		if(type) {
		    if(memchr(fmt,ch,cwidth)) {
			if(!wast)
			    *carg++ = ch;
			++cnt;
		    }else {
			(*uget)(ch,arg);
			break;
		    }
		} else 
		    if(!memchr(fmt,ch,cwidth)) {
			if(!wast)
			    *carg++ = ch;
			++cnt;
		    }else {
			(*uget)(ch,arg);
			break;
		    }
	    *carg = '\0';
	    fmt += cwidth;
	    break;	    
	  case 'e': case 'f': case 'g':
	  case 'E': case 'G':
	    p = field;
	    ch = (*input)(arg);
	    if(ch == '-' || ch == '+') {
		++cnt;
		*p++ = ch;
	    } else
		(*uget)(ch,arg);
	    while(width-- && isdigit(ch=(*input)(arg))) {
		++cnt; *p++ = ch;
	    }
	    if(ch == '.' && width-- > 0) {
		*p++ = '.';
		++cnt;
		while(isdigit(ch=(*input)(arg)) && width--) {
		    ++cnt;
		    *p++ = ch;
		}
	    }
	    if((ch == 'e' || ch == 'E') && width--) {
		*p++ = ch;
		ch = (*input)(arg);
		if(ch != '+' && ch != '-' && !isdigit(ch) &&
		   ch != EOF && !isspace(ch)) {
		    (*uget)(ch,arg);
		    ch = *--p;
		} else
		    cnt++;
		    
	    }
	    if((ch == '+' || ch == '-') && width--) {
		*p++ = ch;
		++cnt;
		ch = (*input)(arg);
	    }		
	    while(isdigit(ch) && width--) {
		*p++ = ch;
		++cnt;
		ch = (*input)(arg);
	    }
	    (*uget)(ch,arg);
	    if(p == field)
		return v;
	    *p = '\0';
	    if(!wast){
		double d = strtod(field,NULL);
		if(!type || type == 'h')
		    *(float*)vptr = (float)d;
		else if(type == 'l' || type == 'L')
		    *(double*)vptr = d;
	    }
	    break;
	  case 'n':
	    if(type == 'l')
		*(long*)vptr = cnt;
	    else if(type == 'h')
		*(short*)vptr = cnt;
	    else
		*(int*)vptr = cnt;
	    v--;
	    break;	    
	  default:
	    switch(*fmt) {
	    case 'b': base = 2; break;
	    case 'i':
	    case 'd':
	    case 'u':
		ch = (*input)(arg);
		if(ch == '0') base = 8;
		else  base = 10;

		(*uget)(ch,arg);
		break;
	    case 'o': base = 8;  break;
	    case 'x':
	    case 'X': 
	    case 'p': base = 16; break;
	    default:
		return v;
	    }
	    p = field;
	    while(width-- && !isspace(ch=(*input)(arg)) && ch != EOF) {
		*p++ = ch;
		++cnt;
	    }
	    if(width > 0)
		(*uget)(ch,arg);	
	    if(wast)
		break;
	    if(p == field)
		return v;
	    *p = '\0';
	{
	    char *endptr;
	    if(*fmt == 'd' || *fmt == 'i') { /* signed conversion */
		long lval = strtol(field,&endptr,base);
		if(type) {
		    if(type == 'h')
			*(short *)vptr =(short) lval;
		    else 
			*(long *)vptr = lval;
		} else
		    *(int *)vptr = (int) lval;
			
	    } else {
		unsigned long ulval = strtoul(field,&endptr,base);
		if(type) {
		    if(type == 'h')
			*(unsigned short *)vptr =(unsigned short) ulval;
		    else
			*(unsigned long *)vptr = ulval;
		} else
		    *(unsigned *)vptr = (unsigned)ulval;
	    }
	    if(endptr == field) /* failed */
		return v;
	    while(--p >= endptr)
		(*uget)(*p,arg);
	}
	    break;
	}
	++fmt;
	++v;
    }
    return v;
}

#ifndef NO_PIPE
val_t eic_popen()
{
    val_t v;
    arg_list ap = getargs();

#ifdef WIN32
    v.p.sp = v.p.p = _popen(arg(0,ap,ptr_t).p,arg(1,ap,ptr_t).p);
#else
    v.p.sp = v.p.p = popen(arg(0,ap,ptr_t).p,arg(1,ap,ptr_t).p);
#endif

    if((FILE *)(v.p.p) != NULL) {
      int i;
      for(i=1;i<FOPEN_MAX;i++)
	if(book1[i] == 0)
	  break;
      book1[i] = NextFopenEntry++;
      book2[i] = (FILE *)(v.p.p);
    }
    setEp( v.p, sizeof(FILE) );
    
    return  v;
}
val_t eic_pclose()
{
  FILE *ptr;
  val_t v;
  int i;

  arg_list ap = getargs();
  
  ptr = arg(0,ap,ptr_t).p;

#ifdef WIN32
  v.ival = _pclose(ptr);
#else
  v.ival = pclose(ptr);
#endif
  
  for(i=0;i<FOPEN_MAX;i++)
    if(ptr == book2[i])
      break;
  if(i<FOPEN_MAX) {
    book1[i] = 0;
    book2[i] = NULL;
  }
  return  v;
}
#endif




/* STDIO.C STUFF */
val_t _get_stdin(void)
{
    val_t v;
    /*
     * This function exists so that EiC can get the address stdin.
     */
    v.p.sp = v.p.p = stdin;
    setEp( v.p, sizeof(*stdin) );
    return v;
}
val_t _get_stdout(void)
{
    val_t v;
    /*
     * This function exists so that EiC can get the address stdout.
     */
    v.p.sp = v.p.p = stdout;
    setEp( v.p, sizeof(*stdout) );
    return v;
}
val_t _get_stderr(void)
{
    val_t v;
    /*
     * This function exists so that EiC can get the address stderr.
     */
    v.p.sp = v.p.p = stderr;
    setEp( v.p, sizeof(*stderr) );
    return v;
}


#ifndef NO_FSEEK
val_t eic_ftell(void)
{
    val_t v;
    v.lval = ftell(arg(0,getargs(),ptr_t).p);
    return v;
}

val_t eic_fseek(void)
{
    val_t v;
    v.ival = fseek(arg(0,getargs(),ptr_t).p,
		   arg(1,getargs(),long),
		   arg(2,getargs(),int));
    return v;
}
#endif


val_t eic_printf(void)
{
    val_t v;
    arg_list ap = getargs();
    char *fmt;
    
    fmt = nextarg(ap,ptr_t).p;
    #ifdef PPCLIB
    v.ival = _eicUprintf(outbyte,stdout,fmt,ap);
    #else
    v.ival = _eicUprintf(fputc,stdout,fmt,ap);
    #endif
    return v;
}

val_t eic_fprintf(void)
{
    val_t v;
    arg_list ap = getargs();
    FILE *fp;
    char *fmt;
    
    fp = nextarg(ap,ptr_t).p;
    fmt = nextarg(ap,ptr_t).p;
    v.ival = _eicUprintf(fputc,fp,fmt,ap);
    return v;
}

val_t eic_vfprintf(void)
{
    val_t v;
    arg_list ap = getargs();
    arg_list ags;
    FILE *fp;
    char *fmt;
    
    fp = nextarg(ap,ptr_t).p;
    fmt = nextarg(ap,ptr_t).p;
    ags = nextarg(ap,arg_list);
    v.ival = _eicUprintf(fputc,fp,fmt,ags);
    return v;
}

val_t eic_sprintf(void)
{
    val_t v;
    arg_list ap = getargs();
    char *fmt, *str; 
    str = nextarg(ap,ptr_t).p;
    fmt = nextarg(ap,ptr_t).p;    
    v.ival = _eicUprintf(charout_,&str,fmt,ap);
    return v;
}


val_t eic_vsprintf(void)
{
    val_t v;
    arg_list ap = getargs();
    arg_list ags;
    char * str;
    char *fmt;
    
    str = nextarg(ap,ptr_t).p;
    fmt = nextarg(ap,ptr_t).p;
    ags = nextarg(ap,arg_list);
    v.ival = _eicUprintf(charout_,&str,fmt,ags);
    return v;
}

#ifndef NO_FILEIO
val_t eic_freopen(void)
{
    val_t v;
    arg_list ap = getargs();
    v.p.sp = v.p.p = freopen(arg(0,ap,ptr_t).p,
			     arg(1,ap,ptr_t).p,
			     arg(2,ap,ptr_t).p);
    
    setEp( v.p, sizeof(FILE) );
    return v;
}

val_t eic_fopen(void)
{
    val_t v;
    arg_list ap = getargs();
    v.p.sp = v.p.p = fopen(arg(0,ap,ptr_t).p,
			   arg(1,ap,ptr_t).p);
    if((FILE *)(v.p.p) != NULL) {
      int i;
      for(i=1;i<FOPEN_MAX;i++)
	if(book1[i] == 0)
	  break;
      book1[i] = NextFopenEntry++;
      book2[i] = (FILE *)(v.p.p);
    }
    setEp( v.p, sizeof(FILE) );
    return v;
}

val_t eic_ungetc(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = ungetc(arg(0,ap,int),
		    arg(1,ap,ptr_t).p);
    return v;
}
    
val_t eic_fgetc(void)
{
    val_t v;
    v.ival = fgetc(nextarg(getargs(),ptr_t).p);
    return v;
}
val_t eic_fclose(void)
{
  FILE *ptr;
  val_t v;
  int i;

  ptr = nextarg(getargs(),ptr_t).p;

  for(i=0;i<FOPEN_MAX;i++)
    if(ptr == book2[i])
      break;
  if(i<FOPEN_MAX) {
    book1[i] = 0;
    book2[i] = NULL;
  }

  v.ival = fclose(ptr);
  return v;
}
val_t eic_fflush(void)
{
    val_t v;
    v.ival = fflush(nextarg(getargs(),ptr_t).p);
    return v;
}

val_t eic_fputc(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = fputc(arg(0,ap,int),
		   arg(1,ap,ptr_t).p);
    return v;
}

val_t eic_puts(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = puts(arg(0,ap,ptr_t).p);
    return v;
}

val_t eic_fputs(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = fputs(arg(0,ap,ptr_t).p,
		   arg(1,ap,ptr_t).p);
    return v;
}

val_t eic_fgets(void)
{
    val_t v;
    int n;
    arg_list ap = getargs();
    getptrarg(0,v.p);

    n = arg(1,ap,int);
    checkEp(v.p,n);

    v.p.p = fgets(v.p.p,n, arg(2,ap,ptr_t).p);
    return v;
}

val_t eic_gets(void)
{
    int n;
    val_t v;
    getptrarg(0,v.p);

    n = (char*)v.p.ep - (char*)v.p.sp;
    v.p.p = fgets(v.p.p,n,stdin);
    return v;
}

val_t eic_fread(void)
{
    val_t v;
    arg_list ap = getargs();
    v.szval = fread(arg(0,ap,ptr_t).p,
		   arg(1,ap,size_t),
		   arg(2,ap,size_t),
		   arg(3,ap,ptr_t).p);
    return v;
}

val_t eic_fwrite(void)
{
    val_t v;
    arg_list ap;
    ap = getargs();
    v.szval = fwrite(arg(0,ap,ptr_t).p,
		   arg(1,ap,size_t),
		   arg(2,ap,size_t),
		   arg(3,ap,ptr_t).p);
    return v;
}

val_t eic_fscanf(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = _eicUscanf(fgetc,ungetc,
		     arg(0,ap,ptr_t).p,
		     arg(1,ap,ptr_t).p,ap-2);
    return v;
}

val_t eic_feof(void)
{
    val_t v;
    getptrarg(0,v.p);
    v.ival = feof((FILE *)(v.p.p));
    return v;
}

val_t eic_ferror(void)
{
    val_t v;
    getptrarg(0,v.p);
    v.ival = ferror((FILE *)(v.p.p));
    return v;
}

val_t eic_rewind(void)
{
    val_t v;
    getptrarg(0,v.p);
    rewind((FILE *)(v.p.p));
    return v;
}

val_t eic_fsetpos(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = fsetpos(arg(0,ap,ptr_t).p,
		   arg(1,ap,ptr_t).p);
    return v;
}

val_t eic_fgetpos(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = fgetpos(arg(0,ap,ptr_t).p,
		   arg(1,ap,ptr_t).p);
    return v;
}

#endif

val_t eic_scanf(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = _eicUscanf(fgetc,ungetc,
		     stdin,
		     arg(0,ap,ptr_t).p,ap-1);
    return v;
}

val_t eic_sscanf(void)
{
    val_t v;
    char * str;
    arg_list ap = getargs();
    str = arg(0,ap,ptr_t).p;
    v.ival = _eicUscanf(charin_,charback_,
		     &str,
		     arg(1,ap,ptr_t).p,ap-2);
    return v;
}

val_t eic_setvbuf(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = setvbuf(arg(0,ap,ptr_t).p,
		     arg(1,ap,ptr_t).p,
		     arg(2,ap,int),
		     arg(3,ap,size_t));
    return v;
}

#ifndef NO_TMPFILE
val_t eic_tmpnam(void)
{
    val_t v;
    v.p.sp = v.p.p = tmpnam(arg(0,getargs(),ptr_t).p);
    setEp( v.p, strlen(v.p.p) + 1 );
    return v;
}
val_t eic_tmpfile(void)
{
    val_t v;
    v.p.sp = v.p.p = tmpfile();
    setEp( v.p, strlen(v.p.p) + 1 );
    return v;
}

val_t eic_rename(void)
{
    val_t v;
#ifdef WIN32
    v.ival = rename(arg(0,getargs(),ptr_t).p,
		  arg(1,getargs(),ptr_t).p);
#else
    v.ival = link(arg(0,getargs(),ptr_t).p,
		  arg(1,getargs(),ptr_t).p);
    if(v.ival == 0)
	v.ival =  unlink(arg(0,getargs(),ptr_t).p);
#endif
    return v;
}


val_t eic_clearerr(void)
{
    val_t v;
    clearerr((FILE *)(arg(0,getargs(),ptr_t).p));
    return v;
}

val_t eic_perror(void)
{
    val_t v;
    perror(arg(0,getargs(),ptr_t).p);
    return v;
}
#endif

/**********************************************************************************/

void module_stdio(void)
{

    book2[0] = stdin;
    book2[1] = stdout;
    book2[2] = stderr;

    /* stdio.h stuff that were macros */
    EiC_add_builtinfunc("puts", eic_puts);
    EiC_add_builtinfunc("feof", eic_feof);
    EiC_add_builtinfunc("ferror", eic_ferror);
    EiC_add_builtinfunc("rewind", eic_rewind);
    EiC_add_builtinfunc("fsetpos", eic_fsetpos);
    EiC_add_builtinfunc("fgetpos", eic_fgetpos);

    /* stdio.h stuff */
    EiC_add_builtinfunc("_get_stdin",_get_stdin);
    EiC_add_builtinfunc("_get_stdout",_get_stdout);
    EiC_add_builtinfunc("_get_stderr",_get_stderr);
    EiC_add_builtinfunc("ftell", eic_ftell);
    EiC_add_builtinfunc("freopen",eic_freopen);	
    EiC_add_builtinfunc("fopen",eic_fopen);	
    EiC_add_builtinfunc("printf", eic_printf);
    EiC_add_builtinfunc("fprintf", eic_fprintf);
    EiC_add_builtinfunc("sprintf", eic_sprintf);
    EiC_add_builtinfunc("vfprintf", eic_vfprintf);
    EiC_add_builtinfunc("vsprintf", eic_vsprintf);
    EiC_add_builtinfunc("ungetc", eic_ungetc);
    EiC_add_builtinfunc("fgetc", eic_fgetc);
    EiC_add_builtinfunc("fputc", eic_fputc);
    EiC_add_builtinfunc("fputs", eic_fputs);
    EiC_add_builtinfunc("fgets", eic_fgets);
    EiC_add_builtinfunc("gets", eic_gets);
    EiC_add_builtinfunc("fread", eic_fread);
    EiC_add_builtinfunc("fwrite", eic_fwrite);
    EiC_add_builtinfunc("fflush", eic_fflush);
    EiC_add_builtinfunc("fclose", eic_fclose);
    EiC_add_builtinfunc("fscanf", eic_fscanf);
    EiC_add_builtinfunc("sscanf", eic_sscanf);
    EiC_add_builtinfunc("scanf", eic_scanf);
    EiC_add_builtinfunc("setvbuf", eic_setvbuf);
    EiC_add_builtinfunc("tmpnam", eic_tmpnam);
    EiC_add_builtinfunc("tmpfile", eic_tmpfile);
    EiC_add_builtinfunc("rename",eic_rename);
    EiC_add_builtinfunc("fseek", eic_fseek);
    EiC_add_builtinfunc("clearerr", eic_clearerr);
    EiC_add_builtinfunc("perror", eic_perror);
   

#ifndef NO_PIPE
    EiC_add_builtinfunc("popen", eic_popen);
    EiC_add_builtinfunc("pclose", eic_pclose);
#endif

}

