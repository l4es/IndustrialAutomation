#include <assert.h>

/*
  
[#6] The translation-time conversion of  floating  constants
       should  match  the  execution-time  conversion  of character
       strings by library functions, such as strtod, given matching
       inputs  suitable  for  both  conversions,  the  same  result
       format, and default execution-time rounding.52)
*/

#include <assert.h>

void T1()
{
    
    double d1, d2, d3,d4;
    float f;
    d1 = f = 0.7;
    d2 = (float) 0.7;
    d3 = 0.7F;
    d4 = 0.7f;

    
    /*
       the values assigned to d1, d2, d3 and d4 are required to  have  been
       converted to float.
    */   

    
    assert(d1 == f);     /* EiC fails  here */
    assert(d2 == f);
    assert(d3 == f);
    assert(d4 == f);
    assert(f == 0.7F);
    
}

void T2()
{
    float f;
    double d;

    assert(sizeof(f) == sizeof(0.7F));
    assert(sizeof(d) == sizeof(0.7));
    assert(sizeof(0.7F) <= sizeof(0.7));
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












