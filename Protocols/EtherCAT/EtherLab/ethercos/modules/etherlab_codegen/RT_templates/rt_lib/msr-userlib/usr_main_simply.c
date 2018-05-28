#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "rt_lib/include/msr.h"
#include "rt_lib/include/msr_reg.h"
#include "rt_lib/include/msr_main.h"


//Beispiel für Nutzung von rt_lib im Userspace

//Parameter



int main()
 {


     void *dev;
     int i;
     int base_rate = 100000; //10Hz
     double process_time_dbl = 0.0;
     //Registrieren
     msr_rtlib_init(1,1.0e6/(double)base_rate,10,NULL);
     msr_reg_kanal("/Time","s",&process_time_dbl,TDBL);
     /*msr_reg_kanal("/Time/sec","s",&process_time.tv_sec,TULINT);
     msr_reg_kanal("/Time/usec","us",&process_time.tv_usec,TULINT);
     msr_reg_kanal("/Linuxtime","s",&msr_time_sync.dlinux_time,TDBL);
     msr_reg_kanal("/Trigger","",&msr_trigger,TINT);*/ 
     
     /* dann die Taskinfo */
     /*msr_reg_kanal("/Taskinfo/Jiffies","",(void *)&msr_jiffies,TULINT);
     msr_reg_kanal("/Taskinfo/Controller_Execution_Time","us",&msr_controller_execution_time,TUINT);
     msr_reg_kanal("/Taskinfo/Controller_Call_Time","us",&msr_controller_call_time,TUINT); */



    //Lesen
    dev = msr_open(STDIN_FILENO,STDOUT_FILENO);


    for(i=0;i<10000;i++) {
      process_time_dbl+=0.1;
      msr_read(dev);
      msr_write_kanal_list(); 
      msr_write(dev);
    }; 

    msr_close(dev);

    msr_cleanup();

    return 0;
}
