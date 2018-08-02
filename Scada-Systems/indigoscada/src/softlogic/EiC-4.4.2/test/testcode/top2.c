/* Datentypen-Test */
#include <stdio.h>

int main(void)
{
    int a;

    printf ("%d (5)\n", (4,5));

    a = 1;

    printf ("%d (3)\n", a+=1+1);
    printf ("%d (0)\n", a == 3 ? 1 : 0);
    printf ("%d (1)\n", -- a < 3 ? 0 : 1);
    printf ("%d (6)\n", 2*2+2);
    printf ("%d (0)\n", 0 && 1 || 0);
    printf ("%d (1)\n", 1 && 0 || 1 );
    printf ("%d (15)\n", 8 | 7 ^ 7 & 8);
    /* printf ("%d ()\n", ); */

    return 0;
}

#ifdef EiCTeStS
main();
:show main
#endif
