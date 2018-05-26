/****************************************************************************/

#include <math.h>

#include <iostream>

#include <simplecat/Master.h>
#include <simplecat/Technosoft/iPOS3604.h>

simplecat::Master master;
simplecat::Technosoft_iPOS3604 ts_ipos3604;

unsigned int control_frequency = 1000; // Hz

/****************************************************************************/

void ctrl_c_handler(int s)
{
    std::cout << "exiting" << std::endl;
    master.stop();
}

/****************************************************************************/

const int    sinusoid_amplitude = 1000;
const double sinusoid_freq = 0.1;

const double Kp = 2.0; // proportional gain
const double Kd = 2.8; // derivative gain
                       // critical unit mass damping Kd = 2*sqrt(Kp)

const int encoder_resolution = 512;
bool initialized = false;
int pos_offset = 0;
int last_pos = 0;

unsigned int loop_counter = 0;
void control_callback()
{
    // PD Control
    if (initialized==false && ts_ipos3604.initialized()){
       pos_offset = ts_ipos3604.position_;
       last_pos = 0;
       initialized = true;

       std::cout << "================\n";
       std::cout << "initial position: " << ts_ipos3604.position_ << '\n';
    }

    double phase = ((double)(loop_counter))/encoder_resolution;
    int desired_pos = sinusoid_amplitude*sin(2*M_PI*sinusoid_freq*phase);
    int zeroed_pos = (ts_ipos3604.position_-pos_offset);
    int est_vel = (zeroed_pos - last_pos);
    ts_ipos3604.target_torque_ = 0; //Kp*(desired_pos - zeroed_pos) - Kd*est_vel;
    last_pos = zeroed_pos;

    // print
    if (loop_counter%500==0)
    {
        std::cout << std::hex;
        std::cout << "status word      : " << ts_ipos3604.status_word_ << '\n';
        std::cout << std::dec;
        std::cout << "desired  position: " << desired_pos << '\n';
        std::cout << "zeroed   position: " << ts_ipos3604.position_-pos_offset << '\n';
        std::cout << "actual   position: " << ts_ipos3604.position_ << '\n';
        std::cout << "estimate velocity: " << est_vel << '\n';
        std::cout << "desired  torque  : " << ts_ipos3604.target_torque_ << '\n';
    }

    ++loop_counter;
}

/****************************************************************************/

int main(int argc, char **argv)
{
    master.setCtrlCHandler(ctrl_c_handler);

    ts_ipos3604.mode_of_operation_ = ts_ipos3604.MODE_CYCLIC_SYNC_TORQUE;

    master.addSlave(0,0,&ts_ipos3604);

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




