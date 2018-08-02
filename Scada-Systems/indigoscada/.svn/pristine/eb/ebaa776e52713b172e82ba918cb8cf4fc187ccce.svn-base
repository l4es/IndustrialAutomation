#ifndef PARSERH_
#define PARSERH_

/* parser.c
------------*/
void EiC_initparser(void);
void EiC_updateLocals(void);
void EiC_parse(environ_t * env);
void EiC_stmt(token_t * e1);
void EiC_assign_expr(token_t * e1);
void cond_expr(token_t * e1);
int EiC_GI(token_t * e1);

void EiCp_freeLabels(Label_t *lab);
Label_t * EiCp_addLabel(Label_t *lab, char *name, int loc, int chck);

extern int EiC_S_LEVEL;
extern unsigned int EiC_ASPOT;

#endif
