
struct Dvector
{
    double x, y;
};

void PrintDVector (struct Dvector * v)
{
    printf ("--- v = (%g/%g)\n", v->x, v->y);
}

struct Dvector v, v2;

v.x = 1.0;
v.y = 1.5;
v2.x = 1.1;
v2.y = 1.6;

printf ("--- v = (%g/%g)\n", v.x, v.y);

PrintDVector (&v);
PrintDVector (&v2);

:show PrintDVector

