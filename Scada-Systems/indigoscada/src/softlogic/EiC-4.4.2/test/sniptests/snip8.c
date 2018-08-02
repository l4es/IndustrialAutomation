#include <stdlib.h>

int main(void)
{
    system("eic -DTEST bincomp.c bincomp.c bincomp.c");
    system("eic -DTEST head.c head.c 7");
    system("eic -DTEST tiresize.c 265 75 16"); 
    system("eic -DTEST strupr.c hello world");
    system("eic -DTEST skiplist.c ");
    system("eic -DTEST remtab.c  < remtab.c");
    system("eic -DTEST spigot.c 13");
    system("eic -DTEST rand1.c");
    system("eic -DTEST regit.c DougFunny");

    return 0;
}

#ifdef EiCTeStS
main();
#endif
