#include "stdio.h"

void testsscanf(void)
{
    char * str = "10.5 15 hello";
    int d;
    char buf[50];
    float fval;
    sscanf(str,"%f %d %s",&fval,&d,buf);
    printf("got -> %g %d %s\n",fval,d,buf); 
}

void testscanf(void)
{
    float fval;
    
    long lval;
    int i,k;
    char buf[20];

    printf("Input a char set:\n");
    scanf("%[abcdefghijklmnopqrstuvwzyz] %*[^\n]",buf);
    printf("got -> %s\n",buf);


    printf("Input a float:\n");
    scanf("%f",&fval);
    printf("got -> %f\n",fval);

    printf("Input int and string:\n");
    scanf("%d %[abcdefghijklmno]", &i, buf);
    printf("got -> %d [%s]\n",i, buf);

    printf("Input long:\n");
    scanf("%ld", &lval);
    printf("got -> %ld\n",lval);

    printf("Input  an int with more than 2 digits:\n");
    scanf("%2d %d ", &i, &k);
    printf("got -> %d  %d\n",i,k);

}



void testfscanf(void)
{
    FILE *fp;
    int count; float quant; char units[21], item[21];
    
    fp = fopen("/tmp/xxeicxx","w+");
    fputs("2 quarts of oil\n"
	  "-12.8 degrees Celsius\n"
	  "lots of luck\n"
	  "10.0LBS       of\n"
	  "dirt\n"
	  "100ergs of energy",
	  fp);
    fseek(fp,0L,SEEK_SET);

    while(!feof(fp)) {
	units[0] = item[0] ='\0';
	count = fscanf(fp,"%f%20s of %20s",&quant, units, item);
	fscanf(fp,"%*[^\n]");
	fprintf(stdout,"count = [%d]  quant = [%g] units = [%s] item = [%s]\n",
	       count,quant,units,item);
    }
    fclose(fp);
}
    

int main(void)
{
    testfscanf();
/*    testscanf();*/
    testsscanf();

    return 0;
    
}





