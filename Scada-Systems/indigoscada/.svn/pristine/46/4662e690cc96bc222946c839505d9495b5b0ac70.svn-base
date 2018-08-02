#include <assert.h>

int sum(int x, int y)
{
    return x + y;
}


int accum()
{
    int a,b,c,d,e;

    return a=1,b=a+1,c=b+1,d=c+1,e=d+1,e;
}
int accum2()
{
    int a,b,c,d,e;

    return (a=1,b=a+1,c=b+1,d=c+1,e=d+1,e);
}

int array[5] = {0,1,2,3,4,}; 
void T1()
{
    int i,j;
    i = (5,6,7,8,9,j=10,j);
    assert(i == j && j == 10);
    assert(sum(5,(j=2,2*j)) == 9);
    j = array[i=2,j=2,i+j];
    assert(j == 4);
    assert(accum() == 5);
    assert(accum() == accum2());
}

void T2()
{
    assert(2 == ((accum()),2));
    assert(2 == (accum(),2));
}



int main()
{
    T1();
    T2();
    return 0;
}

#ifdef EiCTeStS
main();
#endif


