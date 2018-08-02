#include <stdio.h>
#include <assert.h>

int a[2][3];
int b[2][3] = { {1,2,3}, {4,5,6} };
int c[2][3] = { {1,2,}, {4,5,}, };
int d[2][3] = { 1,2,4,5 };

int w[][3] = {0,0,0,0,0,0};
int x[][3] = {{1,2,3}, {4,5,6}};
int y[][3] = { {1,2,}, {4,5,}, };
int z[][3] = { 1,2,4,5 };

int *t = d[0];
int **u;
int *v[2];


char * names1[] = {
    "Edmond",
    "Claire",
    "Sarah",
    "Alexandra",
    "Georgia",
};

char names2[][12] = {
    "Edmond",
    "Claire",
    "Sarah",
    "Alexandra",
    "Georgia",
};

char yell[] = "Hello, world!";

char _3d[][2][5] = {
{ "TooLongToFit", },
{ "Me", "Yes"},
};


struct { int a, b;} ab[3] = {1,2,3,4,5,};
struct { float a,b;} fab[3] = {{1},{2},{3}};
struct { short a,b; } sab[] = {1,2,3,4,5,6,7};
union { char a; int b; float c; }un[3] = { 'a', 'b', 'c'};


void showarray(int a[2][3])
{
    
    int i,j;
    for(i=0;i<2;++i) {
	for(j=0;j<3;++j)
	    printf("%d ",a[i][j]);
	/*printf("\n");*/
    }
}    


int main()
{
    int  n,m,l, i,j,k,C;

    v[0] = c[0];
    v[1] = c[1];

    u = v;
    t = d[0];

    showarray(a); printf(":-> ");showarray(w); printf("\n");
    showarray(b); printf(":-> ");showarray(x); printf("\n");
    showarray(c); printf(":-> ");showarray(y); printf("\n");
    showarray(d); printf(":-> ");showarray(z); printf("\n");

    
    printf("14 :-> %d\n"
	   "[Hello, world!] :-> [%s]\n",sizeof(yell),yell);
    printf("%d :-> %d\n",
	   sizeof(names1)/sizeof(char*),
	   sizeof(names2)/sizeof(names2[0]));
    for(i=0;i<sizeof(names1)/sizeof(char*);++i)
	printf("%s :-> %s\n",
	       names1[i],names2[i]);

    for(i=0;i<3;++i)
	printf("%d %d ",ab[i].a,ab[i].b);
    printf(":-> 1 2 3 4 5 0\n");

    for(i=0;i<3;++i)
	printf("%g %g ",fab[i].a,fab[i].b);
    printf(":-> 1 0 2 0 3 0\n");
    
    for(i=0;i<4;++i)
	printf("%d %d ",sab[i].a,sab[i].b);
    printf(":-> 1 2 3 4 5 6 7 0\n");
    
    printf("%c%c%c :-> abc\n",un[0].a, un[1].a, un[2].a);

    printf("%d%d%d  :-> %d%d%d\n"
	   "%d%d%d  :-> %d%d%d\n",
	   v[0][0],v[0][1],v[0][2],c[0][0],c[0][1],c[0][2],
           u[1][0],u[1][1],u[1][2],c[1][0],c[1][1],c[1][2]);


    printf("%d%d%d%d :-> %d%d%d%d\n",
	   t[0],t[1],t[2],t[3],d[0][0],d[0][1],d[0][2],d[1][0]);
    

    n = sizeof(_3d)/sizeof(_3d[0]);
    m = sizeof(_3d[0])/sizeof(_3d[0][0]);
    l = sizeof(_3d[0][0]);
    printf("%d, %d, %d :-> 2, 2, 5\n",n, m, l);

    for(i=0;i<n;++i) {
	printf(":");
	for(j=0;j<m;j++) {
	    printf(":");
	    for(k=0;k<l;k++) {
		C = _3d[i][j][k];
		printf("%c",C>0?C:'0');
	    }
	    
	}
    }
    
    printf(" :-> ::TooLo:00000::Me000:Yes00\n");

    return 0;
}

#ifdef EiCTeStS
main();
#endif












