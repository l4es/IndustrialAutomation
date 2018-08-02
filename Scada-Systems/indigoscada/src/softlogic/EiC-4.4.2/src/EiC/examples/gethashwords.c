#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


int hashsmc(char * s,int mod)
{
    register unsigned int h, c;
    h = 0;
    while(  (c = (int) *s++) > 0)
	h =   (h << 1) + c;
    return ( h % mod);
}


char * getnextword(FILE *fp)
{
    static char buff[512];
    int c;
    int i;
    if(!feof(fp)) {
	while(!isalpha(c = fgetc(fp)) && c != EOF)
	    ;
	for(i=0;(isalpha(c) || c == '_') && c != EOF;++i) {
	    buff[i] = c;
	    c = fgetc(fp);
	}
	ungetc(c,fp);
	buff[i] = 0;
	return buff;
    } else
	return NULL;
}
	    
void process(char *fname)
{
    char *w;
    FILE *fp;

    fp = fopen(fname,"r+");
    
    while((w = getnextword(fp)) != NULL) 
	printf("%d:%s\n",hashsmc(w,101),w);

    fclose(fp);
}

int main(int argc, char **argv)
{
    while(--argc > 0) {
        printf("processing %s\n",*++argv);
        process(*argv);
    }
    return 0;
}






