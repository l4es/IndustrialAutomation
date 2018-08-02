#include <assert.h>

void T1(void)
{
    int x = 33;
    {
	float x = 1.5;
        {
	    int x = 2;
            {
		float x = 3.5;
		assert(x == 3.5);
	    }
	    assert(x == 2);
	}
	assert(x == 1.5);
    }
    assert(x == 33);
}

void T2(void)
{
    int i;
    int j =1;
    int x[5] = {10,11,12,13,14};
    {
	float x[5] = {20,21,22,23,24};
	int j = 2;
	{
	    int x[5] = {30,31,32,33,34};
	    int j = 3;
	    assert(j==3);
	    for(i=0;i<5;++i)
		assert(x[i] == j*10+i);
	}
	assert(j == 2);
	for(i=0;i<5;++i)
	    assert(x[i] == j*10+i);
    }
    assert(j == 1);
    for(i=0;i<5;++i)
	assert(x[i] == j*10+i);
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












