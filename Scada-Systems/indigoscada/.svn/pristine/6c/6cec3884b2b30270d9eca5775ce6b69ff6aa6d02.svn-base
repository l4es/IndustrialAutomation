#include "testMemErr1.aux"

void g()
{
    int a[5];
    f();   /* call foo in file testMemErr1.aux*/
    a[5];  /* access error */
}

int main()
{
    g();
    return 0;
}


#ifdef EiCTeStS
main();
#endif
