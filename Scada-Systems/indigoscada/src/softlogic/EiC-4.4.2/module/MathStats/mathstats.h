#ifndef MATHSTATSH_
#define MATHSTTSH_

#define sortFloats(d,x)  qsort(d,x,sizeof(float),compareFloats)

#define showVector(data, n) do {int _i; for(_i = 0;_i<n;++_i)\
				    printf("%g,",(double)data[_i]);printf("\n");} while(0)

#define showArray(data,nx,ny) do {int _i,_j,_c;\
				      for(_c=_i = 0;_i<ny;++_i) {\
					 printf("%3d: ",_i);\
					 for(_j =0;_j<nx;++_j,++_c)\
					  printf("%8.4g,",(double)data[_c]);\
				      printf("\n");}} while(0)

#ifndef deg2rad
#define deg2rad(a)   (0.0174533*(a))
#endif


int compareInts(const void *i1, const void *i2);
int compareFloats(const void *i1, const void *i2);
void subarray(void *IN, void *OUT, int n, int sizein, int sizeout);
int * float2int(float *f, int n);
float * int2float(int *f, int n);
float * readarray(char *fname, int ncols, int *nrows);
float getRange(float * data, int n);


int * rselect(int N, int ns, int seed );
float randg(float mean, float sigma);

int *intShell(int *y, int n);
float *floatShell(float *y, int n);

float * getcurve(float * data, int n);
void modeDist(float *in, int nin, int *out, int nout);
void regline(float *x, float *y, int n,
             float *b, float *a,
	     float *r);
void crosscorF(float *x, 
	 float  *y,
	 float *r,
	 int nx, int nr);
void crosscorI(int *x, 
	 int  *y,
	 int *r,
	 int nx, int nr);
int getlagF(float *x, float *y, int nx, float *minimum);
int getlagI(int  *x, int *y, int nx,
	    int *minimum);
float getSum(float *data,int nx);
float getStd(float *data,int nx);


float ** read_arrays(char *input,  /* filename to get data from */
		     int *nx,      /* array used to return array lengths in*/
		     int *na);      /* number of arrays read returned here */

void free_arrays(float **arrays,int na);
float ** readInTabFloats(FILE *fp,
		       int *nv,
		       int nc);

/* combinatorial.c */
void nexcom(int N, int K, int *R, int *MTC);

#endif







