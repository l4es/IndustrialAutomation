#include <stdlib.h>

int main(void)
{
    system("eic -DTEST crc_32.c"); 
    system("eic -DTEST ansiflen.c ansiflen.c"); 
    system("eic -DTEST bitcnt_1.c 1313131212"); 
    system("eic -DTEST bitcnt_2.c 1313131212"); 
    system("eic -DTEST bitcnt_3.c 1313131212"); 
    system("eic -DTEST bitcnt_4.c 1313131212"); 
    system("eic -DTEST bstr_i.c  100110101111"); 

    return 0;
}

#ifdef EiCTeStS
main();
#endif
