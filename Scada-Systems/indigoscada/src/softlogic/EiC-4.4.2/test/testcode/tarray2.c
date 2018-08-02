struct demo
{
    int a;
    int b;
};

struct demo a[5];
int t;

for (t=0; t<5; t++)
{
    a[t].a = 5-t;
    a[t].b = t;
}

for (t=0; t<5; t++)
    printf ("a[%d].a/b = %d/%d\n", t, a[t].a, a[t].b);

