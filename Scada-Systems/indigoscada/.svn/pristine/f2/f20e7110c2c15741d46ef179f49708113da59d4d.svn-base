#include <assert.h>

void T1() 
{ 
    int x;

x :    
    x = 20;

    goto x;    // infinite loop

    x = 5;    // unreachable code
}

void T2()
{
    int x = 0;

x:
    x += 2;

    if(x < 10)
	goto x;

    assert(x >= 10);
}


void T3() 
{
    int f = 30;

    goto end;

    f  *= 20;  // unreachable code

end:;

    assert(f == 30);
}

	
void T4() 
{
    /* what a mess you can make with gotos!!! */
    int i = 1;
    int l1 = 0, l2 = 0, l3 = 0;

goto start;

l1: l1 = 1; i = 2; goto start;
l2: l2 = 2; i = 3; goto start;
l3: l3 = 3; i = 4; goto end;

start:

    switch(i) {
	case 1: goto l1; 
	case 2: goto l2; 
	case 3: goto l3; 
    }

end:
     assert(i == 4 && l1 == 1 && l2 == 2 && l3 == 3);
}

int main()
{

    T2();
    T3();
    T4();

    return 0;
}

#ifdef EiCTeStS
main();
#endif







