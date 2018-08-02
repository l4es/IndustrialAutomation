/* ymem.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xalloc.h"
#include "assertp.h"
#include "global.h"

char XGMARK = 0;

#define freemark -1

/*CUT XALLOC_struct*/
typedef struct {
    size_t nbytes;        /*sizeof allocated area*/
    void * p;             /*pointer to allocated area*/
    char mark;            /*memory mark: used by garbage collector*/
    char * crt_file;      /*Name of file which asked to creat the memory*/
    int crt_lineno;       /*Line no from which the creation call was made*/
    unsigned long alloc_num;        /*allocation entry number*/
}XALLOC;
/*END CUT*/



typedef struct {
    int dbuf_no;
    int top;
    XALLOC *dbuf;
}memtab_t;

#define  MTABSZ  203
memtab_t MTAB[MTABSZ];
#define  BNO(p) ((unsigned long)p % MTABSZ)  
#define  BUFINC  10

#define STDMSG ("line %d in file %s: "\
		"tot_seen  %lu\n",lineno,file,(unsigned long)tot_seen);

/*CUT XALLOC_array*/

unsigned long EiC_tot_memory = 0L;  /*total amount of memory allocated in bytes*/
size_t  EiC_tot_alloc  = 0;         /*total number of current memory allocations*/
size_t  tot_seen = 0;           /*total number of allocations made */
/*END CUT*/

int EiC_getMemMark(unsigned long item)
{
    /* given an item number, return its mark value */
    size_t i,k;
    for(k=0;k<MTABSZ;k++)
	for(i=0;i<MTAB[k].dbuf_no;++i)
	    if(MTAB[k].dbuf[i].alloc_num == item)
		return MTAB[k].dbuf[i].mark;
    return -1;
}

void EiC_freeMemItem(unsigned long item)
{
    size_t i,k;
    for(k=0;k<MTABSZ;k++)
	for(i=0;i<MTAB[k].dbuf_no;++i)
	    if(MTAB[k].dbuf[i].alloc_num == item) {
		xfree(MTAB[k].dbuf[i].p);
		return ;
	    }
}

void EiC_ydumpnonmark(char *outfile, char mark)
{
    size_t i,k;
    for(k=0;k<MTABSZ;k++) 
	for(i=0;i<MTAB[k].dbuf_no;++i)
	    if(MTAB[k].dbuf[i].mark >= 0) {
		if(MTAB[k].dbuf[i].mark != mark)   {
		    printf("item %ld Create line %d file %s nbytes %lu\n",
			   MTAB[k].dbuf[i].alloc_num,
			   MTAB[k].dbuf[i].crt_lineno,
			   MTAB[k].dbuf[i].crt_file,
			   (unsigned long)MTAB[k].dbuf[i].nbytes);
		    MTAB[k].dbuf[i].mark = MEM_LEAK;
		} else
		    MTAB[k].dbuf[i].mark = 0;
	    }
}

size_t EiC_xalloc_NextEntryNum(void)
{
    return tot_seen + 1;
}

void EiC_xalloc_CleanUp(size_t bot, size_t top)
{
    size_t i,k;

    for(k=0;k<MTABSZ;k++)
	for(i=0;i<MTAB[k].dbuf_no;++i)
	    if(MTAB[k].dbuf[i].p)
		if(MTAB[k].dbuf[i].alloc_num >= bot && 
		   MTAB[k].dbuf[i].alloc_num < top  &&
		   MTAB[k].dbuf[i].mark != eicstay)
		    xfree(MTAB[k].dbuf[i].p);
}



void EiC_xfreemark(char mark)
{
    size_t i,k;

    for(k=0;k<MTABSZ;k++)
	for(i=0;i<MTAB[k].dbuf_no;++i)
	    if(MTAB[k].dbuf[i].mark == mark)
		xfree(MTAB[k].dbuf[i].p);
}

int EiC_ymark(char *file,
	  int lineno, void *p, char mark)
{
    int found;
    found = xlookup(p);
    assertp(found < 0,STDMSG);
    MTAB[BNO(p)].dbuf[found].mark = mark;

    return 1;
}

static int install(char *file,
		   int lineno,
		   void *p,
		   size_t nbytes)
{
    int bno;
    unsigned i;
    extern int EiC_memtraceON;


    bno = BNO(p);
    
    for(i=0;i<MTAB[bno].dbuf_no;++i) {  /* search for empty slot */
	if(MTAB[bno].dbuf[i].p == NULL)
	    break;
    }
    
    if(i >= MTAB[bno].top) {
	MTAB[bno].top += BUFINC;
	if(!MTAB[bno].dbuf)
	    MTAB[bno].dbuf = (XALLOC*)(calloc)(sizeof(XALLOC),BUFINC + 1);
	else
	    MTAB[bno].dbuf = (XALLOC*)realloc(MTAB[bno].dbuf,
				   sizeof(XALLOC) * (MTAB[bno].top+1));
    }
    
    assertp(MTAB[bno].dbuf == NULL,("Out of Memory"));
    
    MTAB[bno].dbuf[i].p = p;
    MTAB[bno].dbuf[i].nbytes = nbytes;
    MTAB[bno].dbuf[i].mark = XGMARK;
    MTAB[bno].dbuf[i].crt_file = file;
    MTAB[bno].dbuf[i].crt_lineno = lineno;
    EiC_tot_memory += nbytes;
    EiC_tot_alloc++;
    if(i>=MTAB[bno].dbuf_no)
	MTAB[bno].dbuf_no++;
    
    MTAB[bno].dbuf[i].alloc_num = ++tot_seen;
    if(EiC_memtraceON)
	printf("%lu ",(unsigned long)tot_seen);
    return i;
}

int xlookup(void *p)
{
    unsigned i,bno = BNO(p);
    
    for(i=0; i< MTAB[bno].dbuf_no;i++)
	if(MTAB[bno].dbuf[i].mark >= 0) {
	    if(MTAB[bno].dbuf[i].p == p)
		return i;
	}
    return -1;
}

void * EiC_ymalloc(char *file, int lineno, size_t nbytes)
{
    void * pheap;

    pheap = (malloc)(nbytes);
    assertp(pheap==NULL,STDMSG);
    install(file,lineno,pheap,nbytes);

    return pheap;
}	

void * EiC_ycalloc(char *file,
	       int lineno,
	       size_t nelems,
	       size_t elems)
{
    void * pheap;
    pheap = EiC_ymalloc(file, lineno,nelems * elems);
    if(pheap)
	memset(pheap,0,nelems * elems);
    return pheap;
}

void * EiC_yrealloc(char *file, int lineno, void *oldp, size_t nbytes)
{
    void *newp;
    int found, d;
    
    if(oldp != NULL) {
	found =  xlookup(oldp);
	assertp(found < 0,STDMSG);
    }
    
    newp = realloc(oldp,nbytes);
    
    assertp(nbytes && newp == NULL,("line %d in file %s\n",lineno,file));
    
    if(oldp) {
	int bno = BNO(oldp);
	d = nbytes - MTAB[bno].dbuf[found].nbytes;
	if(bno != BNO(newp)) {

	    int i;
	    
	    MTAB[bno].dbuf[found].p = NULL;
	    MTAB[bno].dbuf[found].mark = freemark;
	    i = install(file,lineno,newp,nbytes);
	    /* retain creation time stamp */
	    MTAB[BNO(newp)].dbuf[i].alloc_num
	    	    = MTAB[bno].dbuf[found].alloc_num;

	} else {
	    MTAB[bno].dbuf[found].p = newp;
	    MTAB[bno].dbuf[found].nbytes = nbytes;
	    MTAB[bno].dbuf[found].crt_file = file;
	    MTAB[bno].dbuf[found].crt_lineno = lineno;
	}
	EiC_tot_memory += d;
    } else
	install(file,lineno,newp,nbytes);
    

    return newp;
}

void EiC_yfree(char *file, int lineno, void * p)
{
    int found,bno = BNO(p);
    found = xlookup(p);
    if(found < 0) {
	/*EiC_warningerror("free non-xalloc ptr: from %s line %d", file, lineno);*/
	(free)(p);
    } else {
	EiC_tot_memory -= MTAB[bno].dbuf[found].nbytes;
	EiC_tot_alloc--;
	(free)(p);
	MTAB[bno].dbuf[found].p = NULL;
	MTAB[bno].dbuf[found].mark = freemark;
    }
}
