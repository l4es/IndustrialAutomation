#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>


int T1_(int flag)
{
    enum {A=1,C,E};
    int a=0,b=1,c=0,d=2,e=0,f=3,g=4;

    switch(flag) {
      case A: a = 1; break;
      case C: c = 1; break;
      case E: e = 1; break;
    }
	
   return a ? b : c ? d : e ? f : g;
}

void T1(void)
{
    assert(T1_(1) == 1);
    assert(T1_(2) == 2);
    assert(T1_(3) == 3);
    assert(T1_(0) == 4);
}    

int T2_(int flag)
{
    enum {A=1,C,E};
    int a=0,b=1,c=0,d=2,e=0,f=3,g=4;

    switch(flag) {
      case A: a = 1; break;
      case C: c = 1; break;
      case E: e = 1; break;
    }
	
   return (a ? b : (c ? d : (e ? f : g)));
}


void T2(void)
{
    assert(T2_(1) == 1);
    assert(T2_(2) == 2);
    assert(T2_(3) == 3);
    assert(T2_(0) == 4);
}    

int T3_(int flag)
{
    enum {A=1,C,E};
    int k = -1;
    int rtn;
    int a=0,b=1,c=0,d=2,e=0,f=3,g=4;

    switch(flag) {
      case A: a = 1; break;
      case C: c = 1; break;
      case E: e = 1; break;
      default:
	k = flag;

    }

    rtn =   (a ?  k = A, b : (c ? k = C, d :  (e ? k = E, f : g)));

    assert(k == flag);
    return rtn;
}


void T3(void)
{
    assert(T3_(1) == 1);
    assert(T3_(2) == 2);
    assert(T3_(3) == 3);
    assert(T3_(0) == 4);
}    


int T4_(int a, int b, int c)
{
    int rtn;
    int k = 0;
    
    rtn  = a ? k  = a, b ? k += 1, c ? k+=1,  3 : 2 : 1 : 0;
    assert(k == a+b+c);
    return rtn;
}

void T4()
{
    assert(T4_(0,0,0) == 0);
    assert(T4_(1,0,0) == 1);
    assert(T4_(1,1,0) == 2);
    assert(T4_(1,1,1) == 3);
}

void T5()
{
    float fv = 0.2;
    double dv = 0.01;
    long lval = 3;

    assert((0.25 ? 1 : 2) == 1);
    assert((fv ? 1 : 2) == 1);
    assert((dv ? 1 : 2) == 1);
    assert((lval ? 1 : 2) == 1);
    
}

int main()
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














