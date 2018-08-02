#include <assert.h>
/*------Test-2------------------------------------*/
void Sort_array(int Tab[],int Last)
{
   int Swap;
   int Temp,I;
   do {
      Swap = 0;
      for (I = 0; I<Last; I++)
	 if (Tab[I] > Tab[I+1]) {
	    Temp = Tab[I];
	    Tab[I] = Tab[I+1];
	    Tab[I+1] = Temp;
	    Swap = 1;
	    }
      }
   while (Swap);
}

void T2 ()
{
    int Tab[100];
    int I,J,K,L;
    int n = sizeof(Tab)/sizeof(int);

    for (L = 0; L < 1; L++) {
	/* Initialize the table that will be sorted. */
	K = 0;
	for (I = 9; I >= 0; I--)
	    for (J = I*10; J < (I+1)*10; J++)
		Tab[K++] = J&1 ? J+1 : J-1;
	Sort_array(Tab,99);	/* Sort it. */
	while(--n)
	    assert(Tab[n] > Tab[n-1]);
    }
}


/*--------Test-1---------------------------------*/
#define BOUND 15

void T1()
{
    int a[BOUND][BOUND],b[BOUND][BOUND],c[BOUND][BOUND];
    int i,j,k;
    for (i=0; i<BOUND; i++) {
	for (j=0; j<BOUND; j++) {
	    a[i][j] = 1;
	    b[i][j] = 1;
	}
    }
    for (i=0; i<BOUND; i++) {
	for (j=0; j<BOUND; j++) {
	    c[i][j] = 0;
	    for (k=0; k<BOUND; k++)
		c[i][j] = c[i][j] + a[i][k] * b[k][j];
	}
    }
    for (i=0; i<BOUND; i++)
	for (j=0; j<BOUND; j++)
	    assert((c[i][j] == BOUND));
}
#undef BOUND
/*-----------------------------------------------*/

void T3()
{
    int i = 2;
    int a[3] = {1,2,3};
    assert("abcd"[2] == 2["abcd"]);
    assert("abcd"[2] == i["abcd"]);
    assert(*"a" == 'a');
    assert(a[1] == 1[a]);
}    

void T4()
{
    /* test 4 subscript types */
    int ar[5] = {11,22,33,44,55};
    char c = 0; short s = 1; long d = 4;
    assert(ar[c] == ar[0]);
    assert(ar[s] == ar[1]);
    assert(ar[d] ==

	   ar[4]);
}    


/*    tests for multidimensional arrays */
#define B 3

void T5()
{
    int a[B][B][B][B];
    int i,j,k,l;
    
    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++)
		for(l=0;l<B;l++)
		    a[i][j][k][l] = k+l;
    
    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++) 
		for(l=0;l<B;l++)
		    a[i][j][k][l] *= 2;

    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++) 
		for(l=0;l<B;l++)
		    assert(a[i][j][k][l] == 2 * (k+l));

}


int a[B][B][B][B];
void T6()
{
    int i,j,k,l;
    
    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++)
		for(l=0;l<B;l++)
		    a[i][j][k][l] = k+l;
    
    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++) 
		for(l=0;l<B;l++)
		    a[i][j][k][l] *= 2;

    for (i=0; i<B; i++) 
	for (j=0; j<B; j++) 
	    for(k=0;k<B;k++) 
		for(l=0;l<B;l++)
		    assert(a[i][j][k][l] == 2 * (k+l));

}
#undef B



int main()
{
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







