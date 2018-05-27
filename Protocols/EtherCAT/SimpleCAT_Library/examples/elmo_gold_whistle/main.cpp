/****************************************************************************/

#include <math.h>

#include <iostream>

#include <simplecat/Master.h>
#include <simplecat/Elmo/Elmo.h>

simplecat::Master master;
simplecat::Elmo_GoldWhistle elmo_gw;

unsigned int control_frequency = 1000; // Hz

/****************************************************************************/

void ctrl_c_handler(int s)
{
    std::cout << "exiting" << std::endl;
    master.stop();
}

/****************************************************************************/

const int encoder_resolution = 1024;
bool initialized = false;
int axis1_pos_offset = 0;
int axis1_last_pos = 0;
int axis2_pos_offset = 0;
int axis2_last_pos = 0;

unsigned int loop_counter = 0;
void control_callback()
{
    // Intialize encoder offsets
    if (initialized==false && elmo_gw.initialized()){
       axis1_pos_offset = elmo_gw.position_;
       axis1_last_pos = 0;
       axis2_pos_offset = elmo_gw.absolute_position_;
       axis2_last_pos = 0;
       elmo_gw.max_torque_ = 1000;
       initialized = true;

       std::cout << "================\n";
       std::cout << "initial position: " << elmo_gw.position_ << '\n';
    }

    // Zeroed positions and velocities
    int axis1_pos = elmo_gw.position_          - axis1_pos_offset;
    int axis2_pos = elmo_gw.absolute_position_ - axis2_pos_offset;

    int axis1_vel = axis1_pos - axis1_last_pos;
    int axis2_vel = axis2_pos - axis2_last_pos;

    axis1_last_pos = axis1_pos;
    axis2_last_pos = axis2_pos;

    // Torque command
    elmo_gw.target_torque_ = 0;

    // print
    if (loop_counter%500==0)
    {
        std::cout << std::dec;
        std::cout << "axis 1  position : " << axis1_pos << '\n';
        std::cout << "axis 2  position : " << axis2_pos << '\n';
        std::cout << "axis 1  velocity : " << axis1_vel << '\n';
        std::cout << "axis 2  velocity : " << axis2_vel << '\n';
        std::cout << "maximum torque   : " << elmo_gw.max_torque_ << '\n';
        std::cout << "desired torque   : " << elmo_gw.target_torque_ << '\n';
        std::cout << "actual  torque   : " << elmo_gw.torque_ << '\n';
        for (int i=0; i<6; ++i){
            std::cout << "digital input " << i << ": " << elmo_gw.digital_inputs_[i] << '\n';
        }
    }

    ++loop_counter;
}

/****************************************************************************/

int main(int argc, char **argv)
{
    master.setCtrlCHandler(ctrl_c_handler);

    master.addSlave(0,0,&elmo_gw);

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


