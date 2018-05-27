/****************************************************************************/

#include <simplecat/Master.h>

#include "../redisKeys.h"
#include <redox.hpp>

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <cstring>

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
    redox::Redox tcp_client;
    bool success = tcp_client.connect("localhost", 6379);
    if (!success){ std::cout << "ERROR. Failed to connect to redis server.\n"; }

    // ctrl c handler
    simplecat::Master::setCtrlCHandler(ctrl_c_handler);

    // open file
    std::cout << "opening file" << file_name << std::endl; 
    myfile.open(file_name);

    int16_t analog_out_value[4] = {0};
    int16_t analog_in_value[4] = {0};

    running = true;
    while(running)
    {
        //blocking read of values from tcp server
        auto& co = tcp_client.commandSync<std::string>({"GET", rkey::analog_out});
        if(co.ok()) {
            std::string value = co.reply();
            if (value.size()==sizeof(analog_out_value)){
                std::memcpy(&analog_out_value[0], value.c_str(), sizeof(analog_out_value));
            } else {
                std::cout << "ERROR. " << rkey::analog_out << " size is incorrect.\n";
            }
        } else {std::cout << "Failed to get key! Status: " << co.status() << '\n'; }
        co.free();

        auto& ci = tcp_client.commandSync<std::string>({"GET", rkey::analog_in});
        if(ci.ok()) {
            std::string value = ci.reply();
            if (value.size()==sizeof(analog_in_value)){
                std::memcpy(&analog_in_value[0], value.c_str(), sizeof(analog_in_value));
            } else {
                std::cout << "ERROR. " << rkey::analog_in << " size is incorrect.\n";
            }
        } else {std::cout << "Failed to get key! Status: " << ci.status() << '\n'; }
        ci.free();


        // write data to file
        std::cout << "writing" << '\n';
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

    tcp_client.disconnect();

    std::cout << "closing file " << file_name << std::endl;;
    myfile.close();
    return 0;
}
