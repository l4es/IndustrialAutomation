#include <stdio.h>

char * nxtString(FILE *fp)
{
    /*
     * nxtString reads the next string of
     * characters from file fp.
     *
     * A string is defined as a sequence
     * of non-white characters.
     *
     * Returns a static char *
     * on success, otherwise NULL
     *
     * Example:
     *        FILE *fp = fopen("tools/nxtString.c","r");
     *        char *p;
     *        while((p=nxtString(fp)))
     *                 printf("%s,n",p);
     *        fclose(fp);
     */
    
    static char buf[256];

    if(fp && fscanf(fp,"%s",buf) == 1)
	return buf;
    else
	return NULL;
}
	
	

