#include <stdlib.h>
#include <math.h>

int * rselect(int N, int ns, int seed )
{
  /* Returns a  dynamic pointer to  an
     array of `ns' non-repeating
     randomly selected numbers from 0 to `N'.
     The `seed' value is used for
     seeding the random number generator.

     Returns a NULL pointer on error.
     */
  int *select,i,s,k;

  if(ns > N || ns <= 0 || N <= 0)
    return NULL;

  select = calloc(ns,sizeof(int));
  if(!select)
    return NULL;

  srand(seed);

  for(i=0;i<ns;) {
    s = rand()%N;
    for(k=0;k<i;k++) {
      if(select[k] == s)
	break;
    }
    if(k == i)
      select[i++] = s;
  }
  return select;
}




float randg(float mean, float sigma)
{
    unsigned i;
    float sum;
    for (sum = 0, i = 0; i < 12; ++i)
        sum += (float) rand() / (float) RAND_MAX;
    return (sum - 6) * sigma + mean;
}


int *intShell(int *y, int n)
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

float *floatShell(float *y, int n)
{
    int i,j, gap;
    float hy;
    for(gap = n >> 1; gap > 0;  gap >>= 1)
        for(i=gap;i<n;++i)
            for(j=i-gap;j>=0 && y[j]>y[j+gap]; j-= gap) {
                hy = y[j];
                y[j] = y[j+gap];
                y[j+gap] = hy;
            }
    return y;
}

#ifndef deg2rad
#define deg2rad(a)   (0.0174533*(a))
#endif

float * getcurve(float * data, int n)
{

  float scale,y;
  int i;

  scale = 360/ n;
  
  for(y = i=0;i<n;++i, y += scale) 
    data[i] = 30 + 10 * cos(deg2rad(y)) + 5 * sin(8*deg2rad(y)) + randg(0,3);

 return data;     
}

void modeDist(float *in, int nin, int *out, int nout)
{
    float delta;
    float mmin,mmax;
    int i,idx;

    mmin = mmax = in[0];
    for(i=1;i<nin;++i) 
	if(in[i] > mmax)
	    mmax = in[i];
	else if(in[i] < mmin)
	     mmin = in[i];

    delta = mmax/nout;

    memset(out,0,sizeof(int)*nout);

    for(i=0;i<nin;++i) {
	idx = in[i]/delta + 0.5;
	out[idx]++;
    }

}

void regline(float *x, float *y, int n,
             float *b, float *a,
	     float *r)
{
    /* In a Least Squares sense, fit the line:
     * y = a + bx. Returns the slope in `b' and
     *  the offset in `a'. It returns Pearson's
     * correlation in `r'.
     *
     * By Hugues Talbot, from Numerical Recipes
     */
    
    int i;
    double t,u,sxoss,syoss;
    double sx=0.0,sy=0.0,sx2=0.0,sy2=0.0,sxy=0.0;

    *b=0.0;
    for (i=0;i<n;i++) {
        sx += x[i];
        sy += y[i];
    }
    sxoss=sx/n;
    syoss=sy/n;
    for (i=0;i<n;i++) {
        t=x[i]-sxoss;
        u=y[i]-syoss;
        sx2 += t*t;
        sy2 += u*u;
        sxy += t*u;
        *b += t*y[i];
    }
    if ((n > 0) && (sx2 > 0.0) && (sy2 > 0.0)) {
        *b /= sx2;
        *a = (sy-sx*(*b))/n;
	*r = sxy/sqrt(sx2*sy2);
    } else {
        *a = *b = 0;
    }
}

void crosscorF(float *x, 
	 float  *y,
	 float *r,
	 int nx, int nr)
{
    /* cross correlates function `x' against
     * `y'. The output is put into
     * array `r', centred at nr/2.
     * Note: the maximum correlation will exist
     * at the minium in `r'.
     *
     * `nx' is the length of `x' and `y'.
     * `nr' is the length of `r', and `nr' <= `nx'.
     *
     * By Ed Breen
     */
    
    int hr,i,j,k,n;
    float sum;

    hr = nr>>1;

    for(n=nx,i=hr;i<nr;++i,--n) {
	for(sum=0,k=i-hr,j=0;k<nx;++j,++k) 
	    sum += fabs(x[j]-y[k]);
	r[i] = sum/n;
    }
    for(i=1;i<=hr;++i) {
	for(sum=0,k=i,j=0;k<nx;j++,k++)
	    sum += fabs(x[k]-y[j]);
	r[hr-i] = sum/(nx-i);
    }
}

void crosscorI(int *x, 
	 int  *y,
	 int *r,
	 int nx, int nr)
{
    /* cross correlates function `x' against
     * `y'. The output is put into
     * array `r', centred at nr/2.
     * Note: maximum correlation will exist
     * at the minium in `r'.
     *
     * `nx' is the length of `x' and `y'.
     * `nr' is the length of `r', and `nr' <= `nx'.
     *
     * By Ed Breen
     */
    
    int hr,i,j,k,n;
    int sum;

    hr = nr>>1;

    for(n=nx,i=hr;i<nr;++i,--n) {
	for(sum=0,k=i-hr,j=0;k<nx;++j,++k) 
	    sum += abs(x[j]-y[k]);
	r[i] = sum/n;
    }
    for(i=1;i<=hr;++i) {
	for(sum=0,k=i,j=0;k<nx;j++,k++)
	    sum += abs(x[k]-y[j]);
	r[hr-i] = sum/(nx-i);
    }
}


int getlagF(float *x, float *y, int nx, float *minimum)
{
    float *r = malloc(sizeof(float)*nx);
    int i, shft;
    float m;

    crosscorF(x,y,r,nx,nx);
    m = *r;
    shft = 0;
    for(i=1;i<nx;++i)
	if(r[i] < m) {
	    m = r[i];
	    shft = i;
	}

    free(r);
    *minimum = m;
    return shft - (nx>>1);
}

int getlagI(int  *x, int *y, int nx,
	    int *minimum)
{
    int *r = malloc(sizeof(float)*nx);
    int i, shft;
    int m;

    crosscorI(x,y,r,nx,nx);
    m = *r;
    shft = 0;
    for(i=1;i<nx;++i)
	if(r[i] < m) {
	    m = r[i];
	    shft = i;
	}

    free(r);
    *minimum = m;
    return shft - (nx>>1);
}

float getSum(float *data,int nx)
{
    int i;
    float sum = *data++;
    for(i = 1;i<nx;++i)
	sum += *data++;

    return sum;
}


float getStd(float *data,int nx)
{
    float mean = getSum(data,nx)/nx, d;
    double std = 0;
    int i;

    for(i=0;i<nx;++i) {
	d = *data++ - mean;
	
	std += d*d; 
    }

    return sqrt(std/(nx-1));
}
