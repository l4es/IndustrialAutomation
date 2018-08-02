/* this file is for
   interpolating functions
*/


int Lagrange3pt(float *data,
		 float *res,
		 int nx,
		 float p)
{
  /* Lagrange 3 point interpolater, written by
   *  Ed Breen (c) 1999
   * 
   * `data' array holding initial values
   *    assumes that data values are 1 unit apart 
   *
   * `res' array where results will be stored.
   *      assumes that `res' will be large enough.
   *      If `res' equals NULL then results printed to 
   *      stdout.
   * `p'  sampling interval
   *    assumes that 0 < p <= 1
   *
   * Returns the number of values interpolated.
   * Returns the interpolated values in 
   * `res' if not NULL.
   *
   * Note: point of max/min if exsists
   *       between any three input values (A,B,C) 
   *       will be at:
   *
   *           0.5*(A-C)/(A-2*B+C)
   *
   *      where A = data(-1); B = data(0);
   *            C = data(+1);
   *    zero being a centre pt.
   */
  int i,k=0;
  float idx;
  
  for(idx=1,i=0;i<nx-1;i+=2) {
    for(idx -= 2;idx<1;idx+=p) {
      float sq = idx * idx;
      float v = (sq-idx)/2*data[i] + 
	(1-sq)*data[i+1] +
	(sq+idx)/2*data[i+2];
      if(res)
	res[k] = v;
      else
	printf("%2d:\t%g\n",k,v);
      k++;
    }
  }
  
  /* should really test if this is appropriate
   *  or not!!
   */
  if(res)
    res[k] = data[nx-1];
  else
    printf("%2d.00:%g\n",k,data[nx-1]);

  return ++k;
}






