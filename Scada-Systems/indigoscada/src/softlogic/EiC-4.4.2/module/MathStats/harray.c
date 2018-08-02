#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define sortFloats(d,x)  qsort(d,x,sizeof(float),compareFloats)

#define showVector(data, n) do {int _i; for(_i = 0;_i<n;++_i)\
				    printf("%g,",(double)data[_i]);printf("\n");} while(0)

#define showArray(data,nx,ny) do {int _i,_j,_c;\
				      for(_c=_i = 0;_i<ny;++_i) {\
					 printf("%3d: ",_i);\
					 for(_j =0;_j<nx;++_j,++_c)\
					  printf("%8.4g,",(double)data[_c]);\
				      printf("\n");}} while(0)


static int compareInts(const void *i1, const void *i2)
{
    if(*(int*)i1 > *(int*)i2)
	return 1;
    else if(*(int*)i1 <  *(int*)i2)
	return -1;
    else
	return 0;
}
static int compareFloats(const void *i1, const void *i2)
{
    if(*(float*)i1 > *(float*)i2)
	return 1;
    else if(*(float*)i1 <  *(float*)i2)
	return -1;
    else
	return 0;
}



void subarray(void *IN, void *OUT, int n, int sizein, int sizeout)
{
    /* Subset `n' elements form the array in `in' and
     * place them in `out'
     * usage:
     *		subarray(data,sub,28,sizeof(*data),sizeof(*sub));
     */

    char *in = IN;
    char *out = OUT;
    int i;
    for(i=0;i<n;i++,in+=sizein,out+=sizeout)
	memcpy(out,in,sizeout);
}


int * float2int(float *f, int n)
{
    /* convert a float  array to an int array
     *
     * The returned array is static.
     */
    static int *d=NULL;
    int i;

    d = realloc(d,sizeof(int)*n);
    for(i=0;i<n;++i,++f,++d)
	*d = *f;
    d -= n;
    return d;
}

float * int2float(int *f, int n)
{
    /* convert an int array to a float array
     *
     * The returned array is static.
     */
    static float *d=NULL;
    int i;

    d = realloc(d,sizeof(float)*n);
    for(i=0;i<n;++i,++f,++d)
	*d = *f;
    d -= n;
    return d;
}


void free_arrays(float **arrays,int na)
{
  int i;
  for(i=0;i<na;i++)
    free(arrays[i]);
  free(arrays);
}

#define RA_moD 25


#define seParator(x)  (isspace(x) || x == ',')

float ** read_arrays(char *input,  /* filename to get data from */
		     int *nx,      /* array used to return array lengths in*/
		     int *na)      /* number of arrays read returned here */
{ 
  /* this function reads N arrays of floats
     from file named input, it expects
     that each array is separated by 
     empty or lines of text, which don't
     begin with a number. The file
     can also contain lines of header info.
     For example, type of input can be like:

              THis is the first array
              12.3 15
	      13.4 10
	      This is the 
              2nd array
	      15.6
	      15.6
	      12.3
	      There is no more info


     The column separator can be any whitespace or comma
     or combination thereof. The program also assumes that
     caller knows the number of columns associated with each
     array.     
     
     Each array can be multidimensional. In which
     case array elements are interleaved into 
     a one dimensional array.

     It returns a pointer to the arrays read
     and it is the callers responsibility
     to free it up via say a call to 
     free_arrays(float **f, int n);

     It will return NULL on zero input;
     It returns the length of each array read
     in the array nx, which is assumes is long enough
     to hold this data. The number of arrays 
     actually read will be returned in ni.

  */

  float **inarray = NULL;
  FILE *in = fopen(input,"r");
  float item;
  char buff[512];
  int s;
  *na = 0;

  if(!in) {
    fprintf(stderr,"failed to read [%s]",input);
    return NULL;
  }
  
  while(!feof(in)) {
    /*printf("doing %d\n", *na+1);*/
    /* skip over header and blank lines*/
    while(fgets(buff,512,in))
      if(sscanf(buff,"%g",&item) == 1)
	break;
    s = 0;
    if(!feof(in)) {
      int x = 0;
      int nlines = 0;
      inarray = realloc(inarray,sizeof(*inarray) *(*na+1));
      inarray[*na] = NULL;
      nx[*na] = 0;

      while(buff[s] && seParator(buff[s])) s++;

      do {

	if(!(x % RA_moD))
	  inarray[*na] = realloc(inarray[*na],(RA_moD+1+x) * sizeof(**inarray));

	inarray[*na][x++] = item;
	/*printf("item = %g\n",item);*/

	/* go to next number */
	while(buff[s] && !seParator(buff[s])) s++;
	while(buff[s] && seParator(buff[s])) s++;

	if(!buff[s]) {
	  s = 0;
	  fgets(buff,512,in);
	  if(!feof(in)) {
	    while(buff[s] && seParator(buff[s])) s++;
	    nlines++;
	  } else
	    buff[0] = 0;
	}

      } while(sscanf(&buff[s],"%g",&item) == 1);

      if(x) {
	nx[*na] = nlines;
	*na +=1;
      }
    }
  }
  return inarray;

}
#undef Separator


float * readarray(char *fname, int ncols, int *nrows)
{
    /* Read a 2D array of ascii numerical values
     * from file `fname'. It will assume that the file
     * format consists of nrows by ncols.
     * It returns a pointer to the data read,
     * plus it will place in nrow the number of rows read.
     *
     * The returned array is dynamic. That is, it is
     * your responsibility to free it.
     * p.s this code should be made safe for the weak
     * at heart.
     */
    
    float *data=NULL;
    FILE *fp;
    int i = 0;

    if(*fname == '-' && strlen(fname) == 1)
	fp = stdin;
    else
	fp = fopen(fname,"r");
    if(!fp)
	return 0;
    do 
      if(!(i % RA_moD))
	data = realloc(data,(i + RA_moD) * sizeof(float));
    while(fscanf(fp,"%g",&data[i++]) == 1);


    *nrows = (i-1)/ncols;

    if(fp != stdin)
	fclose(fp);
    return data;
}    

#undef RA_moD


float getRange(float * data, int n)
{
    int i;
    float mx, mn;
    mx = mn = *data++;
    for(i=1;i<n;++i,++data) {
	if(mx < *data)
	    mx = *data;
	if(mn > *data)
	    mn = *data;
    }

    return mx - mn;

}
   

int getNumTabCols(FILE *fp)
{
  /* this function determines the number of tab delimited
     columns in stream `fp'. It does it via   
     the first line of text pointed 2  by `fp' and by adding
     1 to the number of tabs stops met, which could be zero.

     Returns number of columns read.
  */
  int nc = 0,c;
  long p = ftell(fp);
  while((c=fgetc(fp)) != EOF) {
    if(c == '\n' || c == '\r') {
      nc++;
      break;
    }
    if(c == '\t')
      nc++;
  }
  /* now rewind to original position*/
  fseek(fp,p,SEEK_SET);
  return nc;
}

    
float ** readInTabFloats(FILE *fp,
			 int *nv,
			 int nc)
{
  /* This function reads in a tab delimited file from
     the input stream fp. Such as output by Excel (moan).

     It expects columns of data separated by tabs.
     The columns don't have to be the same length
     but respective columns must always have the
     same number of tabs between them; that is, the
     column values always starts on the same line 
     location:
     

        12.5   16   13  4
        15          10  6
                    4


     nc is the number of columns to read, it should
     have been determined prior to calling this function --
     via a call 2 getNumTabCols, perhaps.
     nv is an array, nc elements long, that will 
     be used to store the number of values read
     for each column.

     Any carriage return met is treated as a row break.

     Returns the data as a static array of pointers 
     to floats; so don't try to free the array!!
     
     Returns NULL on error.

  */

  static float **f = NULL;
  static unsigned N = 0;

  float v;
  int i,c, col = 0;

  for(i=0;i<nc;i++)
    nv[i] = 0;
    
  if(f==NULL) 
    f = calloc(sizeof(*f),nc);
  else {
    if(nc < N)
      for(i=nc;i<N;i++)
	free(f[i]);
    f = realloc(f,sizeof(*f)*nc); 
  }
  N = nc;
  while((c=fgetc(fp)) != EOF) {
    /* expect mode */
    if(!isspace(c)) {
      ungetc(c,fp);
      if(fscanf(fp,"%g",&v) != 1) {
	return NULL;

      }
      /*printf("%d:%g",col,v);*/

      f[col] = realloc(f[col],(nv[col]+1)*sizeof(**f));
      f[col][nv[col]++] = v;
      /* chew up delimiter */
      c=fgetc(fp);
    }
    if(c == '\r')
	c=fgetc(fp);
    col = (col+1)%nc;
    if(c == '\n')
      col = 0;
    /*putchar(c);*/
  }

  return f;

}








