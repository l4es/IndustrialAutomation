/*
 *  The version of `printf' in EiC has a few non
 *  standard features, such as:
 * NEW FLAGS:
 *        FLAG     WHAT IT SPECIFIES
 *         |      Centre justification.
 *         !      Used with floating point numbers.
 *                   It specifies, if possible, that the
 *                   number will be centred with respect
 *                   to the decimal point.
 *                   If used with non floating point
 *                   numbers or the floating point number
 *                   does not contain a decimal point,
 *                   the ! flag is equivalent to |.
 * NEW CONVERSION CHARACTER:
 *      CHARACTER	WHAT IT SPECIFIES
 *         b      int, unsigned binary notation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <math.h>

void test1(void)
{
	char *str = "Edmond";
	char *str2 = "Claire Breen";
	int d = 365;
	double f1 = 12.567, f2 = 5666.56789;
	double f3 = 999.99999, f4 = 899.9999;
	printf("\n---------------test1\n");
	printf("|%10d|%|10x|%10X|%10o|\n",d,d,d,d);
	printf("|%+10d|%-+10d|%0+10d|%+10d|\n",d,d,d,-d);
	printf("|%#10x|%-#10X|%|10s|%|.10s|\n",d,d,str,str2);
	printf("|%021.0f|%!21.2f|\n",f1,f2);

	printf("|%021.2f|%!21.2f|\n",f3,f4);
}

void test2(void)
{
	printf("---------- test2\n");
	printf("%f :-> 0.000000\n",0.0);
	printf("%f :-> 1.000000\n",1.0);
	printf("%f :-> 10.000000\n",10.0);
	printf("%f :-> 12.345000\n",12.345);
	printf("%f :-> 1234567.800000\n",1234567.8);
	printf("%.2f :-> 1000.00\n",999.999);
	printf("%.2f :-> 999.99\n",999.985999);
	printf("%f :-> 0.100000\n",.099999999860301614);
	printf("%f :-> 1.000000\n",.999999999860301614);
	printf("%e :-> 0.000000e+00\n",0.0);
	printf("%e :-> 1.000000e+00\n",1.0);
	printf("%e :-> 1.000000e+01\n",10.0);
	printf("%e :-> 1.234568e+11\n", 123456789999.4);
	printf("%e :-> 1.234568e+06\n", 1234567.8);
	printf("%E :-> 1.234568E+06\n", 1234567.8);
	printf("%.5e :-> 4.94066e-300\n",4.9406565584124654e-300);
	/*
	   does not handle long doubles
	printf("%Lf :-> -123456789999.400000\n", -123456789999.4L);
	*/

	printf("%.5f :-> -0.00000\n",-4.9406565584124654e-300);
	printf("%e :-> -1.234568e+11\n", -123456789999.4);
	printf("%.5e :-> -4.94066e-300\n",-4.9406565584124654e-300);
	printf("%f :-> -123456789999.400000\n", -123456789999.4);
	printf("%.5f :-> -0.00000\n",-4.9406565584124654e-300);
}

void test3(void)
{
	printf("---------- test3\n");
	printf("%.0f :-> 0\n",0.0);
	printf("%.0f :-> 13\n",12.545);
	printf("%.0f :-> 0\n",.099999999860301614);
	printf("%.0f :-> 1\n",.999999999860301614);
	printf("%.0e :-> 0e+00\n",0.0);
	printf("%.0e :-> 1e+11\n", 123456789999.4);
	printf("%.0e :-> 2e+11\n", 153456789999.4);
	printf("%.0f :-> -0\n",-4.9406565584124654e-300);
	printf("%.0f :-> -123456790000\n", -123456789999.9);
	printf("%.0e :-> -1e+11\n", -123456789999.4);
	printf("%.0e :-> -5e-300\n",-4.9406565584124654e-300);
}

void test4(void)
{
	printf("---------- test4a\n");

	printf("%g :-> 0\n", 0.0);
	printf("%g :-> 10\n", 10.0);
	printf("%g :-> 0.02\n", 0.02);
	printf("%g :-> 1.234578e+06\n", 1234567.8);
	printf("%g :-> 123.456\n", 123.456);
	printf("%G :-> 1E+06\n", 1000000.0);

	printf("---------- test4b\n");

	printf("%.0g :-> 0e+00\n", 0.0);
	printf("%.0g :-> 1e+01\n", 10.0);
	printf("%.0g :-> 0\n", 0.02);
	printf("%.0g :-> 1e+06\n", 1234567.8);
	printf("%.0G :-> -2E+02\n", -153.456);
	printf("%.0g :-> 1e+06\n", 1000000.0);

	printf("---------- test4c\n");

	printf("%#.0g :-> 0.0e+00\n", 0.0);
	printf("%#.0g :-> 1.0e+01\n", 10.0);
	printf("%#.0g :-> 0.0\n", 0.02);
	printf("%#.0g :-> 1.2e+06\n", 1234567.8);
	printf("%#+.0G :-> +1.5E+02\n", 153.456);
	printf("%#.0g :-> 1.0e+06\n", 1000000.0);

}

void test5(void)
{
	char *str = "-----------------------------------";
	printf("\n---------------test5\n");
	printf("|%*.*s|\n",20,20,str);

	printf("|%!*g|\n",20,10.0);
	printf("|%!*.*f|\n",20,2,1.2);
	printf("|%!*.*f|\n",20,2,12.3);
	printf("|%!*.*f|\n",20,2,1234567.8);
	printf("|%!*.*f|\n",20,2,123456789012.9);
	printf("|%!*g|\n",20,12345.67);
	printf("|%!*G|\n",20,153.456);
	printf("|%!*g|\n",20,1000000.0);
	printf("|%#!*g|\n",20,-10.0);
	printf("|%!*.*f|\n",20,2,-1.2);
	printf("|%!*.*f|\n",20,2,-12.3);
	printf("|%!*.*f|\n",20,2,-1234567.8);
	printf("|%!*.*f|\n",20,2,-123456789012.9);
	printf("|%!*g|\n",20, -12345.67);
	printf("|%!*G|\n",20, -153.456);
	printf("|%#!*.*g|\n",20,2, -1000000.0);
	printf("|%+!*.*f|\n",20,2,1.2);
	printf("|%+!*.*f|\n",20,2,12.3);
	printf("|%+!*.*f|\n",20,2,1234567.8);
	printf("|%+!*.*f|\n",20,2,123456789012.9);

	printf("|%*.*s|\n",20,20,str);
}

int main(void)
{
    test1();
    test2();
    test3();
    test4();
    test5();
    puts("<fini>\n");
    return 0;
}

#ifdef EiCTeStS
main();
#endif







