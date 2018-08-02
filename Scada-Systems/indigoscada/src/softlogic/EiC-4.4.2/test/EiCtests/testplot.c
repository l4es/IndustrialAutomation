#include <stdio.h>
#include <stdlib.h>


float randg(float mean, float sigma)
{
    unsigned i;
    float sum;
    for (sum = 0, i = 0; i < 12; ++i) {
	sum += (double) rand() / (double) RAND_MAX;
    }
    return (sum - 6 ) * sigma + mean;
}

int *shell(int *y, int n)
{
    int i,j, gap, hy;
    for(gap = n >> 1; gap > 0;  gap >>= 1)
	for(i=gap;i<n;++i)
	    for(j=i-gap;j>=0 && y[j]>y[j+gap]; j-= gap) {
	        hy = y[j];
		y[j] = y[j+gap];
		y[j+gap] = hy;
	    }
    return y;
}

void plothist(int *x, int m)
{
  unsigned i, j;
  for (i = 0; i < m; ++i, ++x) {
    printf("%3d: ", i);
    for (j = 0; j < *x; ++j)
      printf("*");
    printf("\n");
  }
}


void testplot(float std, int x)
{
  int i, idx, m, ary[24];
  m = sizeof(ary)/sizeof(int);
  
  for(i=0;i<m;i++)
    ary[i] = 0;
    

  for(i=0;i<x;++i)  {
    idx =  (int)(randg(m>>1,std)+0.5);
    if(idx >= m)
      idx = m - 1;
    else if(idx < 0)
      idx = 0;
    ary[idx]++;
  }
  plothist(ary,m);
}


int main()
{
    testplot(3,300);
    return 0;
}

#ifdef EiCTeStS
main();
#endif








