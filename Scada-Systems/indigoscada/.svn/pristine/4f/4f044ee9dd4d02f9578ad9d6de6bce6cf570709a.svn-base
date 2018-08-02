#include <assert.h>
#include <limits.h>

void T1()
{
  char c = 'a', cmax = CHAR_MAX, cmin =CHAR_MIN;
  int i = 15, imax = INT_MAX, imin = INT_MIN;
  short s = 66, smax = SHRT_MAX, smin = SHRT_MIN;
  long l = 5, lmax = LONG_MAX, lmin = LONG_MIN;
  long long x = 666;
  long long ll;

  ll=c;
  assert(ll == c);
  ll=cmax;
  assert(ll == cmax);
  ll=cmin;
  assert(ll == cmin);
  ll=CHAR_MIN;
  cmin=ll;
  assert(cmin == CHAR_MIN);
  ll=CHAR_MAX;
  cmax == ll;
  assert(cmax == CHAR_MAX);

  ll=s;
  assert(ll == s);
  ll=smax;
  assert(ll == smax);
  ll=smin;
  assert(ll == smin);
  ll=SHRT_MIN;
  smin=ll;
  assert(smin == SHRT_MIN);
  ll=SHRT_MAX;
  smax == ll;
  assert(smax == SHRT_MAX);

  ll=i;
  assert(ll == i);
  ll=imax;
  assert(ll == imax);
  ll=imin;
  assert(ll == imin);
  ll=INT_MIN;
  imin=ll;
  assert(imin == INT_MIN);
  ll=INT_MAX;
  imax == ll;
  assert(imax == INT_MAX);

  ll=l;
  assert(ll == l);
  ll=lmax;
  assert(ll == lmax);
  ll=lmin;
  assert(ll == lmin);
  ll=LONG_MIN;
  lmin=ll;
  assert(lmin == LONG_MIN);
  ll=LONG_MAX;
  lmax == ll;
  assert(lmax == LONG_MAX);

  ll=x;
  assert(ll == x);

  ll = 103;
  c = ll;
  assert(c == 103);
  s = ll;
  assert(s == 103);
  i = ll;
  assert(i == 103);
  l = ll;
  assert(l == 103);

  assert(l == 2 * ll - c);
  assert(s == 2 * ll - i);

  ll = LONG_MAX;
  assert(ll >= INT_MAX);
  assert(ll >= SHRT_MAX);
  assert(ll >= CHAR_MAX);

  ll = LONG_MIN;
  assert(ll <= INT_MIN);
  assert(ll <= SHRT_MIN);
  assert(ll <= CHAR_MIN);

}

void T2()
{
  unsigned char c = 'a', cmax = UCHAR_MAX;
  unsigned int i = 15, imax = UINT_MAX;
  unsigned short s = 66, smax = USHRT_MAX;
  unsigned long l = 5, lmax = ULONG_MAX;
  long long ll;

  ll=c;
  assert(ll == c);
  ll=cmax;
  assert(ll == cmax);
  ll=CHAR_MAX;
  cmax == ll;
  assert(cmax == UCHAR_MAX);

  ll=s;
  assert(ll == s);
  ll=smax;
  assert(ll == smax);
  ll=SHRT_MAX;
  smax == ll;
  assert(smax == USHRT_MAX);

  ll=i;
  assert(ll == i);
  ll=imax;
  assert(ll == imax);
  ll=INT_MAX;
  imax == ll;
  assert(imax == UINT_MAX);

  ll=l;
  assert(ll == l);
  ll=lmax;
  assert(ll == lmax);
  ll=ULONG_MAX;
  lmax == ll;
  assert(lmax == ULONG_MAX);

  ll = 103;
  c = ll;
  assert(c == 103);
  s = ll;
  assert(s == 103);
  i = ll;
  assert(i == 103);
  l = ll;
  assert(l == 103);

  assert(l == 2 * ll - c);
  assert(s == 2 * ll - i);

  ll = LONG_MAX;
  assert(ll >= INT_MAX);
  assert(ll >= SHRT_MAX);
  assert(ll >= CHAR_MAX);

}

void T3()
{
  float f = 2;
  double d = 15.5;
  long long ll;

  ll = f;
  assert(f == ll);
  ll = d;
  assert((long long)d == ll);
}


void T4()
{
  struct { char a; long long b; int c; } s = {'a', 'b', 'c'};
  struct { char a; long long b; int c; } S[2] = {{'a', 'b', 'c'},
						 {'d', 'e', 'f'}};
  assert(s.a == 'a');
  assert(s.b == 'b');
  assert(s.c == 'c');

  assert(S[0].a == 'a');
  assert(S[0].b == 'b');
  assert(S[0].c == 'c');

  assert(S[1].a == 'd');
  assert(S[1].b == 'e');
  assert(S[1].c == 'f');

}


void T5()
{
  long long a[2] = {1,2};

  assert(a[0] == 1);
  assert(a[1] == 2);

  assert(a[1] > a[0]);
  if(a[1] > a[0]) a[1]--;

  assert(a[0] == a[1]);
}

void T6()
{
  /* some basic arithmetic checks */

  long long a = 10;
  assert(a * 2 == 20);

  assert(a+a == 20);
  assert(a/10 == 1);
  assert(a-11 == -1);

  assert(a<<1 == 20);
  assert(a>>1 == 5);

  assert(5%a == 5);
  assert(10%a == 0);

}


long long a[2] = {1,2};

int main()
{

  assert(sizeof(long long) >= sizeof(long));
  
  if(a[0]) a[0] --;
  assert(a[0] == 0);
  if(a[0] == 0) a[0] ++;
  assert(a[0] == 1);



  T1();
  T2();
  T3();
  T4();
  T5();
  T6();
  return 0;
}

#ifdef EiCTeStS
main();
#endif




