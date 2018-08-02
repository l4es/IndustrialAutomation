#include <stdio.h>
#include <assert.h>

void T1()
{
    int i = 0,cnt = 0;
    float f=0;
    double d=0;

    char *bad = "x";
    char *good ="567";

    while(sscanf(bad,"%d", &i)==1)
	cnt++;
    assert(cnt == 0);
    while(sscanf(bad,"%f", &f)==1)
	cnt++;
    assert(cnt == 0);
    while(sscanf(bad,"%lf", &d)==1)
	cnt++;
    assert(cnt == 0);

    assert(sscanf(bad,"%d",&i) != 1);
    assert(sscanf(good,"%d",&i) == 1);
    assert(i == 567);
       
    assert(sscanf(bad,"%f",&f)  != 1);    
    assert(sscanf(good,"%f",&f) == 1);
    assert(f == 567);

    assert(sscanf(bad,"%lf",&d) != 1);
    assert(sscanf(good,"%lf",&d) == 1);
    assert(d == 567);
}

int main()
{
    T1();
    return 0;
}

#ifdef EiCTeStS
main();
#endif
