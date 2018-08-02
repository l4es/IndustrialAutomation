#include <assert.h>
int i = 5;

void T1_()
{
    extern int i,j;
    i = 7;
    j = 10;
}

int j = 3;

void T1(void)
{
    assert(i == 5);
    assert(j == 3);
    T1_();
    assert(i == 7);
    assert(j == 10);
}

#define N 4
double x[N];


void T2(void)
{
    void T2_(void);
    T2_();
    assert(x[0] == 10);
    assert(x[1] == 20);
    assert(x[2] == 30);
    assert(x[3] == 40);
}
 
void T2_(void)
{
    extern double x[N];
    x[0] = 10;
    x[1] = 20;
    x[2] = 30;
    x[3] = 40;

    assert(x[0] == 10);
    assert(x[1] == 20);
    assert(x[2] == 30);
    assert(x[3] == 40);
}

int main(void)
{
    T1();
    T2();
    return 0;
}

#ifdef EiCTeStS
main();
#endif







