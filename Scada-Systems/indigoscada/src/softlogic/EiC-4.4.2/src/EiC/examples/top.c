
/* Datentypen-Test */
#include <stdio.h>
int main(void)
{
    int a=7;
    int b=4;
    float f=7, g=4.0;
    char c;

    printf ("a = %d (7), b = %d (4)\n", a, b);
    printf ("a + b = %2d (11)\n", a+b);
    printf ("a - b = %2d ( 3)\n", a-b);
    printf ("a * b = %2d (28)\n", a*b);
    printf ("a / b = %2d ( 1)\n", a/b);
    printf ("a %% b = %2d ( 3)\n", a%b);
    printf ("a & b = %2d ( 4)\n", a&b);
    printf ("a | b = %2d ( 7)\n", a|b);
    printf ("a ^ b = %2d ( 3)\n", a^b);
    printf ("a &&b = %2d ( 1)\n", a && b);
    printf ("a ||b = %2d ( 1)\n", a || b);
    printf ("a <<b = %3d (112)\n", a<<b);
    printf ("a >>b = %2d ( 0)\n", a>>b);
    printf ("a < b = %2d ( 0)\n", a<b);
    printf ("a <=b = %2d ( 0)\n", a<=b);
    printf ("a ==b = %2d ( 0)\n", a==b);
    printf ("a !=b = %2d ( 1)\n", a!=b);
    printf ("a >=b = %2d ( 1)\n", a>=b);
    printf ("a > b = %2d ( 1)\n", a>b);

    printf ("f = %f (7.00000), g = %f (4.00000)\n", f, g);
    printf ("f + g = %2g (11)\n", f+g);
    printf ("f - g = %2g ( 3)\n", f-g);
    printf ("f * g = %2g (28)\n", f*g);
    printf ("f / g = %g (1.75)\n", f/g);
/*    printf ("f %% g = %2g ( 3)\n", f%g);*/
    printf ("f &&g = %d (1)\n", f&&g);
    printf ("f ||g = %d (1)\n", f||g);
/*    printf ("f <<g = %3g (112)\n", f<<g);*/
/*    printf ("f >>g = %g (0.4375)\n", f>>g);*/
    printf ("f < g = %2d ( 0)\n", f<g);
    printf ("f <=g = %2d ( 0)\n", f<=g);
    printf ("f ==g = %2d ( 0)\n", f==g);
    printf ("f !=g = %2d ( 1)\n", f!=g);
    printf ("f >=g = %2d ( 1)\n", f>=g);
    printf ("f > g = %2d ( 1)\n", f>g);

    c = 1;
    printf ("c = %+2d (+1)\n", c --);
    printf ("c = %+2d (+0)\n", c --);
    printf ("c = %+2d (-1)\n", c);
    c = 255;
    printf ("c = %+2d (-1)\n", c ++);
    a = c; printf ("c = %+2d (+0)\n", c ++);
    a = c; printf ("c = %+2d (+1)\n", c);
    c = 126;
    printf ("c = %+3d (+126)\n", c++);
    printf ("c = %+3d (+127)\n", c++);
    printf ("c = %+3d (-128)\n", c++);
    printf ("c = %+3d (-127)\n", c++);

    return 0;
}

#ifdef EiCTeStS
main();
:show main
#endif




