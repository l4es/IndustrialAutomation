#include <stdlib.h>
#include <assert.h>

int * ap1(int n)
{
    return malloc(sizeof(int) * n);
}

int ** ap2(int x,int y)
{
    int **p,i,k;

    p = malloc(sizeof(int *) * y);

    for(i=0;i<y;i++) {
	p[i] = ap1(x);
	for(k = 0;k<x;k++)
	    p[i][k] = k + i;
    }
    return p;
}

int ***ap3(int x, int y, int z)
{
    int ***p;
    int i;
    
    p = malloc(sizeof(int **) * z);
    for(i=0;i<z;i++)
	p[i] = ap2(x,y);

    return p;
}


int ****ap4(int x, int y, int z, int t)
{
    int ****p;
    int i;
    
    p = malloc(sizeof(int ***) * t);
    for(i=0;i<t;i++)
	p[i] = ap3(x,y,z);

    return p;
}

void free2D(int **p, int y)
{
    int i;
    for(i=0;i<y;++i)
	free(p[i]);
    free(p);
}

void free3D(int ***p, int y, int z)
{
    int i;
    for(i=0;i<z;i++)
	free2D(p[i],y);
    free(p);
}

void free4D(int ****p, int y, int z, int t)
{
    int i;
    for(i=0;i<t;i++)
	free3D(p[i],y,z);
    free(p);
}

void T1()
{
    int x = 3, y = 3;
    int **p, i,k;

    p = ap2(x,y);
    for(i=0;i<y;i++)
	for(k=0;k<x;k++)
	    assert(p[i][k] == k + i);
    
    free2D(p,y);
}

void T2()
{
    int x = 3, y = 3, z = 3;
    int ***p, i,k,j;

    p = ap3(x,y,z);

    for(j=0;j<z;j++)
	for(i=0;i<y;i++)
	    for(k=0;k<x;k++)
		assert(p[j][i][k] == k + i);
    
    
    free3D(p,y,z);
}

void T3()
{
    int x = 3, y = 3, z = 3, t= 3;
    int ****p, i,k,j,l;

    p = ap4(x,y,z,t);

    for(l=0;l<t;l++)
	for(j=0;j<z;j++)
	    for(i=0;i<y;i++)
		for(k=0;k<x;k++)
		    assert(p[l][j][i][k] == k + i);
    
    
    free4D(p,y,z,t);
}
	

int main()
{
    T1();
    T2();
    T3();
    return 0;
}

#ifdef EiCTeStS
main();
#endif












