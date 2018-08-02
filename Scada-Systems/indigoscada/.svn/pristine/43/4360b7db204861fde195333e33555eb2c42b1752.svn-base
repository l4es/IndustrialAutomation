#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

/***********************************/
void testKR()
{
#define FLOAT 'f'
#define INT   'i'

	union {
		struct {
			int type;
		} n;
		struct {
			int type;
			int intnode;
		} ni;
		struct {
			int type;
			float floatnode;
		} nf;
	}u;
	assert(sizeof(u) == 8);
	u.nf.type = FLOAT;
	u.nf.floatnode = 3;
	assert(u.n.type == FLOAT);
	assert(u.nf.floatnode == 3);
#undef FLOAT
#undef INT
}
/********************************************************/
/********************************************************/
typedef struct {
	int x, y;
} point_t;

void reflect(point_t *p)
{
	p->x = -p->x;
	p->y = -p->y;
}

point_t randpoint(void)
{
	static point_t p;

	p.x = (1 + p.x * 28) % 1013;
	p.y =  p.x * 28 % 1013;
	return p;
}

void testarr2()
{
    struct {
	int x;
	int y;
    } *p, pa[3] = {{1,2},{3,4},{5,6}};

    p = pa;

    assert(p->y == p[0].y);
    assert((p)->y == (p[0]).y);
    assert(p[2].y == 6);
    assert(p->y == (*p).y);
    assert((p+1)->x == pa[1].x);
}

void testarr()
{
	int i,n;

	point_t *p, pa[3];
	p = &pa[0];
	/* test an array of structures */
	n = sizeof(pa)/sizeof(point_t);
	assert(n==3);
	for(i=0;i<n;++i,++p)
		p->x = i, p->y = n - i;
	for(i=0;i<n;++i)
	    assert(pa[i].x == i && pa[i].y == n-i);
	p = &pa[0];

	/* test various addressing schemes */
	assert(p->y == (*p).y);
	assert((p+1)->y == p[1].y);
	assert(p->y == pa[0].y);

	/* test passing a pointer to a structure */
	reflect(&pa[1]);
	assert(pa[1].x == -1 && pa[1].y == -2);

	/* test returning a structure */
	pa[1] = randpoint();
	assert(1 == pa[1].x && 28 == pa[1].y);
}

void tscope()
{
    /* scope and name space tests */
    struct node {int a, b;} node;
    node.a = 5;
    node.b = 10;
    {
	struct node {int a, b, node; double da[5];} node;
	struct node b;
	b.a = node.a = 15;
	b.b = node.b = 30;
	b.node = node.node = 44;
	assert(node.a == 15 && 30 == node.b && node.node == 44);
	assert(b.a == 15 && 30 == b.b && b.node == 44);
	/* array addressing */
	node.da[2] = 15;
	node.da[1] = 30;
	node.da[4] = 4;
	assert(node.da[2] == 15 && 30 == node.da[1]  && node.da[4] == 4);
    }
    assert(node.a == 5 && node.b == 10);
}

/************************************************************/
/************************************************************/
/* test for linked list type stuff */

typedef struct list_t { 
    char *name; 
    struct list_t *next; 
}list_t; 


list_t * add2list(list_t * list,char * name) 
{ 
    list_t *new; 
    new = (list_t*)calloc(sizeof(list_t),1); 
    if(new != NULL) { 
	new->name = (char*)calloc(strlen(name)+1,1); 
	strcpy(new->name,name); 
	new->next = list; 
    } 
    return new; 
} 

void freelist(list_t* list)
{
    /*
     * recursively delete the list.  Not to
     * be taken seriously.
     */
    
    if(list != NULL) {
	freelist(list->next);
	free(list->name);
	free(list);
    }
}

void testlist(list_t *list, ...)
{
    va_list ap;
    va_start(ap,list);
    while(list != NULL) {
	assert( strcmp(va_arg(ap,char*),list->name) == 0);
	list = list->next;
    }
    va_end(args);
}

void testLL()
{
    list_t *p;
    p = add2list(NULL,"first");
    p = add2list(p,"2nd");
    p = add2list(p,"3rd");

    testlist(p,"3rd","2nd","first",NULL);
    freelist(p);
}

/*******************************************************/
/************************************************************/
/* simple tests for const structures and unions */

typedef struct { int a, b;} ab_t;

const ab_t Acon = {5,6};

int Tconst1a(const ab_t c)
{
    return c.a;
}

int Tconst1b(const ab_t c)
{
    return c.b;
}

void Tconst1()
{
    assert(Acon.a == 5 && Acon.b == 6);
    assert(Tconst1a(Acon) == Acon.a);
    assert(Tconst1b(Acon) == Acon.b);
}

void Tconst2()
{
    const ab_t a = { 15,16};
    assert(a.a == 15 && a.b == 16);
}

void Tconst()
{
    Tconst1();
    Tconst2();
}

void Tlvalue()
{
    int i = 2; struct {int a;} a = {15}, b = {69};
    assert((i,b).a == 69);  /* comma operator */
    assert((i == 2 ? a:b).a == 15);  /* conditional operator  */
    assert((a=b).a == 69);         /* assignment operator */
}


int main()
{
    testLL();
    testKR();
    testarr();
    testarr2();
    tscope();
    Tconst();
    Tlvalue();
    return 0;
}


#ifdef EiCTeStS
main();
#endif







