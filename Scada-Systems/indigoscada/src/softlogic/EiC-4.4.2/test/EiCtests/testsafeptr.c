
/* boundary test for safe ptrs */
/* run -D_EiC -I../include */
#include <stdlib.h>

char *p = "hello";
p[0];p[4]; p[5];



char *q = p;

q[0]; q[5];

{char *q, *p = "hello"; p[0];p[5];q=p; q[0];q[5];}  




int a[5]; a[0]; a[4];

int aa[2][2]; aa[0][0]; aa[1][1];
char *pp[] = {"hello", "world"};

pp[0][0]; pp[1][5];

char *mp = malloc(10); mp[0]; mp[9];
void foo(char *p) {p[0]; p[9];} foo(mp);
free(mp);


int c[2][3] = { {1,2,}, {4,5,}, };
int *v[2];
v[0] = c[0]; v[1] = c[1];
v[0][0]; v[1][2]; c[1][2];


int *ip, i; ip = &i; *ip = 5; i== *ip;
 
int ia1d[5]; *ia1d;


typedef struct {int x, y;} point_t; 

void ref(){ static point_t p; p.x;} ref();

int i1 = 1, i2 = 2;
int *i1_2[] = {&i1,&i2};

*i1_2[0]; *i1_2[1];








