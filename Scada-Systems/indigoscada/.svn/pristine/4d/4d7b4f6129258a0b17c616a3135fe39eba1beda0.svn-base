/* taken from P.J. Plauger's book The Standard C library */ 


#include <stdio.h>
#include <setjmp.h>
#include <assert.h>


int ctr;
jmp_buf b0;

void jmpto(int n)
{
    longjmp(b0,n);
}

char *stackptr(void)
{
    char ch;

    return &ch;
}


int tryit(void)
{
    jmp_buf b1;
    char *sp = stackptr();

    ctr = 0;

    switch(setjmp(b0)) {
	case 0: 
	    printf("case 0\n");
	    assert(sp == stackptr());
	    assert(ctr == 0);
	    ++ctr;
	    jmpto(0);
	    break;
	case 1:
	    printf("case 1\n");
	    assert(sp == stackptr());
	    assert(ctr == 1);
	    ctr++;
	    jmpto(2);
	    break;
	case 2:
	    printf("case 2\n");
	    assert(sp == stackptr());
	    assert(ctr == 2);
	    ++ctr;

	    switch(setjmp(b1)) {
		case 0:
		    printf("\tcase 2: 0\n");
		    assert(sp == stackptr());
		    ++ctr;
		    longjmp(b1,-7);
		    break;
		case -7:
		    printf("\tcase 2: -7\n");
		    assert(ctr == 4);
		    ++ctr;
		    jmpto(3);
		    break;
		case 5:
		    printf("\tcase 2: 5\n");
		    return 13;
		default:
		    printf("\tcase 2: default\n");
		    return 0;
	    }
	
	case 3:
	    printf("case 3\n");
	    longjmp(b1,5);
	    break;
    }
			   
    return -1;
}



int main()
{
    assert(tryit() == 13);
    puts("SUCCESS testing <setjmp.h>");
    
    return 0;
}




#ifdef EiCTeStS
main();
#endif







