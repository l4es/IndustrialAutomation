/* symbol.h
 *
 *	(C) Copyright May  7 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef HSIZE
#define HSIZE 101
#endif
extern symentry_t *EiC_HTAB[];

int EiC_iskeyword(keyword_t *keywords,char*id,int n);
void EiC_eicpush(eicstack_t *s, val_t v);
int EiC_eicpop(eicstack_t *s, val_t *pop);
int init_hashtab(int size);
int hashsmc(char * s,int mod);
symentry_t * EiC_lookup(char nspace,char *id);
void free_sym(symentry_t *sym);
void EiC_remlevel(int level);
void EiC_lut_CleanUp(size_t bot);
void EiC_addoffsettolevel(char nspace,int level,int off);
void EiC_marktype(type_expr *type, char mark);
void EiC_marksyms(char mark);
void EiC_remsym(symentry_t *sym);
symentry_t * EiC_insertLUT(char nspace,char *id,int type);
char * EiC_strsave(char *s);
void EiC_newsymtype(symentry_t *sym, type_expr *t);
void newsymARval(token_t *e1, val_t *v);
int nextstackitem(int level);
int EiC_stackit(symentry_t * sym,int level);
void EiC_inittoken(token_t * e1);
void EiC_freetoken(token_t * e1);

void initcode(code_t * code);
void EiC_killcode(symentry_t *sym);
void EiC_freecode(code_t * code);
void EiC_generate(code_t * code, int opcode,val_t *val,int ext);
void copycode(code_t * c1, code_t * c2);
void EiC_concode(code_t * c1, code_t * c2);
void EiC_contoken(token_t * e1, token_t * e2);
void EiC_swaptokens(token_t *e1, token_t * e2);

#define setCodeLineNo(C,idx,No); ((C)->inst[idx].line = No)

size_t EiC_lut_NextEntryNum(void);






