/* this file is for creating a version of EiC which
 *  does not link to its implementation of the
 *  standard C library nor does it use readline.
 *
 *
 * To build this version: 
 *		
 * 		make -f makefile.ppc 
 *
 *  which makes the executabe seic, (small eic) and seic.sr (an S-record)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eic.h"
#include "../module/stdClib/src/stdClib2eic.h" 
#include "preproc.h"

#define str(x)  #x
#define xstr(x) str(x)

#ifdef POWERPC
void prs(char *str);
val_t eic_gets(void);
val_t eic_puts(void);
val_t eic_getch(void);
val_t eic_putch(void);
val_t eic_getchq(void);
val_t eic_in(void);
val_t eic_out(void);
val_t eic_load(void);

extern char *startstr;

static char loadstring[4096]="

void puts(char *str);
char *gets(char *str);
int getch(void);
void putch(int ch);
int getchq(void);
int in(unsigned long addr);
void out(unsigned char val, unsigned long addr);
int load(char *str);
int printf(const char *fmt, ...);
int sprintf(char *buf, const char * fmt, ...);
int scanf(const char *fmt, ...);
int sscanf(const char *str,const char *fmt, ...);

typedef unsigned size_t;

void *memmove(void * dst, const void * src , size_t n);
char *strcpy(char * dst, const char * src);
char *strncpy(char * dst, const char * src, size_t n);
char *strcat(char * s1, const char * s2);
char *strncat(char * s1, const char *s2, size_t n);
int memcmp(const void * s1, const void * s2, size_t n);
int strcmp(const char * s1, const char * s2);
int strcoll(const char * s1, const char * s2);
int strncmp(const char * s1, const char *s2, size_t n);
size_t strxfrm(char *dst, const char * src, size_t n);
void *memchr(const void *s, int c, size_t n);
char *strchr(const char *s, int c);
size_t strcspn(const char *s, const char *reject);
char *strpbrk(const char *s, const char *accept);
char *strrchr(const char *s, int c);
size_t strspn(const char *s, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *s, const char *delim);
void *memset(void *s, int c, size_t n);
char *strerror(int n);
size_t strlen(const char *s);
char * strrev(char * s);

#define RAND_MAX 32767
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MB_CUR_MAX 1

typedef struct { int quot, rem; }div_t;

typedef struct { long quot, rem; }ldiv_t;

div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
void * malloc(size_t n);
void * calloc(size_t x, size_t y);
void * realloc(void * ptr, size_t n);
#define  free(x)  (free)(&(x));
void  (free)(void *ptr);
#define strtoul(x,y,z) strtol(x,y,z)
int rand(void);
void srand(unsigned int seed);
double strtod(const char *str, char **endptr);
long strtol(const char *str, char **endptr,int base);

int atoi(const char *s);
double atof(const char *str);
long atol(const char *s);
void abort(void);
char * _itoa(int n,char *str, int radix, int mod);
char * _ltoa(int n,char *str, int radix, int mod);
char *fftoa(double d, char *str, int p, char type, int trunc);
#define itoa(x,y,z)     _itoa(x,y,z,1)
#define utoa(x,y,z)     _itoa(x,y,z,2)
#define ltoa(x,y,z)     _ltoa(x,y,z,1)
#define ultoa(x,y,z)	_ltoa(x,y,z,2)


double acos(double a);
double asin(double a);
double atan(double a);
double atan2(double a, double b);
double cos(double a);
double sin(double a);
double tan(double a);
double cosh(double a);
double sinh(double a);
double tanh(double a);
double exp(double a);
double frexp(double a, int * b);
double ldexp(double a, int b);
double log(double a);
double log10(double a);
double modf(double a, double *b);
double pow(double a, double b);
double sqrt(double a);
double ceil(double a);
double fabs(double a);
double floor(double a);
double fmod(double a, double b);
puts(\"\\nDone loading prototypes...\");
";


#endif

int  main(int argc, char ** argv)
{

    EiC_init_EiC();

    /* the macro PLATFORM 
     * is passed in by the 
     * compiler
     */

    dodefine("_EiC");
    dodefine(&(xstr(PLATFORM)[1]));

    #ifdef PPCLIB
    prs("Starting EiC...\n");
    setvbuf(stdout,NULL,_IONBF,0);
    EiC_add_builtinfunc("puts",eic_puts);
    EiC_add_builtinfunc("gets",eic_gets);
    EiC_add_builtinfunc("putch",eic_putch);
    EiC_add_builtinfunc("getch",eic_getch);
    EiC_add_builtinfunc("getchq",eic_getchq);
    EiC_add_builtinfunc("in",eic_in);
    EiC_add_builtinfunc("out",eic_out);
    EiC_add_builtinfunc("load",eic_load);
    
    EiC_add_builtinfunc("printf",eic_printf);
    EiC_add_builtinfunc("sprintf",eic_sprintf);
    EiC_add_builtinfunc("scanf",eic_scanf);
    EiC_add_builtinfunc("sscanf",eic_sscanf);
    EiC_add_builtinfunc("setvbuf",eic_setvbuf);
    
      /* string .h */
    EiC_add_builtinfunc("memcpy",eic_memcpy);
    EiC_add_builtinfunc("memmove",eic_memmove);
    EiC_add_builtinfunc("strcpy",eic_strcpy);
    EiC_add_builtinfunc("strncpy",eic_strncpy);
    EiC_add_builtinfunc("strcat",eic_strcat);
    EiC_add_builtinfunc("strncat",eic_strncat);
    EiC_add_builtinfunc("memcmp",eic_memcmp);
    EiC_add_builtinfunc("strcmp",eic_strcmp);
    EiC_add_builtinfunc("strcoll",eic_strcoll);
    EiC_add_builtinfunc("strncmp",eic_strncmp);
    EiC_add_builtinfunc("strxfrm",eic_strxfrm);
    EiC_add_builtinfunc("memchr",eic_memchr);
    EiC_add_builtinfunc("strchr",eic_strchr);
    EiC_add_builtinfunc("strcspn",eic_strcspn);
    EiC_add_builtinfunc("strpbrk",eic_strpbrk);
    EiC_add_builtinfunc("strrchr",eic_strrchr);
    EiC_add_builtinfunc("strspn",eic_strspn);
    EiC_add_builtinfunc("strstr",eic_strstr);
    EiC_add_builtinfunc("strtok",eic_strtok);
    EiC_add_builtinfunc("memset",eic_memset);
    EiC_add_builtinfunc("strerror",eic_strerror);
    EiC_add_builtinfunc("strlen",eic_strlen);  
    EiC_add_builtinfunc("strrev", eic_strrev);
    
     /* stdlib.h */
    EiC_add_builtinfunc("_itoa", eic_itoa);
    EiC_add_builtinfunc("_ltoa", eic_ltoa);
    EiC_add_builtinfunc("fftoa", eic_fftoa);
    EiC_add_builtinfunc("malloc", eic_malloc);
    EiC_add_builtinfunc("calloc", eic_calloc);
    EiC_add_builtinfunc("realloc", eic_realloc);
    EiC_add_builtinfunc("free", eic_free);
    EiC_add_builtinfunc("strtod", eic_strtod);
    EiC_add_builtinfunc("strtol", eic_strtol);
    EiC_add_builtinfunc("strtoul", eic_strtoul);
    EiC_add_builtinfunc("rand", eic_rand);
    EiC_add_builtinfunc("srand", eic_srand);
    EiC_add_builtinfunc("atoi", eic_atoi);
    EiC_add_builtinfunc("atof", eic_atof);
    EiC_add_builtinfunc("atol", eic_atol);
    EiC_add_builtinfunc("abort",eic_exit);

    EiC_add_builtinfunc("eic_exit",eic_exit);

    EiC_add_builtinfunc("div",eic_div);
    EiC_add_builtinfunc("ldiv",eic_ldiv);
    EiC_add_builtinfunc("acos",eic_acos);
    EiC_add_builtinfunc("asin",eic_asin);
    EiC_add_builtinfunc("atan",eic_atan);
    EiC_add_builtinfunc("atan2",eic_atan2);
    EiC_add_builtinfunc("cos",eic_cos);
    EiC_add_builtinfunc("sin",eic_sin);
    EiC_add_builtinfunc("tan",eic_tan);
    EiC_add_builtinfunc("cosh",eic_cosh);
    EiC_add_builtinfunc("sinh",eic_sinh);
    EiC_add_builtinfunc("tanh",eic_tanh);
    EiC_add_builtinfunc("exp",eic_exp);
    EiC_add_builtinfunc("frexp",eic_frexp);
    EiC_add_builtinfunc("ldexp",eic_ldexp);
    EiC_add_builtinfunc("log",eic_log);
    EiC_add_builtinfunc("log10",eic_log10);
    EiC_add_builtinfunc("modf",eic_modf);
    EiC_add_builtinfunc("pow",eic_pow);
    EiC_add_builtinfunc("sqrt",eic_sqrt);
    EiC_add_builtinfunc("ceil",eic_ceil);
    EiC_add_builtinfunc("fabs",eic_fabs);
    EiC_add_builtinfunc("floor",eic_floor);
    EiC_add_builtinfunc("fmod",eic_fmod);


    startstr=loadstring; /* load defines */
    
    /* strcat(startstr,"typedef struct {void *p, *sp, *ep;} ptr_t;\n");
    strcat(startstr,"typedef struct { ptr_t _ptr; int _cnt; ptr_t _base;\\\n");
    strcat(startstr,"unsigned short _flag; short _fd; unsigned long _pid;\\\n");
    strcat(startstr,"unsigned  _bsize; ptr_t _tmpnam; char _sbuf[4];\\\n");
    strcat(startstr,"unsigned _entry; }FILE;\n");

    strcat(startstr,"int fgetc(FILE * fp);\n");
    strcat(startstr,"int fputc(int c, FILE * fp);\n");
    strcat(startstr,"char * fgets(char *s, int n, FILE *fp);\n");
    strcat(startstr,"int fputs(const char *s, FILE *fp);\n");  */

    #endif
    EiC_startEiC(argc, argv);
    return 0;
}

