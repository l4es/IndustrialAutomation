#include <stdio.h>
#include <string.h>
#include <assert.h>


void T1()
{
    int in = 555;
    char buf[10];

    sprintf(buf, "|%%%n |",&in);
    assert(in == 2);
    in = 555;
    sscanf(buf, "|%%%n", &in);
    assert(in == 2);
}

void testsscanf(void)
{
    char * str = "10.5 hello";
    char buf[50];
    float fval;
    sscanf(str,"%f %s",&fval,buf);
    assert(fval == 10.5);
    assert(strcmp("hello",buf) == 0);

}

void testfscanf(void)
{
    FILE *fp;
    int count; float quant; char units[21], item[21];
    char * tfile = "xx";
    
    fp = fopen(tfile,"w+");
	
    if(!fp){
	printf("file open fail\n");
	return;	
    } else
	printf("Okay 1\n");
	
    fputs("2 quarts of oil\n"
	  "-12.8 degrees Celsius\n"
	  "lots of luck\n"
	  "10.0LBS       of\n"
	  "dirt\n"
	  "100ergs of energy",
	  fp);
    fseek(fp,0L,SEEK_SET);

    printf("Okay 1\n");
    while(!feof(fp)) {
	units[0] = item[0] ='\0';
	count = fscanf(fp,"%f%20s of %20s",&quant, units, item);
	fscanf(fp,"%*[^\n]");
	printf("count = [%d]  quant = [%g] units = [%s] item = [%s]\n",
	       count,quant,units,item);
    }
    fclose(fp);
    remove(tfile);
}

int  main(void)
{
    testsscanf();
    testfscanf();
    T1();
    return 0;
}

#ifdef EiCTeStS
main();
/*"\nTo clean  :rm testscanf,testsscanf,testfscanf\n";*/
#endif






    








