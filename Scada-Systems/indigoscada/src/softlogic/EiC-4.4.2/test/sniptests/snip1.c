#include <stdlib.h>

int main(void)
{
    system("eic -DTEST arccrc16.c arccrc16.c");
    system("eic -DTEST bcdl.c");
    system("eic -DTEST bcdd.c");
    system("eic -DTEST bitstrng.c");
    system("eic -DTEST bodymass.c  67 175");
    system("eic -DTEST ccard.c 38437534974933");
    
    return 0;
}

#ifdef EiCTeStS
main();
#endif
