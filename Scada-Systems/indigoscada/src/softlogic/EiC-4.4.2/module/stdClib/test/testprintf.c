#include "stdio.h"
#include "stdlib.h"

#include "ctype.h"
#include "limits.h"
#include "float.h"
#include "math.h"

#define eprintf printf 

void test1(void)
{
	char *str = "Edmond";
	char *str2 = "Claire Breen";
	int d = 365;
	double f1 = 12.567, f2 = 5666.56789;
	double f3 = 999.99999, f4 = 899.9999;
	eprintf("\n---------------test1\n");
	eprintf("|%10d|%|10x|%10X|%10o|\n",d,d,d,d);
	eprintf("|%+10d|%-+10d|%0+10d|%+10d|\n",d,d,d,-d);
	eprintf("|%#10x|%-#10X|%|10s|%|.10s|\n",d,d,str,str2);
	eprintf("|%021.0f|%!21.2f|\n",f1,f2);

	eprintf("|%021.2f|%!21.2f|\n",f3,f4);
}

void test2(void)
{
	eprintf("---------- test2\n");
	eprintf("%f :-> 0.000000\n",0.0);
	eprintf("%f :-> 1.000000\n",1.0);
	eprintf("%f :-> 10.000000\n",10.0);
	eprintf("%f :-> 12.345000\n",12.345);
	eprintf("%f :-> 1234567.800000\n",1234567.8);
	eprintf("%.2f :-> 1000.00\n",999.999);
	eprintf("%.2f :-> 999.99\n",999.985999);
	eprintf("%f :-> 0.100000\n",.099999999860301614);
	eprintf("%f :-> 1.000000\n",.999999999860301614);
	eprintf("%e :-> 0.000000e+00\n",0.0);
	eprintf("%e :-> 1.000000e+00\n",1.0);
	eprintf("%e :-> 1.000000e+01\n",10.0);
	eprintf("%e :-> 1.234568e+11\n", 123456789999.4);
	eprintf("%e :-> 1.234568e+06\n", 1234567.8);
	eprintf("%E :-> 1.234568E+06\n", 1234567.8);
	eprintf("%.5e :-> 4.94066e-300\n",4.9406565584124654e-300);
	/*
	   does not handle long doubles
	eprintf("%Lf :-> -123456789999.400000\n", -123456789999.4L);
	*/

	eprintf("%.5f :-> -0.00000\n",-4.9406565584124654e-300);
	eprintf("%e :-> -1.234568e+11\n", -123456789999.4);
	eprintf("%.5e :-> -4.94066e-300\n",-4.9406565584124654e-300);
	eprintf("%f :-> -123456789999.400000\n", -123456789999.4);
	eprintf("%.5f :-> -0.00000\n",-4.9406565584124654e-300);
}

void test3(void)
{
	eprintf("---------- test3\n");
	eprintf("%.0f :-> 0\n",0.0);
	eprintf("%.0f :-> 13\n",12.545);
	eprintf("%.0f :-> 0\n",.099999999860301614);
	eprintf("%.0f :-> 1\n",.999999999860301614);
	eprintf("%.0e :-> 0e+00\n",0.0);
	eprintf("%.0e :-> 1e+11\n", 123456789999.4);
	eprintf("%.0e :-> 2e+11\n", 153456789999.4);
	eprintf("%.0f :-> -0\n",-4.9406565584124654e-300);
	eprintf("%.0f :-> -123456790000\n", -123456789999.9);
	eprintf("%.0e :-> -1e+11\n", -123456789999.4);
	eprintf("%.0e :-> -5e-300\n",-4.9406565584124654e-300);
}

void test4(void)
{
	eprintf("---------- test4a\n");

	eprintf("%g :-> 0\n", 0.0);
	eprintf("%g :-> 10\n", 10.0);
	eprintf("%g :-> 0.02\n", 0.02);
	eprintf("%g :-> 1.234578e+06\n", 1234567.8);
	eprintf("%g :-> 123.456\n", 123.456);
	eprintf("%G :-> 1E+06\n", 1000000.0);

	eprintf("---------- test4b\n");

	eprintf("%.0g :-> 0e+00\n", 0.0);
	eprintf("%.0g :-> 1e+01\n", 10.0);
	eprintf("%.0g :-> 0\n", 0.02);
	eprintf("%.0g :-> 1e+06\n", 1234567.8);
	eprintf("%.0G :-> -2E+02\n", -153.456);
	eprintf("%.0g :-> 1e+06\n", 1000000.0);

	eprintf("---------- test4c\n");

	eprintf("%#.0g :-> 0.0e+00\n", 0.0);
	eprintf("%#.0g :-> 1.0e+01\n", 10.0);
	eprintf("%#.0g :-> 0.0\n", 0.02);
	eprintf("%#.0g :-> 1.2e+06\n", 1234567.8);
	eprintf("%#+.0G :-> +1.5E+02\n", 153.456);
	eprintf("%#.0g :-> 1.0e+06\n", 1000000.0);

}

void test5(void)
{
	char *str = "-----------------------------------";
	eprintf("\n---------------test5\n");
	eprintf("|%*.*s|\n",20,20,str);

	eprintf("|%!*g|\n",20,10.0);
	eprintf("|%!*.*f|\n",20,2,1.2);
	eprintf("|%!*.*f|\n",20,2,12.3);
	eprintf("|%!*.*f|\n",20,2,1234567.8);
	eprintf("|%!*.*f|\n",20,2,123456789012.9);
	eprintf("|%!*g|\n",20,12345.67);
	eprintf("|%!*G|\n",20,153.456);
	eprintf("|%!*g|\n",20,1000000.0);
	eprintf("|%#!*g|\n",20,-10.0);
	eprintf("|%!*.*f|\n",20,2,-1.2);
	eprintf("|%!*.*f|\n",20,2,-12.3);
	eprintf("|%!*.*f|\n",20,2,-1234567.8);
	eprintf("|%!*.*f|\n",20,2,-123456789012.9);
	eprintf("|%!*g|\n",20, -12345.67);
	eprintf("|%!*G|\n",20, -153.456);
	eprintf("|%#!*.*g|\n",20,2, -1000000.0);
	eprintf("|%+!*.*f|\n",20,2,1.2);
	eprintf("|%+!*.*f|\n",20,2,12.3);
	eprintf("|%+!*.*f|\n",20,2,1234567.8);
	eprintf("|%+!*.*f|\n",20,2,123456789012.9);

	eprintf("|%*.*s|\n",20,20,str);
}

void test6(void)
{
	eprintf("\n-----------------test6\n");
	eprintf("|%!*b|\n",20,20);
	eprintf("|%!*c|\n",20,'E');
	eprintf("|%|c|\n",'E');
	eprintf("|%d%d%d%d%d%d\n",1,2,3,4,5,6);
	eprintf("|%c%c%c%c%c%c|\n",'e','d','m','o','n','d');
	eprintf("|%a%%%v|\n");
	eprintf("|%.*s|\n",20,"--------------------------------------");
}

int main(void)
{
	test1();
	eprintf("<push CR to continue>");
	getchar();
	test2();
	eprintf("<push CR to continue>");
	getchar();
	test3();
	eprintf("<push CR to continue>");
	getchar();
	test4();
	eprintf("<push CR to continue>");
	getchar();
	test5();
	eprintf("<push CR to continue>");
	getchar();
	test6();
	eprintf("<push CR to continue>");
	getchar();

	eprintf("<fini>\n");
	
	return 0;
}











