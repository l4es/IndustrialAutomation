#include <stdlib.h>
int main()
{
    system("eic -sA < testMemErr2.aux");
    system("eic -sA < testMemErr2b.aux");
    system("eic -sA < testMemErr2c.aux");
    return 0;
}

#ifdef EiCTeStS
main();
#endif
