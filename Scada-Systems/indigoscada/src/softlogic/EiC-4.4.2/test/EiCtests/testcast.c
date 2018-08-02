#include <assert.h>

void T1(void)
{
    int i = 10;
    unsigned ui = 20;
    long l = 50;
    unsigned long  ul = 60;
    long long ll = 70;
    int *p;

    p = (int *)i;
    assert((int)p == i);

    p = (int *)ui;
    assert((int)p == ui);

    p = (int *)l;
    assert((int)p == l);

    p = (int *)ul;
    assert((int)p == ul);

    p = (int *)ll;
    assert((int)p == ll);


}

int main()
{

    T1();
    return 0;
}


#ifdef EiCTeStS
main();
#endif










