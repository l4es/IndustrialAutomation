#include <errno.h>
#include <assert.h>

int main(void)
{
    assert(errno == 0);
    errno = ERANGE;
    assert(errno == ERANGE);
    errno = 0;
    assert(errno == 0);
    return 0;
}


#ifdef EiCTeStS
main();
#endif
