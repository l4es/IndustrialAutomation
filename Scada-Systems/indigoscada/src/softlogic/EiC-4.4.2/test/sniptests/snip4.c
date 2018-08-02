#include <stdlib.h>

int main(void)
{
    system("eic -DTEST jdn_l.c 13 11 57");
    system("eic -DTEST l2roman.c  1345");
    system("eic -DTEST logscale.c");
    system("eic -DTEST match.c ell hello");
    system("eic -DTEST mathstat.c 12 13 14 15");
    system("eic -DTEST maxline.c helloworldhow");
    system("eic -DTEST memmem.c");
    system("eic -DTEST moon_age.c 11 13 57");
    system("eic -DTEST parity.c 13456");
    system("eic -DTEST parstime.c 12:30:30");

    return 0;    
}

#ifdef EiCTeStS
main();
#endif
