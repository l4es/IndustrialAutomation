#include <assert.h>
#include <limits.h>
#include <float.h>


const char cmax = CHAR_MAX;
const char cmin = CHAR_MIN;

const unsigned char ucmax = UCHAR_MAX;
const unsigned char ucmin = 0;

const short smax =  SHRT_MAX;
const short smin =  SHRT_MIN;
const unsigned short usmax = USHRT_MAX;
const unsigned short usmin = 0;

const int imax = INT_MAX;
const int imin = INT_MIN;
const unsigned int uimax = UINT_MAX;
const unsigned int uimin = 0;

const long lmax = LONG_MAX;
const long lmin = LONG_MIN;
const long ulmax = ULONG_MAX;
const long ulmin = 0;

const float fmax = FLT_MAX;
const float fmin = FLT_MIN;
const double dmax = DBL_MAX;
const double dmin = DBL_MIN;


void T1()
{
  assert(cmax == CHAR_MAX);
  assert(cmin == CHAR_MIN);
  assert(ucmax == UCHAR_MAX);
  assert(ucmin == 0);

  assert(smax == SHRT_MAX);
  assert(smin == SHRT_MIN);
  assert(usmax == USHRT_MAX);
  assert(usmin == 0);

  assert(imax == INT_MAX);
  assert(imin == INT_MIN);
  assert(uimax == UINT_MAX);
  assert(uimin == 0);

  assert(lmax == LONG_MAX);
  assert(lmin == LONG_MIN);
  assert(ulmax == ULONG_MAX);
  assert(ulmin == 0);

  assert(fmax == FLT_MAX);
  assert(fmin == FLT_MIN);
  assert(dmax == DBL_MAX);
  assert(dmin == DBL_MIN);

}

void T2()
{
  const char cmax = CHAR_MAX;
  const char cmin = CHAR_MIN;

  const unsigned char ucmax = UCHAR_MAX;
  const unsigned char ucmin = 0;

  const short smax =  SHRT_MAX;
  const short smin =  SHRT_MIN;
  const unsigned short usmax = USHRT_MAX;
  const unsigned short usmin = 0;

  const int imax = INT_MAX;
  const int imin = INT_MIN;
  const unsigned int uimax = UINT_MAX;
  const unsigned int uimin = 0;

  const long lmax = LONG_MAX;
  const long lmin = LONG_MIN;
  const long ulmax = ULONG_MAX;
  const long ulmin = 0;

  const float fmax = FLT_MAX;
  const float fmin = FLT_MIN;
  const double dmax = DBL_MAX;
  const double dmin = DBL_MIN;


  assert(cmax == CHAR_MAX);
  assert(cmin == CHAR_MIN);
  assert(ucmax == UCHAR_MAX);
  assert(ucmin == 0);

  assert(smax == SHRT_MAX);
  assert(smin == SHRT_MIN);
  assert(usmax == USHRT_MAX);
  assert(usmin == 0);

  assert(imax == INT_MAX);
  assert(imin == INT_MIN);
  assert(uimax == UINT_MAX);
  assert(uimin == 0);

  assert(lmax == LONG_MAX);
  assert(lmin == LONG_MIN);
  assert(ulmax == ULONG_MAX);
  assert(ulmin == 0);

  assert(fmax == FLT_MAX);
  assert(fmin == FLT_MIN);
  assert(dmax == DBL_MAX);
  assert(dmin == DBL_MIN);

}


const char *p = "123";
const int a[] = {1,2,3};

void T3()
{
  int i;
  for(i =0;i<3;++i) {
    assert(p[i] == '1' + i);
    assert(i+ '1' == p[i]);
    assert(a[i] == 1+i);
    assert(1+i == a[i]);
  }

  for(i=0;i<3;++i) {
    assert(*(p+i) == '1' + i);
    assert('1' + i == *(p+i) );
    assert(1 + i == *(a+i));
  }
}


void T4()
{
  const char *p = "123";
  const int a[] = {1,2,3};

  int i;
  for(i =0;i<3;++i) {
    assert(p[i] == '1' + i);
    assert(i+ '1' == p[i]);
    assert(a[i] == 1+i);
    assert(1+i == a[i]);
  }

  for(i=0;i<3;++i) {
    assert(*(p+i) == '1' + i);
    assert('1' + i == *(p+i) );
    assert(1 + i == *(a+i));
  }
}

void T5()
{
  assert(!cmax == 0);
  assert(!smax == 0);
  assert(!imax == 0);
  assert(!lmax == 0);

}


void T6()
{
  const char cmax = CHAR_MAX;
  const short smax =  SHRT_MAX;
  const int imax = INT_MAX;
  const long lmax = LONG_MAX;

  assert(!cmax == 0);
  assert(!smax == 0);
  assert(!imax == 0);
  assert(!lmax == 0);

}

void T7(const int x)
{
  assert(!x == 0);
}


int main()
{
  T1();
  T2();
  T3();
  T4();
  T5();
  T6();

  T7(!0);

  return 0;
}

#ifdef EiCTeStS
main();
#endif




