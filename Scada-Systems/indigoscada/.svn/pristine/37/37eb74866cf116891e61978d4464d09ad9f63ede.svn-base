#ifndef FUNCH_
#define FUNCH_

#include "typemod.h"
#include "datastruct.h"

typedef struct {
    type_expr *type;   /* parameter type */
    char *name;        /* parameter name */
    char *comm;        /* parameter comment */
    void *val;         /* parameter default value */
} parm_t;


typedef struct {
    int Num;             /* # of parameters */
    parm_t * parm;

    int sn;            /* # of strings */
    char **strs;       /* string pointers */
    int stn;           /* # of static arrays */
    int *st;           /* pointer to static arrays */
    extern_t *link;    /* pointer to links */
    int ni;            /* # of initialisers */
    void **inzs;       /* initialisers */
    char *comm;        /* function comment */

    code_t *callBack;  /* call Back code */

} func_t;

/** func.c **/
/* methods */
#define getFNp(f)         ((f)->Num)     /* get number of parameters */
#define setFNp(f,i)       ((f)->Num = i)
#define getFPname(f,i)    ((f)->parm[i].name)  /* get parameter name */
#define setFPname(f,i,s)  ((f)->parm[i].name = s) /* set parameter name */
#define getFPty(f,i)      ((f)->parm[i].type) /* get parameter type */
#define setFPty(f,i,s)    ((f)->parm[i].type = s) 

#define getFPcomm(f,i)    ((f)->parm[i].comm)  /* get parameter comment */
#define setFPcomm(f,i,s) ((f)->parm[i].comm = s) /* set parameter comment */ 

#define getFPval(f,i)    ((f)->parm[i].val)  /* get parameter value */
#define setFPval(f,i,s) ((f)->parm[i].val = s) /* set parameter value */ 

#define getFPtyList(f)    ((f)->parm)      /* get  parameter list */
#define setFPtyList(f,s)  ((f)->parm = s)  /* set  parameter list */

#define getFComm(f)      ((f)->comm)
#define setFComm(f,s)    ((f)->comm=s)

#define getFcallBack(f)  ((f)->callBack)
#define setFcallBack(f,s)  ((f)->callBack=s)

void EiC_make_func(token_t * e1);
void EiC_add_func_parm(func_t * f, type_expr ** type, char * name);
void EiC_add_func_str(func_t * f, char *s);
void EiC_add_func_static(func_t * f, int n);
void EiC_add_func_initialiser(func_t * f, void *s);
void EiC_free_func_inf(func_t * f);
void EiC_markFunc(type_expr * type, int mark);
void EiC_freeFuncComments(func_t *f);
int EiC_sameFuncs(type_expr *t1, type_expr *t2);

int EiC_IsVariadic(func_t *f);
int EiC_hasPrototype(func_t *f);
void EiC_swapFPLists(func_t *f1, func_t *f2);
int EiC_gotMissingNames(func_t *f);
 
int EiC_HasHiddenParm(type_expr *ty);

void add_param_initialiser(func_t *f);

void (*EiC_saveComment(void)) (char * s);

#endif

















