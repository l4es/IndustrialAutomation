
#include <stdio.h>

void alignment ()
{

    union {
	short int s;
	char  a[2];
    } endian;
    
    struct{
	char cdummy;
	char c;
    } sc;

    struct{
	char cdummy;
	short s;
    } ss;

    struct{
	char cdummy;
	int i;
    } si;

    struct{
	char cdummy;
	long l;
    } sl;

    struct{
	char cdummy;
	unsigned u;
    } su;

    struct{
	char cdummy;
	float f;
    } sf;

    struct{
	char cdummy;
	double d;
    } sd;

    struct{
	char cdummy;
	void * p;
    } sp;

    struct{
	char cdummy;
	char * cp;
    } scp;

    struct{
	char cdummy;
	short * sp;
    } ssp;

    struct{
	char cdummy;
	long * lp;
    } slp;

    struct{
	char cdummy;
	float * fp;
    } sfp;

    struct{
	char cdummy;
	double * dp;
    } sdp;

    
    struct{
	char cdummy;
	int * p;
    } sip;

    struct{
	char cdummy;
	long long ll;
    } sll;

    int diff[20];
   
    diff[0] = &sc.c - &sc.cdummy;
    diff[1] = (char *)&ss.s - &ss.cdummy;
    diff[2] = (char *)&si.i - &si.cdummy;
    diff[3] = (char *)&sl.l - &sl.cdummy;
    diff[4] = (char *)&su.u - &su.cdummy;
    diff[5] = (char *)&sf.f - &sf.cdummy;
    diff[6] = (char *)&sd.d - &sd.cdummy;
    diff[7] = (char *)&sp.p - &sp.cdummy;
    diff[8] = (char *)&sip.p - &sip.cdummy;
    diff[9] = (char *)&scp.cp - &scp.cdummy;

    diff[10] = (char *)&ssp.sp - &ssp.cdummy;
    diff[11] = (char *)&slp.lp - &slp.cdummy;
    diff[12] = (char *)&sfp.fp - &sfp.cdummy;
    diff[13] = (char *)&sdp.dp - &sdp.cdummy;

    diff[14] = (char *)&sll.ll - &sll.cdummy;

    endian.s = 1;
    printf("#define EIC_LITTLE_ENDIAN %d\n",endian.a[0]); 
    printf("{%d, %d}, /* %s */\n",(int)sizeof(char), diff[0],"char");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(short), diff[1],"short");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(int), diff[2],"int");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(long), diff[3],"long");

    printf("{%d, %d}, /* %s */\n",(int)sizeof(long long), diff[14],"long long");

    printf("{%d, %d}, /* %s */\n",(int)sizeof(float), diff[5],"float");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(double), diff[6],"double");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(void *), diff[7],"void pointer");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(int *), diff[8]," int pointer");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(char *), diff[9]," char pointer");

    printf("{%d, %d}, /* %s */\n",(int)sizeof(short *), diff[10]," short pointer");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(long *), diff[11]," long pointer");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(float *), diff[12]," float pointer");
    printf("{%d, %d}, /* %s */\n",(int)sizeof(double *), diff[13]," double pointer");

    printf("{0, 1}, /* struct*/\n");
   
}

int main(void)
{
    alignment();
    return 0;
}
