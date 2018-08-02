int add (int a, int b)
{
    int c;

    c = a+b;

    printf ("c = %d\n", c);

    return (c);
}

int a;

a = add (4,5);

printf ("a = %d (%d)\n", a, 4+5);


:show add
