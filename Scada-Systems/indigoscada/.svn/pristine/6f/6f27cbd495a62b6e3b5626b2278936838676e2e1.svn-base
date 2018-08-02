/* stringtab.c
 *
 *	(C) Copyright Apr 20 1997, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */



#include <stdio.h>
#include <string.h>
#include "xalloc.h"
#include "stab.h"

/* STRING TABLE ROUTINES
   -------------------*/
char * stab_PopLastString(stab_t *stab)
{
    char * p;
    if(stab->n) {
	stab->n--;
	p = stab->strs[stab->n];
	if(stab->n)
	    stab->strs = xrealloc(stab->strs, sizeof(*stab->strs) * (stab->n));
	else
	    xfree(stab->strs);
    } else
	p = NULL;

    return p;
}

int EiC_stab_FindString(stab_t *stab, char *s)
{
    int i;
    char **strs = stab->strs;
    
    for(i=0;i<stab->n;++i,++strs)
	if(strcmp(*strs,s) == 0)
	    return i;
    return -1;
}
    
int EiC_stab_RemoveString(stab_t *stab, char *s)
{
    int i = EiC_stab_FindString(stab,s);
    if(i >= 0) {
	xfree(stab->strs[i]);
	if(i != stab->n - 1) 
	    memcpy(&stab->strs[i],
		   &stab->strs[i+1],
		   (stab->n - i - 1) * sizeof(char*));
	stab->n--;
	if(stab->n) 
	    stab->strs = xrealloc(stab->strs,
			      sizeof(char*) * stab->n);
	else
	    xfree(stab->strs);
	return 1;
    }

    return -1;
}
	

char * EiC_stab_SaveString(stab_t *stab, char *s)
{
    char *EiC_strsave(char*);
    int i;
    if((i=EiC_stab_FindString(stab,s)) < 0) {
	if(stab->n == 0)
	    stab->strs = xmalloc(sizeof(*stab->strs));
	else
	    stab->strs = xrealloc(stab->strs, sizeof(*stab->strs) * (stab->n+1));
	i = stab->n; 
	stab->strs[i] = EiC_strsave(s);
	stab->n++;
    }
    return stab->strs[i];
	
}

size_t EiC_stab_NextEntryNum(stab_t *stab)
{
    return stab->n;
}

void EiC_stab_CleanUp(stab_t *stab, size_t bot)
{
    while(stab->n> bot)
	xfree(stab_PopLastString(stab));
}

void EiC_stab_Mark(stab_t *stab, char mark)
{
    int i;
    if(stab->n) {
	xmark(stab->strs,mark);
	for(i=0;i<stab->n;++i) 
	    xmark(stab->strs[i],mark);
    }
}

void EiC_stab_ShowStrings(stab_t *stab)
{
    int i;
    for(i=0;i<stab->n;++i)
	printf("%s\n",stab->strs[i]);
}
/*END STRING TABLE ROUTINES*/











