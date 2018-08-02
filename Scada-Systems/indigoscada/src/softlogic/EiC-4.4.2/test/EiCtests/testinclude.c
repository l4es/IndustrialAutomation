#include <assert.h>
struct Dvector { double x, y; };
struct Dvector { double x, y; };

#undef EiCTeStS
#include testplot.c
    :rm main
#include testarray.c
    :rm main
#include teststru.c
    :rm main

/* EiC should allow this */

#include teststru.c
    :rm main
#include testplot.c
    :rm main
#include testarray.c
    :rm main

struct Dvector { double x, y; };
struct Dvector { double x, y; };


/* a bit of torture for EiC */

#include <stdlib.h>

#undef _STDLIBH

#include <stdlib.h>
#include <assert.h>

int main()
{
    assert(atoi("22") == 22);
    assert(atof("1.5") == 1.5);
    srand(100);
    assert(rand() == 12662);
    return 0;
}


main();








