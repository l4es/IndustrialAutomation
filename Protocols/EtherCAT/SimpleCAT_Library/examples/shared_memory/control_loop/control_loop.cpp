/****************************************************************************/

#include <simplecat/Master.h>
#include <simplecat/Beckhoff/Beckhoff.h>

#include "../sharedMemory.h"
#include "../sharedMemoryStruct.h"

#include <iostream>

simplecat::Master master;
simplecat::Beckhoff_EK1100 bh_ek1100;
simplecat::Beckhoff_EL3104 bh_el3104;
simplecat::Beckhoff_EL4134 bh_el4134;

unsigned int control_frequency = 1000; // Hz

SharedMemoryStruct *shared_data;
//#define boostip boost::interprocess

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
    // optionally lock the memory to ensure no one reads.
    // all shared memory readers (e.g. data_logger) must also lock
    // for this to be thread safe.
    SIMPLECAT_MUTEX lock(shared_data->mutex, boost::interprocess::try_to_lock);
    for (int i=0; i<4; ++ i){
        shared_data->analog_out_value[i] = bh_el4134.write_data_[i];
    }
    for (int i=0; i<4; ++ i){
        shared_data->analog_in_value[i] = bh_el3104.read_data_[i];
    }
    if (lock){
        lock.unlock();
    } else {
        std::cout << "failed to lock\n";
    }

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
    // create shared memory object
    simplecat::SharedMemory<SharedMemoryStruct> shm;
    shared_data = shm.createSharedMemory("MySharedMemory");

    // stagger the values between 2.5V and 10V approximately.
    bh_el4134.write_data_[0] =  8000;
    bh_el4134.write_data_[1] = 16000;
    bh_el4134.write_data_[2] = 24000;
    bh_el4134.write_data_[3] = 32000;

    master.setCtrlCHandler(ctrl_c_handler);

    master.addSlave(0,0,&bh_ek1100);
    master.addSlave(0,1,&bh_el3104);
    master.addSlave(0,2,&bh_el4134);

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

