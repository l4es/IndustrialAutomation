#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


char *strs[] = {
    "\n",
    "A\n",
    "this is myfirst string\n",
    "string 2nd\n",
    "3rd string which is much longer than the file buffer\n",
    "0123456789\n",
};    
	
		     
void test()
{
    long pos;
    char buf[50];
    char str[100];
    char *fn = "/tmp/_EdsTmPFiLe";
    int i,n;
    FILE *fp;

    assert((fp = tmpfile()) != NULL);
    assert(fprintf(fp,"12345") == 5);
    
    assert(fseek(fp,0,SEEK_END) == 0);
    assert(ftell(fp) == 5);
    assert(fseek(fp,-2,SEEK_CUR) == 0);
    assert(ftell(fp) == 3);
    assert(fseek(fp,0,SEEK_SET) == 0);
    assert(ftell(fp) == 0);
    assert(fclose(fp) == 0);

    assert((fp = fopen(fn,"wb+")) != NULL);
    assert(fp != NULL);
    assert(fclose(fp) == 0);
    assert(remove(fn) == 0 && fopen(fn,"r") == NULL);
    
    assert((fp = fopen(fn,"wb+")) != NULL);
    assert(setvbuf(fp,buf,_IOLBF,10) == 0);
    
    for(i=0;i<10;i++)
	assert(fprintf(fp,"%d",i) == 1);

    assert(fseek(fp,3,SEEK_SET) == 0);
    assert(fread(str,1,4,fp) == 4 && str[0] == '3' && str[3] == '6');

    assert(fseek(fp,3,SEEK_SET) == 0);

    assert(fwrite("6543",1,4,fp) == 4);
    assert(fseek(fp,3,SEEK_SET) == 0);
    assert(fread(str,1,4,fp) == 4 && str[0] == '6' && str[3] == '3');
    assert(fseek(fp,0,SEEK_END) == 0);
    assert(ftell(fp) == 10);
    assert(fwrite("12345",1,5,fp) == 5);
    assert(ftell(fp) == 15);
    assert(fseek(fp,-7,SEEK_CUR) == 0);
    
    assert(fgetc(fp) == '8' && fgetc(fp) == '9');

    
    n = sizeof(strs)/sizeof(char*);
    pos = ftell(fp);
    fseek(fp,pos,SEEK_SET);
    for(i=0;i<n;i++) 
	assert(fputs(strs[i],fp) >= 0);

    assert(fseek(fp,pos + 1,SEEK_SET) == 0);
    for(i=1;i<n;i++) {
	fgets(str,100,fp);
	assert(strncmp(str,strs[i],10) == 0);
    }
	
    fclose(fp);
    assert(remove(fn) == 0 && fopen(fn,"r") == NULL);

    return;
}


/*
*/

int main(void)
{
    test();
    return 0;
}

#ifdef EiCTeStS
main();
#endif




