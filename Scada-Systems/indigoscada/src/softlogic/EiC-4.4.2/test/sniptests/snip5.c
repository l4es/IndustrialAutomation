#include <stdlib.h>

int main(void)
{
    system("eic -DTEST pbmsrch.c");
    system("eic -DTEST rad2deg.c");
    system("eic -DTEST rg_rand.c");
    system("eic -DTEST rnd_div.c 333 22");
    system("eic -DTEST roman2l.c MCIV");
    system("eic -DTEST ruleline.c");
    system("eic -DTEST soundex.c helloworld");
    system("eic -DTEST stptok.c ItsaniceDay iD");
    system("eic -DTEST stristr.c");
    system("eic -DTEST strrpbrk.c");
    system("eic -DTEST vfname.c");
    system("eic -DTEST w_wrap.c");

    return 0;
}

#ifdef EiCTeStS
main();
#endif
