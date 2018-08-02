#include <assert.h>
#define BOUND 15
#define BOUND2 11
#define BOUND3 17
#define BOUND4 21

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

{/* scople level 2 */
    int a[BOUND2][BOUND2],b[BOUND2][BOUND2],c[BOUND2][BOUND2];
    int i,j,k;
    for (i=0; i<BOUND2; i++) {
	for (j=0; j<BOUND2; j++) {
	    a[i][j] = 1;
	    b[i][j] = 1;
	}
    }

{/* scope level 3 */
    int a[BOUND3][BOUND3],b[BOUND3][BOUND3],c[BOUND3][BOUND3];
    int i,j,k;
    for (i=0; i<BOUND3; i++) {
	for (j=0; j<BOUND3; j++) {
	    a[i][j] = 1;
	    b[i][j] = 1;
	}
    }
    for (i=0; i<BOUND3; i++) {
	for (j=0; j<BOUND3; j++) {
	    c[i][j] = 0;
	    for (k=0; k<BOUND3; k++)
		c[i][j] = c[i][j] + a[i][k] * b[k][j];
	}
    }
    for (i=0; i<BOUND3; i++)
	for (j=0; j<BOUND3; j++)
	    assert((c[i][j] == BOUND3));

} /* enter scope level 2 */
     
    for (i=0; i<BOUND2; i++) {
	for (j=0; j<BOUND2; j++) {
	    c[i][j] = 0;
	    for (k=0; k<BOUND2; k++)
		c[i][j] = c[i][j] + a[i][k] * b[k][j];
	}
    }
    for (i=0; i<BOUND2; i++)
	for (j=0; j<BOUND2; j++)
	    assert((c[i][j] == BOUND2));



} /* enter scope level 1 */
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

void T2b()
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
void T2a()
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
    T2b();
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

void T2()
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
    T2a();
    for (i=0; i<BOUND; i++)
	for (j=0; j<BOUND; j++)
	    assert((c[i][j] == BOUND));


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


int main()
{
    T1();
    return 0;
}

#ifdef EiCTeStS
main();
#endif







