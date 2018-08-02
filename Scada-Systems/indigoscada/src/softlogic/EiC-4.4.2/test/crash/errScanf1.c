#include <stdio.h>
#include <string.h>
#include <assert.h>

void main()
{
    char * str = "100ergs";
    float f; 
    char outstr[100];
    sscanf(str,"%f %s",&f,&outstr);
    assert(f==100);
    assert(strcmp(outstr,"ergs") == 0);
/*    printf("%g %s",f,outstr);*/
}

#ifdef _EiC
main();
#endif

