#include <stdio.h>
#include <stdlib.h>


#include "tools/nxtString.c"

int main()
{
    FILE *fp;
    char *p;
    
    system("ls *.eic > /tmp/123eicList");

    fp = fopen("/tmp/123eicList","r");
    if(!fp) {
	fprintf(stderr,"failed to open /tmp/123eicList\n");
	exit(0);
    }
	
    while((p=nxtString(fp))) {
	printf("\n running %s\n",p);
	system(p);
    }
    
    fclose(fp);

    return 0;
}


#ifdef EiCTeStS
main();
#endif
