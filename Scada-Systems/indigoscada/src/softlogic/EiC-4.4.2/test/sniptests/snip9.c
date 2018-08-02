#include <stdlib.h>

int main(void)
{
    system("eic -DTEST rdxcnvrt.c 2013 10 3");    
    system("eic -DTEST strdel.c helloworld pos 2 3");
    system("eic -DTEST strdelch.c eo helloworld");
    system("eic -DTEST c_cmnt.c < c_cmnt.c"); 
    system("eic -DTEST strrev.c DougFunny");
    system("eic -DTEST etphi.c 11"); 

    return 0;
}

#ifdef EiCTeStS
main();
#endif
