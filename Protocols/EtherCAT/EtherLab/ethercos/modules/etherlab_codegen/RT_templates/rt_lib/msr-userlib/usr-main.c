#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "rt_lib/include/msr.h"
#include "rt_lib/include/msr_reg.h"
#include "rt_lib/include/msr_main.h"


#define   SS_DOUBLE   0
#define   SS_SINGLE   1
#define   SS_INT8     2
#define   SS_UINT8    3
#define   SS_INT16    4
#define   SS_UINT16   5
#define   SS_INT32    6
#define   SS_UINT32   7
#define   SS_BOOLEAN  8

//Beispiel für Nutzung von rt_lib im Userspace

//Parameter

int p1=3;
double p2=2.0;

double p3[10]={1,2,3,4,5,6,7,8,9,0};


int en[5];

//Kanäle

struct rtp_struct {
    //  int a;
    int k1;
    double k2;
    double k3[5];
};

struct rtp_struct rtp;






int main()
 {


     void *dev;
     int cnt;
     int i,j;
     //Registrieren
    msr_init(&rtp,NULL);

    msr_reg_rtw_param("/daten/p1(ll=\"1\" ul=\"2\")","gain","int",&p1,1,1,SS_INT32,var_SCALAR,sizeof(int));
    msr_reg_rtw_param("/daten/p2(init=\"0.5\" ll=\"1\" ul=\"2\")","gain","double",&p2,1,1,SS_DOUBLE,var_SCALAR,sizeof(double));
    msr_reg_rtw_param("/daten/p3(unit=\"s\")","gain","double",&p3,1,10,SS_DOUBLE,var_VECTOR,sizeof(double));

    msr_reg_rtw_signal("/kanal/k1","","int",(void *)&rtp.k1 - (void *)&rtp,1,1,SS_INT32,var_SCALAR,sizeof(int));

    msr_reg_rtw_signal("/kanal/k2","","int",(void *)&rtp.k2 - (void *)&rtp,1,1,SS_DOUBLE,var_SCALAR,sizeof(double));

    msr_reg_rtw_signal("/kanal/k3","","int",(void *)&rtp.k3[0] - (void *)&rtp,1,5,SS_DOUBLE,var_VECTOR,sizeof(double));

    msr_reg_enum_list("/Aufzaehlung","",&en[0],MSR_R |MSR_W,5,1,"Eins,Zwei,Drei",NULL,NULL);

    //Kanäle füllen
    for(i=0;i<1000;i++) {
//	printf("index %i\n",i);
	rtp.k1 = i;
	rtp.k2 = i*0.3;
	for(j=0;j<5;j++)
	    rtp.k3[j] = j*i;
	msr_update(&rtp);
    }

    //Lesen
    dev = msr_open(STDIN_FILENO,STDOUT_FILENO);

    msr_read(dev);
    do {
	cnt = msr_write(dev);
//	printf("Write count: %i\n",cnt);
    } while(cnt>0); 

    msr_close(dev);

    msr_cleanup();

    return 0;
}
