#include "stdio.h"
#include "string.h"
#include "fcntl.h"

void test7(void)
{
    FILE * fp;
    int len;
    char cmd[50]; 
    
    fp = popen("/usr/bin/gnuplot","w");
    if (fp == NULL) {
	fprintf(stderr,"Problems\n");
	return;
    }

    while(1) {
	printf("Input command:>");
	fflush(stdout);
	gets(cmd);
	printf("comand is %s\n",cmd);
	if((len = strlen(cmd)) > 1) {
	    fputs(cmd,fp);
	    fputc('\n',fp);
	    fflush(fp);
	} else
	    break;
    }
    printf(" close status = %d\n",pclose(fp));
}



void test6(void)
{

    FILE *fp;
    int count; float quant; char units[21], item[21];
    int i;
    
    fp = fopen("/tmp/xxeicxx","w+");
    fputs("2 quarts of oil\n"
	  "-12.8 degrees Celsius\n"
	  "lots of luck\n"
	  "10.0LBS       of\n"
	  "dirt\n"
	  "100ergs of energy",
	  fp);
    fseek(fp,0L,SEEK_SET);
    i = 0;
    while(!feof(fp) && i < 10) {
	units[0] = item[0] ='\0';
	count = fscanf(fp,"%f%20s of %20s",&quant, units, item);
	fscanf(fp,"%*[^\n]");
	printf("count = [%d]  quant = [%g] units = [%s] item = [%s]\n",
	       count,quant,units,item);
       ++i;
    }
    fclose(fp);

}



void test5(void)
{
    fprintf(stdout,"%s = %b\n","binary 100", 100);
    fprintf(stdout,"|%!15.2f|\n", 13.3333);
}

void test4(void)
{
    int i;
    char *buf = "Hello world Hello world Hello world Hello world\n";
    for(i=0;buf[i] != '\0'; ++i)
	fputc(buf[i],stdout);

    fputs(buf,stdout);
    
}

int main(void)
{
    test6();
    test5();
    test4();
    return 0;
}








