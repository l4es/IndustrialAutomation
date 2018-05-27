/****************************************************************************/

#include "../sharedMemory.h"
#include "../sharedMemoryStruct.h"
#include <simplecat/Master.h>

#include <iostream>
#include <fstream>

std::string file_name = "data.txt";
std::ofstream myfile;
bool running = false;

/****************************************************************************/

void ctrl_c_handler(int s)
{
    running = false;
}

/****************************************************************************/

int main()
{
    // open shared memory object
    simplecat::SharedMemory<SharedMemoryStruct> shm;
    SharedMemoryStruct* shared_data = shm.openSharedMemory("MySharedMemory");

    // ctrl c handler
    simplecat::Master::setCtrlCHandler(ctrl_c_handler);

    // open file
    std::cout << "opening file" << file_name << std::endl; 
    myfile.open(file_name);

    int16_t analog_out_value[4];
    int16_t analog_in_value[4];

    running = true;
    while(running)
    {
        std::cout << "writing" << std::endl;

        // optionally lock the memory to ensure thread safe reads.
        // all shared memory writers (e.g. control_loop) must also lock
        // for this to be thread safe.
        SIMPLECAT_MUTEX lock(shared_data->mutex);
        for (int i=0; i<4; ++ i){
            analog_out_value[i] = shared_data->analog_out_value[i];
        }
        for (int i=0; i<4; ++ i){
            analog_in_value[i] = shared_data->analog_in_value[i];
        }
        lock.unlock();

        // write data to file
        myfile << analog_out_value[0] << ", "
               << analog_out_value[1] << ", "
               << analog_out_value[2] << ", "
               << analog_out_value[3] << ", ";
        myfile << analog_in_value[0] << ", "
               << analog_in_value[1] << ", "
               << analog_in_value[2] << ", "
               << analog_in_value[3] << "\n";

        sleep(1);
    }

    std::cout << "closing file " << file_name << std::endl;;
    myfile.close();
    return 0;
}
