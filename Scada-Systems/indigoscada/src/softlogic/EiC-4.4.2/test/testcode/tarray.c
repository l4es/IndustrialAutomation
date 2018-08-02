int size=5;
int a[4+1];
int t;

for (t=0; t<size; t++)
    a[t] = size-t;

for (t=0; t<size; t++)
    printf ("a[%d] = %d\n", t, a[t]);

