void nexcom(int N, int K,
	    int * R, 
	    int *MTC)
{
  /*  get next composition of n into k parts.  
     (Nijenhuis, J. (1978) Combinatorial algorithms. pp 46-51. N.Y. Academic Press)

  ** N: Number of elements in the universe.
  ** K: Number of elements in the desired subset.
  ** R: Output array 
  ** MTC: Logical: *MTC = 0 for first call for a new sequence. 1 otherwise. 
  */

    static int T=0,H=0;
    int i;

    /* you are allowed to point 1 position before and 1 after
       a legal area.
     */
    R--;
    
    if(!*MTC) {
	R[1] = N;
	T = N;
	H = 0;
    	if(K != 1)
	    for(i=2; i <= K; i++)
		R[i] = 0;
	*MTC = R[K] != N;
	return;
    }
    if(T > 1)
	H = 0;
    H = H + 1;
    T = R[H];
    R[H] = 0;
    R[1] = T - 1;
    R[H+1] = R[H+1] + 1;
    *MTC = R[K] != N;

}	


#if 0

void test_nexcom()
{

  int W[3], mtc=0,j;

  do {
    nexcom(5,sizeof(W)/sizeof(int),W,&mtc);
    for(j=0;j<sizeof(W)/sizeof(int);j++) 
      printf("%d,",W[j]);
    printf(": %d\n",mtc);
  }while(mtc);

}

#endif



