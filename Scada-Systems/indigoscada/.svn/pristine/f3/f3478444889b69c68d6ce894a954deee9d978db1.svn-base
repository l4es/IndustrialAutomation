#include <stdio.h>

int y;

int *T(void)
{
    return &y;
}


int *T1(int y)
{
    return &y;  /* dangling pointer */
}


int *T2(void)
{
    int f;
    return &f; /* dangling pointer */
}

int *T3(void)
{
    static int x;
    return &x;
}

int *T4(int *t)
{
    return t;
}

int *T5(int a[])
{
    return a;
}

int main(void)
{
    printf("This file should generate 2 dangling pointer warnings\n");
    return 0;
}

#ifdef EiCTeStS
main();
#endif
