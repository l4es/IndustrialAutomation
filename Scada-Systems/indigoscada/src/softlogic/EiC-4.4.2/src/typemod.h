/* typemod.h
 *
 *	(C) Copyright Dec 14 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */


#ifndef TYPEMODH_
#define TYPEMODH_


            /*CUT typeExprStruct*/
typedef struct type_expr {
    unsigned short obj;
    unsigned char alias;
    unsigned char base;
    unsigned char qual;
    union {
	void * inf;
	size_t sz;
    }u;
    struct type_expr * nxt;
} type_expr;
            /*END CUT*/

/* macro methods */
#define setBase(X)  do {type_expr * x = X; if(x) x->base=1;} while 0
#define nextType(x) (x)->nxt
#define setInf(x,y) (x)->u.inf = (y)
#define getNumElems(x)  (x)->u.sz
#define setNumElems(x,y)  (x)->u.sz = (y)

/* handle qualifers */
/* the safe qualifer is coupled to pointer types only */
#define isunsafe(x)           ((x)->qual  & q_unsafe)
#define issafe(x)            !isunsafe(x)
#define setSafe(x)           ((x)->qual &= ~q_unsafe)
#define setUnSafe(x)          ((x)->qual |=  q_unsafe)
#define unsetPtr(x)          ((x)->qual &= ~q_unsafe)

#define isconst(x)          ((x)->qual  & q_const)
#define isconstp(x)         ((x)->qual  & q_constp)
#define setConst(x)         ((x)->qual  |= q_const)
#define setConstp(x)     ((x)->qual  |= q_constp)
#define unsetConst(x)       ((x)->qual &= ~(q_const | q_constp))

#define IsTemp(x)      ((x)->qual & q_temp)
#define SetTemp(x)     ((x)->qual |= q_temp)

/* test 4 lose of const qualifier */ 
#define ConstIntegrity(x,y) \
(!((x)->qual & (q_const | q_constp)) && ((y)->qual & (q_const | q_constp)))

void * EiC_getInf(type_expr *t);

void EiC_setAsBaseType(type_expr * t);
type_expr *EiC_copyBaseType(type_expr * t);


type_expr * EiC_getcontype(type_expr * t1, type_expr * t2);
type_expr * EiC_catTypes(type_expr *t1, type_expr *t2);
int EiC_get_sizeof(type_expr * t);
int EiC_get_align(type_expr * t);
void EiC_exchtype(int obj, type_expr * t);
type_expr * EiC_succType(type_expr *t);
type_expr * EiC_revtype(type_expr *t);
void * EiC_freetype(type_expr *t);
void EiC_setaliases(type_expr *t, char a);
void EiC_set_bastype(unsigned t, type_expr * typ);
char basaliase(type_expr *t);
void * basinf(type_expr *t);
int EiC_bastype(type_expr *t);

int EiC_sametypes(type_expr *t1, type_expr *t2);
type_expr * EiC_addtype(unsigned obj,
			type_expr *t);
type_expr * EiC_transtype(type_expr *t);
type_expr * EiC_copytype(type_expr *t);
int EiC_gettype(type_expr *t);
int compareConst(type_expr *t1, type_expr *t2);
int EiC_compareSafe(type_expr *t1, type_expr *t2);

int EiC_compatibletypes(type_expr *t1, type_expr * t2);


#endif










