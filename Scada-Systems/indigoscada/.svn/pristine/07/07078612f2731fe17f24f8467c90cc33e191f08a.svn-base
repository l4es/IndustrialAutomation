#include <assert.h>

void T1()
{
	int i;
	i = 1;
	assert((i<<2) == 4);
	assert((30%40) == 30);
}

void T2()
{
    long int left=30, right=30;

    assert((left | right) == 30);
    assert((left & right) == 30);
    assert((left ^ right) == 0);
    assert((left < right) == 0);
    assert((left <= right) == 1);
    assert((left > right) == 0);
    assert((left >= right) == 1);
    assert((left == right) == 1);
    assert((left != right) == 0);
    assert((left || right) == 1);
    assert((left && right) == 1);


    assert((left%40) == 30);

    assert((left | 30) == 30);
    assert((left & 30) == 30);
    assert((left ^ 30) == 0);
    assert((left < 30) == 0);
    assert((left <= 30) == 1);
    assert((left > 30) == 0);
    assert((left >= 30) == 1);
    assert((left == 30) == 1);
    assert((left != 30) == 0);
    assert((left || 30) == 1);
    assert((left && 30) == 1);


    assert((40%right) == 10);

    assert((30 | right) == 30);
    assert((30 & right) == 30);
    assert((30 ^ right) == 0);
    assert((30 < right) == 0);
    assert((30 <= right) == 1);
    assert((30 > right) == 0);
    assert((30 >= right) == 1);
    assert((30 == right) == 1);
    assert((30 != right) == 0);
    assert((30 || right) == 1);
    assert((30 && right) == 1);
}

void T3()
{
     float left=30, right=30;
    
    assert((left < right) == 0);
    assert((left <= right) == 1);
    assert((left > right) == 0);
    assert((left >= right) == 1);
    assert((left == right) == 1);
    assert((left != right) == 0);
    assert((left || right) == 1);
    assert((left && right) == 1);

    assert((left < 30) == 0);
    assert((left <= 30) == 1);
    assert((left > 30) == 0);
    assert((left >= 30) == 1);
    assert((left == 30) == 1);
    assert((left != 30) == 0);
    assert((left || 30) == 1);
    assert((left && 30) == 1);

    assert((30 < right) == 0);
    assert((30 <= right) == 1);
    assert((30 > right) == 0);
    assert((30 >= right) == 1);
    assert((30 == right) == 1);
    assert((30 != right) == 0);
    assert((30 || right) == 1);
    assert((30 && right) == 1);
}

void T4()
{
    int a,c;
    float fa;
    
    a = 10;
    fa = 20;
    assert((a && fa) == 1);
    a = 0;
    assert((a && fa) == 0);
    fa = 0;
    assert((a && fa) == 0);
    a = 10;
    assert((a && fa) == 0);
    
    c = a && fa;
    assert(c == 0);

    fa = 10;
    c = a && fa;
    assert(c == 1);
    
    assert((a || fa) == 1);
    a = 0;
    assert((a || fa) == 1);
    fa = 0;
    assert((a || fa) == 0);
    a = 10;
    assert((a || fa) == 1);
    
    c = a || fa;
    assert( c == 1);
    a = 0; fa = 0;
    c = a || fa;
    assert(c == 0);

    c = 10; a = 10; fa = 10;

    assert((a && (c || fa)) == 1);
    fa = 0;
    assert((a && (c || fa)) == 1);
    fa = 10;
    c = 0;
    assert((a && (c || fa)) == 1);
    fa = 0;
    assert((a && (c || fa)) == 0);
    
    fa = 10; a = 0;
    assert((a == 0 && fa) == 1);

}

int main(void)
{
    T1();
    T2();
    T3();
    T4();
    return 0;
}


#ifdef EiCTeStS
main();
#endif

