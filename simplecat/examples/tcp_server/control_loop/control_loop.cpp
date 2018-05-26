/****************************************************************************/

#include <simplecat/Master.h>
#include <simplecat/Beckhoff/Beckhoff.h>

#include "../redisKeys.h"
#include <redox.hpp>

#include <iostream>

simplecat::Master master;
simplecat::Beckhoff_EK1100 bh_ek1100;
simplecat::Beckhoff_EL3104 bh_el3104;
simplecat::Beckhoff_EL4134 bh_el4134;

unsigned int control_frequency = 1000; // Hz

redox::Redox tcp_client;

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
    //non-blocking write of values to tcp server
    tcp_client.command<std::string>({"SET", rkey::analog_out,
                                      std::string((const char*)bh_el4134.write_data_, sizeof(bh_el4134.write_data_))}, NULL);
    tcp_client.command<std::string>({"SET", rkey::analog_in,
                                      std::string((const char*)bh_el3104.read_data_, sizeof(bh_el3104.read_data_))}, NULL);


    if (loop_counter%500==0){

        // increasing analog output by 2.5 V
        for (int i=0; i<4; ++i){
            if (bh_el4134.write_data_[i]+8000 > 32000){
                bh_el4134.write_data_[i] = 0;
            } else {
                bh_el4134.write_data_[i] += 8000;
            }
        }

        std::cout << "analog write: ["
                  << bh_el4134.write_data_[0] << ", "
                  << bh_el4134.write_data_[1] << ", "
                  << bh_el4134.write_data_[2] << ", "
                  << bh_el4134.write_data_[3] << "]\n";
    }


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
    bool success = tcp_client.connect("localhost", 6379);
    if (!success){ std::cout << "ERROR. Failed to connect to redis server.\n"; }

    // stagger the values between 2.5V and 10V approximately.
    bh_el4134.write_data_[0] =  8000;
    bh_el4134.write_data_[1] = 16000;
    bh_el4134.write_data_[2] = 24000;
    bh_el4134.write_data_[3] = 32000;

    std::cout << "initial values\n";
    std::cout << bh_el4134.write_data_[0] << '\n';
    std::cout << bh_el4134.write_data_[1] << '\n';
    std::cout << bh_el4134.write_data_[2] << '\n';
    std::cout << bh_el4134.write_data_[3] << '\n';

    master.setCtrlCHandler(ctrl_c_handler);

    master.addSlave(0,0,&bh_ek1100);
    master.addSlave(0,1,&bh_el3104);
    master.addSlave(0,2,&bh_el4134);

    master.setThreadHighPriority();
    //master.setThreadRealTime();

    master.activate();

    master.run(control_callback, control_frequency);

    tcp_client.disconnect();

    std::cout << "run time : " << master.elapsedTime() << std::endl;
    std::cout << "updates  : " << master.elapsedCycles() << std::endl;
    std::cout << "frequency: " << master.elapsedCycles()/master.elapsedTime() << std::endl;

    return 0;
}

/****************************************************************************/

