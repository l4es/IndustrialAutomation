
/* datastruct.h
 *
 *	(C) Copyright May  7 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#if !defined(_eic_ptr) && !defined(_EiC)
#define _eic_ptr
typedef struct {void *p, *sp, *ep;} ptr_t;
#endif


#include "stab.h"

#include "eicval.h"


typedef struct {
    int n;
    val_t * val;
}eicstack_t;

typedef struct extern_t {
    char * name;
    type_expr * type;
    int  n;
    unsigned *loc;
    struct extern_t *nxt;
} extern_t;

/* methods for extern_t */
#define crt_extern()    xcalloc(sizeof(extern_t),1)
#define getExtName(x)   ((x)->name)
#define setExtName(x,y) ((x)->name = y)
#define getExtType(x)   ((x)->type)
#define setExtType(x,y) ((x)->type = y) 
#define getExtNext(x)   ((x)->nxt)
#define setExtNext(x,y) ((x)->nxt = y)


typedef struct {
    char cl;           /* closed flag */
    int n;             /* number of members */
    int tsize;         /* total size in bytes of struct */
    int align;         /* alignment of structure */
    type_expr **type;  /* member types */
    char **id;         /* member names */
    int *offset;       /* offsets to members data position*/
    int ntags;         /* number of tags */
    type_expr **tag;   /* tag types */
}struct_t;


typedef struct {
    unsigned short Tok;
    int Tab;		     /* name space table */
    unsigned char Pflag;              /* processed flag */
    unsigned char Sclass;             /* storage class */
    unsigned char Typequal;           /* type qualifier */
    struct symentry_t * Sym;
    code_t Code;
    type_expr * Type;
    val_t Val;
}token_t;

/*****
#define getTokenVal(x)     ((x).Val)
#define setTokenVal(x,y)   ((x).Val = (y))
#define getTokenType(x)    ((x).Type)
#define setTokenType(x,y)  ((x).Type = (y))
#define getTokenCode(x)    ((x).Code)
#define setTokenCode(x,y)  ((x).Code = (y))
#define getTokenSym(x)     ((x).Sym)
#define setTokenSym(x,y)    ((x).Sym = (y))
#define getTokenTypequal(x) ((x).Typequal)
#define setTokenTypequal(x,y) ((x).Typequal = (y))
#define getTokenSclass(x)     ((x).Sclass)
#define setTokenSclass(x,y)   ((x).Sclass = (y))
#define getTokenPflag(x)      ((x).Pflag)
#define setTokenPflag(x,y)    ((x).Pflag = (y))
#define getTokenTab(x)        ((x).Tab)
#define setTokenTab(x,y)      ((x).Tab = (y))
#define getTokenTok(x)        ((x).Tok)
#define setTokenTok(x,y)      ((x).Tok = (y))
********/



/*CUT symEntry*/
typedef struct symentry_t  {
  int tag;                   /* maker */
  unsigned int entry;        /* entry number */
  struct symentry_t *next;   /* link to next symentry */
  char *id;                  /* pointer to identifier string */
  unsigned char sclass;               /* storage class code */
  unsigned char typequal;             /* type qualifier */
  unsigned char level;                /* scope level */
  unsigned char nspace;               /* name space identifier */
  char ass;                  /* used to flag assignment */
  type_expr * type;          /* object type */
  val_t  val;                /* symbol value information */
  char *pname;               /* previous file name */
  char *fname;               /* file name pointer */

#if 0
  int  calls;          /* number of local references made */
  int Nref;
  struct symentry_t **ref;               /* references */
#endif

}symentry_t;
/*END CUT*/

typedef struct {
    stab_t stab;
    
    code_t CODE;
    eicstack_t ARgar;        /* for garbage collection of AR units*/
    unsigned int ARsize,sp;
    AR_t *AR;   		 /* static activation record */
    unsigned int LARsize,lsp;
    AR_t *LAR;
    extern_t *link;
    
}environ_t;

#define getenvcode(env)  ((env)->CODE)

typedef struct {
    int n;                  /* number of enumerators */
    int *eval;              /* array of enumerator values */
    symentry_t **syms;      /* list of symbol tabel entries */
}enum_t;

typedef struct {
    int hsize;
    symentry_t **htab;
}hashtab_t;

#endif






