/****************************************************************************/

#include <iostream>

#include <simplecat/Master.h>
#include <simplecat/Beckhoff/Beckhoff.h>

simplecat::Master master;
simplecat::Beckhoff_EK1100 bh_ek1100;
simplecat::Beckhoff_EL5101 bh_el5101;

unsigned int control_frequency = 1000; // Hz

/****************************************************************************/

void ctrl_c_handler(int s)
{
    std::cout << "exiting" << std::endl;
    master.stop();
}

/****************************************************************************/

unsigned int loop_counter = 0;
void control_callback()
{
    if (loop_counter%1000==0){
        std::cout << "encoder counter: ["
                  << bh_el5101.read_value << "]\n";
    }
    ++loop_counter;
}

/****************************************************************************/

int main(int argc, char **argv)
{
    master.setCtrlCHandler(ctrl_c_handler);

    master.addSlave(0,0,&bh_ek1100);
    master.addSlave(0,1,&bh_el5101);

    master.setThreadHighPriority();
    //master.setThreadRealTime();

    master.activate();

    master.run(control_callback, control_frequency);


    std::cout << "run time : " << master.elapsedTime() << std::endl;
    std::cout << "updates  : " << master.elapsedCycles() << std::endl;
    std::cout << "frequency: " << master.elapsedCycles()/master.elapsedTime() << std::endl;

    return 0;
}

/****************************************************************************/
