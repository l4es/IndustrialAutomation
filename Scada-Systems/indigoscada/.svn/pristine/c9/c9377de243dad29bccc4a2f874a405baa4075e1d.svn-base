#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>


/* The `tryit' and `testtryit' functions are from
 *  P.J. Plauger's book: "The standard C library"
 */
typedef struct {
    char c;
} Cstruct;

int tryit(const char *fmt, ...)
{
    int ctr = 0;
    va_list ap;

    va_start(ap,fmt);
    for(; *fmt;++fmt)
	switch(*fmt) {
	  case 'i':
	    assert(va_arg(ap,int) == ++ctr);
	    break;
	  case 'd':
	    assert(va_arg(ap,double) == ++ctr);
	    break;
	  case 'p':
	    assert(va_arg(ap, char *)[0] = ++ctr);
	    break;
	  case 's':
	    /* EiC fails here */
	    assert(va_arg(ap,Cstruct).c == ++ctr);
	    break;
	}
    va_end(ap);
    return ctr;
}

void testtryit(void)
{
    Cstruct x = {3};

//    assert(tryit("iisdi",'\1', 2, x, 4.0, 5) == 5);
    assert(tryit("") == 0);
    assert(tryit("pdp", "\1", 2.0, "\3") == 3);
}

/*--------------------------------------------*/

void print_args (int num, ...)
{
  va_list  ap;
  va_start (ap, num);
  while (num-- > 0)
    printf ("%d ", va_arg (ap, int));
  va_end (ap);
  printf ("\n");
}

void tprint (void)
{
  print_args (3, 6, 34, -4);
  print_args (5, -5, 6, 98, 23, 4);
}
/*--------------------------------------------*/

void mprintf(char *fmt,...)
{
    va_list ap;
    short radix;
    char str[20], *s;

    va_start(ap,fmt);
    while(*fmt != 0) {
	if(*fmt != '%') {
	    putchar(*fmt++);
	    continue;
	}
	radix = 0;
	switch(*++fmt) {
	  case 's': s = va_arg(ap,char*);break;
	  case 'd': radix = 10;break;
	  case 'b': radix =  2;break;
	  default:		/* unrecognisable input */
	    while(*fmt !=0)
		putchar(*fmt++);
	    return;
	}
	++fmt;
	if(radix>0)
	    s = itoa(va_arg(ap,int),str,radix);
	while(*s != 0)
	    putchar(*s++);
    }
}

void error(char * fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    vfprintf(stderr,fmt,args);
    va_end(args);
}
    

void serror(char *str, char * fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    vsprintf(str,fmt,args);
    va_end(args);
}

int main(void)
{
    char buff[100];
    mprintf("125 :-> %d%d%d\n",1,2,5);
    error("error %d %s %.3f ",13,"hello",13.333);
    serror(buff,"error %d %s %.3f ",13,"hello",13.333);
    mprintf(":-> error 13 hello 13.333\n");
    printf("%s\n",buff);
    tprint();
    testtryit();
    return 0;
}

#ifdef EiCTeStS
main();
"\nTo clean :rm error,mprintf\n";
#endif










