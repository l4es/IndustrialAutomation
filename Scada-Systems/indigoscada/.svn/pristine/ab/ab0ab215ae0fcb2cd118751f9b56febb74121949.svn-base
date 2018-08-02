bool t;
int i;
string s;

t=TRUE;

for (i=0; i<10; i++)
{
    t=!t;
    s=t;

    printf ("%s\n", s);
}

i = 0;

if (i++)
{
    printf ("Falsch (i wird erst geprüft und dann erhöht)\n");
}
printf ("i = %d (1)\n", i);

if (0 && i++)
{
    printf ("Falsch (0 && ist immer falsch, Rest wird ignoriert)\n");
}

printf ("i = %d (1)\n", i);

if (1 || i++)
{
    if (i != 1)
	printf ("Falsch (1 || ist immer war, Rest wird ignoriert)\n");
}


