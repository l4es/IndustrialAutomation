#include <assert.h>

int max(int a, int b)
{
    if(a > b)
	return a;
    else
	return b;
}

void T1()
{
    assert(5 == max(2,5));
}

int EiC_testsw(int i)
{
    int j;
    switch(i) {
      case 1: j = 1; break;
      case 2: j = 2; break;
      case 3: j = 3; break;
      case 4: j = 4; break;
      case 5:
      case 6:
      case 7:
	j = 7; break;
      default: j = 10;
    }
    return j;
}
int a[] = {10, 1, 2, 3, 4, 7, 7, 7, 10,10};
int b[] = {10, 1, 0, 3, 0, 5, 7, 7, 10,10};
void T2()
{
    int i,j;
    /* for if break test */
    for(i=0;i<10;++i)
	if(i>5)
	    break;
    assert(i == 6);
    /* for if continue test */
    for(i=0;i<10;++i) {
	if(i>5)
	    continue;
	j = i;
    }
    assert(i == 10 && j == 5);
    /* switch case default test */
    for(i=0;i<10;++i)      
	assert(EiC_testsw(i) == a[i]);
    /* switch break continue default test*/
    for(i=0;i<10;++i) {
	switch(i) {
	  case 3: j = 3; break;
	  case 2: j = 2; continue;
	  default: j = 10; break;
	  case 1: j = 1; break;
	  case 5: j = 5; break;
	  case 4: j = 4; continue;
	  case 6:
	  case 7:
	    j = 7; break;
	}
	assert(j==b[i]);
    }
    
}

int loop100()
{
    static int i=0;
    if(i++ < 100)
	return 0;
    else
	i = 0;
    return 1;
}

void T3()
{				/* test break from infinite loops */
    int c = 0;
    while(1) {
	if(loop100())
	    break;
	c++;
    }
    assert(c == 100);
    c = 0;
    for(;;) {
	if(loop100())
	    break;
	c++;
    }
    assert(c == 100);
    c=0;
    do
	if(loop100())
	    break;
	else
	    c++;
    while(1);
    assert(c == 100);
}

void T4(void)
{
    int x = 10;
    int i = 0;
    int k = 0;
    do {
	i++;
	if(i > x)
	    continue;
	k++;
    }while(i < x + 10);
    assert(k == x);
    assert(i == x + 10);
}

#define  INT    0x7FFF
#define  UINT   0x8000
#define  LONG   0x7FFFFFFF
#define  ULONG  0x80000000

unsigned long T5_(unsigned long F)
{
    switch(F) {
      case INT:  return INT;
      case UINT: return UINT;
      case LONG: return LONG;
      case ULONG:return ULONG;
      default:
	return 0;
    }
}

void T5(void)
{
    assert(T5_(INT) == INT);
    assert(T5_(UINT) == UINT);
    assert(T5_(LONG) == LONG);
    assert(T5_(ULONG) == ULONG);
}

int main(void)
{
    T1();
    T2();
    T3();
    T4();
    T5();
    return 0;
}

#ifdef EiCTeStS
main();
#endif



