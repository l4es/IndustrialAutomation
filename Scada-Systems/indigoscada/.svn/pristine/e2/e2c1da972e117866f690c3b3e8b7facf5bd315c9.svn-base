#ifndef EICVALH_
#define EICVALH_

#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif

#if !defined(_eic_ptr)
#define _eic_ptr
typedef struct {void *p, *sp, *ep;} ptr_t;
#endif


#ifndef NO_LONG_LONG
typedef long long eic_llong;
#else
typedef long eic_llong;
#endif


union VaL {
  char cval;   	          /* char value */
  unsigned char ucval;
  short sval;             /* short integer val */
  unsigned short usval;
  int ival;    	          /* integer value */
  unsigned  uival;
  long lval;              /* long integer */
  unsigned long ulval;

  /* long longs are not yet supported by ANSI C*/

  eic_llong llval;        /* long long value */

  float fval;  	         /* float value */
  double dval;           /* double float value */

  ptr_t p;              /* safe pointer */
  void *up;             /* unsafe pointer */

  div_t divval;
  ldiv_t ldivval;
        
  int (*func)(); 	     /* function pointer */
  union VaL (*vfunc)();

  struct symentry_t * sym;/* pointer into symbol table */

#ifndef WIN32
  pid_t pid;              /* process ID value */
#endif
  size_t szval;           /* generic size value */
#ifndef WIN32
  ssize_t sszval;         /* POSIX.1 byte count value */
#endif

#ifndef WIN32
  mode_t mval;            /* mode_t value */
#endif

#ifdef _NETBSD
  long offval;		  /* file offset position */
#else
  off_t offval;           /* file offset position */
#endif
};

typedef union VaL val_t;


typedef struct Label_t {
    char *name;            /* label name */
    int loc;               /* label location */
    struct Label_t *nxt;   /* link to next label in list */
} Label_t;

typedef struct {
    unsigned opcode;
    val_t  val;
    int ext;
    unsigned short line;
}InsT_t;


typedef struct {
    unsigned int nextinst;  /* next instruction */
    unsigned int binst;     /* physical size */
    InsT_t * inst;          /* instructions */
    char * Filename;        /* file with source code */
    Label_t * labels;
    Label_t * gotos;
    void * parent;         /* used for callbacks */
    void * prev;           /* used for chaining during reentry and callbacks*/
}code_t;

/* methods for code */
#define opcode(C,i)   ((C)->inst[i].opcode)
#define setopcode(C,i,y) (opcode(C,i) = y)
#define ivalcode(C,i) ((C)->inst[i].val.ival)
#define pvalcode(C,i) ((C)->inst[i].val.p.p)
#define nextinst(C)   ((C)->nextinst)
#define instline(C,i) ((C)->inst[i].line)
#define codeName(C)   (C)->Filename


#ifndef EICH_
typedef struct AR_t {
    val_t v;
    type_expr * type;
}AR_t;

#else
typedef struct AR_t {
    val_t v;
    void * type;
}AR_t;
#endif

#endif



