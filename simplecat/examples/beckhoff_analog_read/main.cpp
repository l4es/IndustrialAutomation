/****************************************************************************/

#include <iostream>

#include <simplecat/Master.h>
#include <simplecat/Beckhoff/Beckhoff.h>

simplecat::Master master;
simplecat::Beckhoff_EK1100 bh_ek1100;
simplecat::Beckhoff_EL3104 bh_el3104;

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


    if (loop_counter %550==0){

        // read analog input values
        std::cout << "analog read : ["
                  << bh_el3104.read_data_[0] << ", "
                  << bh_el3104.read_data_[1] << ", "
                  << bh_el3104.read_data_[2] << ", "
                  << bh_el3104.read_data_[3] << "]\n";
    }

    ++loop_counter;
}

/****************************************************************************/

int main(int argc, char **argv)
{
    master.setCtrlCHandler(ctrl_c_handler);

    master.addSlave(0,0,&bh_ek1100);
    master.addSlave(0,1,&bh_el3104);

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
