#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <math.h>

int main()
{
    double radlog;
    int digs;
    static int radix = FLT_RADIX;
    
    assert(10 <= DBL_DIG && FLT_DIG <= DBL_DIG);
    assert(DBL_EPSILON <= 1e-9);
    assert(1e37 <= DBL_MAX);
    radlog = log10(radix);
    digs = (DBL_MANT_DIG -1)* radlog;
    assert(digs <= DBL_DIG && DBL_DIG <= digs+1);
    assert(37 <= DBL_MAX_10_EXP);
    return 0;
}


#ifdef EiCTeStS
main();
#endif
