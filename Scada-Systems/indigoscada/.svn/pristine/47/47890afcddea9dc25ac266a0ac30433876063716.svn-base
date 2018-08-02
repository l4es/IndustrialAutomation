
/* default pointer type should be safe */
#include <assert.h>


int *pABC;
int * safe qABC;
int * unsafe rABC;
assert(sizeof(pABC) == sizeof(qABC));
assert(sizeof(pABC) > sizeof(rABC));


#pragma push_unsafeptr

int *pBCD;
int * safe qBCD;
int * unsafe rBCD;
assert(sizeof(pBCD) < sizeof(qBCD));
assert(sizeof(pBCD) == sizeof(rBCD));



#include testaryptr.c



#pragma pop_ptr

int *pCDE;
int * safe qCDE;
int * unsafe rCDE;
assert(sizeof(pCDE) == sizeof(qCDE));
assert(sizeof(pCDE) > sizeof(rCDE));


