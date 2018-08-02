#include <stdio.h>
#include <assert.h>

void T1()
{
    typedef struct {int x,y;} xy_t;
    xy_t a,b, xya[5];
    int x,y,z,i;
    char *p, ca[20];

    x = y = z = 5;
    assert(x == y && y == z && z == 5);

    x += y += z;

    assert(y == 10 && x == 15);

    for(i=0;i<sizeof(ca);++i)
	ca[i] = i;
    
    assert(ca[0] == 0 && ca[1] == 1 && ca[2] == 2);
    
    p = ca;
    
    *p++ += 1;
    assert(*p == ca[0] && ca[1] == ca[0]);

    *++p += 2;
    assert(*p == ca[4] && ca[2] == ca[4]);

    b.x = 5; b.y = 10;
    xya[2] = a = b;
    assert(xya[2].x == b.x);
    assert(xya[2].y == b.y);
}
    

void T2()
{
    struct {int a,b;} ab1, ab2, ab3, aba[4], *p;

    ab3.a = 22;
    ab3.b = 33;

    p = &aba[2];
    aba[2] = aba[3] = ab1 = ab2 = ab3;

    assert(ab1.a == ab3.a);
    assert(aba[2].b == aba[3].b && aba[3].b == ab3.b);
    assert(p == &aba[2] && p->a == aba[2].a);
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



