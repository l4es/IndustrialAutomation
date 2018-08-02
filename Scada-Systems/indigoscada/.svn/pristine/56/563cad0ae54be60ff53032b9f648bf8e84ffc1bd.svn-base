#include <assert.h>
#include <string.h>

void T1()
{
    int *p,i = 2, k = 0,n;
    int a[5] = {5,4,3,2,1};

    p = a;
    
    assert(p == a);
    assert(*(1+p) == a[1]);
    assert(*(p+1) == a[1]);
    assert(*(i+p) == a[i]);
    assert(*(i+k+p) == a[i+k]);
    assert(*(p+i+k) == a[i+k]);

    assert(p-1 < a);
    assert(p-i < a);
    assert(p+1 > a);
    assert(1+p > a);
    assert(i+p > a);

    assert(p-1 <= a);
    assert(p-i <= a);
    assert(p+1 >= a);
    assert(p+i >= a);
    assert(1+p >= a);
    assert(i+p >= a);

    assert(p-1 != a);
    assert(p-i != a);

    assert(1+p > a);

    n = sizeof(a)/sizeof(int);

    for(i=0;i<n;++i)
	assert(p+i == &a[i] && *(i+p) == a[i]);

    p = a + n-1;
    
    for(i=0,k=n-1;i<n;i++,k--)
	assert(*(p-k) == a[i]);

    i = 1;
    p = &i;

    assert(a[*p] == a[i]);

}

void T2()
{
    int aa[2][5] = {{4,3,2,1,0}, {8,6,4,2,0}};
    int (*q)[5];
    
    /* the following assignments to q are equivalent
     *  and should not generate any diagnostics.
     */

    q = &aa[0];
    q = aa;
    q = *&aa;
    
    assert(q == aa);
    assert(q == &aa[0]);
    assert(q+1 == &aa[1]);
    assert(1+q == &aa[1]);
    assert(**aa == 4);
    assert(**(aa+1) == 8);
    assert(*(aa+1) == aa[1]);
    assert(*(1+aa) == aa[1]);
    assert(*(1+aa)+2 == &aa[1][2]);
    assert(*(2+*(aa+1)) == aa[1][2]);

    
    assert(sizeof aa == sizeof *&aa);
    assert(sizeof aa == 2 * 5 * sizeof(int));
    assert(sizeof aa[0] ==  5 * sizeof (int));
    assert(sizeof aa[0][0] == sizeof(int));

}

void T3(void)
{
    int a[1];
    a[0] = 23;
    assert(a ==&a[0]);
    assert(a == *&a);
    assert(a[0] == 23);
    assert(a[0] == *a);
    assert(a[0] == **&a);
}
    
char *my_strcpy(char dest[], char source[])
{
    int i = 0;
    while (source[i] != '\0')
    {
	dest[i] = source[i];
	i++;
    }
    dest[i] = '\0';
    return dest;
}

void T4(void)
{
    char a1[15], a2[15] = "hello world";
    assert(strcmp(a2,my_strcpy(a1,a2)) == 0);
    assert(strcmp(a1,a2) == 0);
}


#define ROWS 5
#define COLS 10


void set_value(int m_array[][COLS],int val)
{
    int row, col;
    for(row = 0; row < ROWS; row++)
    {
	for(col = 0; col < COLS; col++)
	{
	    m_array[row][col] = val;
	}
    }
}

void T5(void)
{

    int multi[ROWS][COLS];
    int row, col;
    for (row = 0; row < ROWS; row++)
	for(col = 0; col < COLS; col++)
	    multi[row][col] = row*col;
    for (row = 0; row < ROWS; row++)
	for(col = 0; col < COLS; col++)
	    assert(multi[row][col] == *(*(multi + row) + col));

    set_value(multi,5);
    for (row = 0; row < ROWS; row++)
	for(col = 0; col < COLS; col++)
	    assert( multi[row][col] == 5);
    
}

#undef ROWS
#undef COLS


int aa[2][5] = {{4,3,2,1,0}, {8,6,4,2,0}}, k=1;
int (*q)[5];

void T6()
{
    assert(aa[1][2] == *(aa[1] + 2));
    assert(aa[1][2] == *(aa[k] + 2));
    assert(aa[1][1] == *(aa[k] + k));
    assert(aa[1][1] == *(aa[1] + k));

    q = &aa[0];
    q = aa;
    q = *&aa;
    
    assert(q == aa);
    assert(q == &aa[0]);
    assert(q+1 == &aa[1]);
    assert(1+q == &aa[1]);
    assert(**aa == 4);
    assert(**(aa+1) == 8);
    assert(*(aa+1) == aa[1]);
    assert(*(1+aa) == aa[1]);
    assert(*(1+aa)+2 == &aa[1][2]);
    assert(*(2+*(aa+1)) == aa[1][2]);

    
    assert(sizeof aa == sizeof *&aa);
    assert(sizeof aa == 2 * 5 * sizeof(int));
    assert(sizeof aa[0] ==  5 * sizeof (int));
    assert(sizeof aa[0][0] == sizeof(int));

}


int main(void)
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












