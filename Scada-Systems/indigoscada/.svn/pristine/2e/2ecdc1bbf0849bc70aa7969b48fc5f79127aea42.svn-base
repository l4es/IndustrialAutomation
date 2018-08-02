#include <stdio.h>
#include <stdlib.h>
#include <assert.h>



int compints(const void *d1, const void *d2)
{
    if(*(int *)d1 > *(int*)d2)
       return 1;
    else if(*(int *)d1 < *(int*)d2)
	return -1;
    return 0;
}


#define print for(i=0;i<N;++i) printf("%3d ",data[i]); printf("\n");
#define N 10

int main()
{
    int k = 0;    
    int i, data[N] = { 110,86,22,100,85,74,25,109,95,116};

    /* test qsort */
    print;
    qsort(data,N,sizeof(int),compints);
    print;


    /* tests for bsearch */
    printf(" %d = %d\n", data[0],
	   *(int*)bsearch(&data[0],data,N,sizeof(int),compints));
    printf(" %d = %d\n", data[N-1],
	   *(int*)bsearch(&data[N-1],data,N,sizeof(int),compints));    
    printf(" %d = %d\n", data[N>>1],
	   *(int*)bsearch(&data[N>>1],data,N,sizeof(int),compints));

    assert(bsearch(&k,data,N,sizeof(int),compints) == NULL);

    return 0;
}    
    
#ifdef EiCTeStS
main();
#endif
	
	       
	    


