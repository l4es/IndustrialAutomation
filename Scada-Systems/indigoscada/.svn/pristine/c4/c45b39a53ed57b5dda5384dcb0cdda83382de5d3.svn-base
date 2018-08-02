/* optomizer.c
 *
 *	(C) Copyright Jul 27 1996, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MachSet.h"
#include "global.h"
#include "typesets.h"
#include "error.h"

#define isgoto(C,i)  (opcode(C,i) >= jmpu && opcode(C,i) <= jmpTptr)

typedef struct {
    int ninst;        /* # of instructions */
    int leader;       /* block leader */
    int nb;           /* number of braches leading from block */
    int *branch;      /* branch leaders */
}block_t;

#define crt_block() (block_t*)calloc(sizeof(block_t),1)

#define addbranch(x,y,z)    do{\
				    x = (int*)realloc(x,(y+1)*sizeof(int));\
				    x[y++] = z;\
			    }while(0)

static void freeblock(block_t * b, int nb)
{
    int i;
    for(i =0;i<nb;++i)
	if(b[i].branch)
	    free(b[i].branch);
    free(b);
}

static block_t  initblock(code_t *c, int leader, int *visit)
{
    int i;
    block_t  b;
    b.leader = leader;
    b.branch = NULL;
    b.nb = 0;
    b.ninst = 0;

    for(i=leader;i<nextinst(c);++i) {
	visit[i]++;
	b.ninst++;
	if(opcode(c,i) == eicreturn)
	    break;
	else if(isgoto(c,i)) {
	    /*printf("[%d:%d]\n",i,i+ivalcode(c,i));*/
	    addbranch(b.branch,b.nb,i+ivalcode(c,i));
	    if(opcode(c,i) != jmpu) {
		/*printf("[%d:%d]\n",i,i+1);*/
		addbranch(b.branch,b.nb,i+1);
	    }
	    break;
	} else if(opcode(c,i) == jmptab) {
	    struct {int n;val_t *loc;} *p;
	    int j;
	    p = pvalcode(c,i);
	    addbranch(b.branch,b.nb,i + p->loc[0].ival); 
	    for(j =1;j<p->n;j+=2)
		addbranch(b.branch,b.nb,i + p->loc[j+1].ival);
	    break;
	}
    }
    if(leader == nextinst(c))
	visit[leader]++;
    return b;
}


void  EiC_peephole(code_t *C, int *visit)
{
    int  i, n,j;
    n = nextinst(C);
    for(i = 0; i < n; ++i)
	if(isgoto(C,i)) {
	    j = i + ivalcode(C,i);
	    if(j >= n)
		continue;
	    if(opcode(C,j) == jmpu || opcode(C,i) == opcode(C,j)) {
		/*visit[i+ivalcode(C,i)]--;*/
		ivalcode(C,i) += ivalcode(C,j);

	    }
	}
}

int EiC_analyseCode(code_t *C)
{
    /* returns the index to the last visited instruction */
    block_t *block = NULL;

    int nb = 0;
    int  i,j,rtn;
    int *visit;

    visit = calloc(sizeof(*visit),nextinst(C)+1);
    block = realloc(block,(nb+1)*sizeof(*block));
    block[nb++] = initblock(C,0,visit);

    for(i=0;i<nb;++i) {
	for(j=0;j<block[i].nb;++j) {
	    if(!visit[block[i].branch[j]]) {
		block = realloc(block,(nb+1)*sizeof(*block));
		block[nb++] = initblock(C,block[i].branch[j],visit);
	    } else
		visit[block[i].branch[j]]++;
	}
    }
	
    rtn = 0;
    for(i=0;i<=nextinst(C);)
	if(visit[i]) 
	    rtn = i++;
	else if(i < nextinst(C) && instline(C,i)) {
	    EiC_warningerror("Unreachable code at line %d",instline(C,i));	    
	    for(;i<nextinst(C) && !visit[i];i++)
		;
	} else
	    i++;

    EiC_peephole(C,visit);

/*******
    for(i=0;i<nextinst(C);++i)
	if(!visit[i])
	setopcode(C,i,empty);
*******/
    
    freeblock(block,nb);
    free(visit);

    
    return rtn;
}


int EiC_checkPeepHole(token_t *e1,int op)
{
    /*
     * A simple arithmetic optimizer:
     * do not add or subtract 0
     * and do not multiply  or divide by 1
     */
    int v = 0;
    if (isconst(e1->Type)) {
	if (op == '+' || op == '-')
	    switch (EiC_gettype(e1->Type)) {
	      CASE_INT: CASE_UINT:v = e1->Val.ival == 0;
		break;
	      CASE_LONG: CASE_ULONG:v = e1->Val.lval == 0;
		break;
	      CASE_FLOAT:v = e1->Val.dval == 0;
		break;
		}
	else if (op == '*')
	    switch (EiC_gettype(e1->Type)) {
	      CASE_INT: CASE_UINT:v = e1->Val.ival == 1;
		break;
	      CASE_LONG: CASE_ULONG:v = e1->Val.lval == 1;
		break;
	      CASE_FLOAT:v = e1->Val.dval == 1;
		break;
	    }
	}
    return v;
}



























