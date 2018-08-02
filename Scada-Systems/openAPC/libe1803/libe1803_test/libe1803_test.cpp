#include <stdio.h>
#include <stdlib.h>

#include "../libe1803.h"

#ifdef ENV_WINDOWS
 #include <windows.h>
#else
 #include <unistd.h>
#endif

static int cardNum=-1;


static void waitMotionEnd()
{
   // wait until motion has started
   while ((E1803_get_card_state(cardNum) & (E1803_CSTATE_MARKING|E1803_CSTATE_PROCESSING))==0)
   {
#ifdef ENV_WINDOWS
       Sleep(10);
#else
       usleep(10000);
#endif
   }
   // wait until motion is finished
   while ((E1803_get_card_state(cardNum) & (E1803_CSTATE_MARKING|E1803_CSTATE_PROCESSING))!=0)
   {
      double pos0,pos1,pos2,pos3;
#ifdef ENV_WINDOWS
      Sleep(500);
#else
      usleep(500000);
#endif
      E1803_motion_get_pos(cardNum,0,&pos0);
      E1803_motion_get_pos(cardNum,1,&pos1);
      E1803_motion_get_pos(cardNum,2,&pos2);
      E1803_motion_get_pos(cardNum,3,&pos3);

      printf("Axis pos: %.3f, %.3f, %.3f, %.3f\r\n",pos0,pos1,pos2,pos3);
   }
}


// set power for a CO2 laser in unit % using frequency of 20 kHz and maximum duty cycle of 50%
static int set_power(unsigned char n,double power,void *userData)
{
   double         maxDutyCycle=0.5;
   unsigned short halfPeriod;
   unsigned short pwrPeriod1;

   halfPeriod=(unsigned short)(((1.0/20000.0)/2.0)*1000000.0); // usec
   if (halfPeriod<2) halfPeriod=2;
   else if (halfPeriod>65500) halfPeriod=65500;

   pwrPeriod1=(unsigned short)(halfPeriod*2*(power/100.0)*maxDutyCycle);
   return E1803_set_laser_timing(n,20000,pwrPeriod1);
}


int main(int argc, char *argv[])
{
   int          ret;
#ifdef ENV_WINDOWS
   WSADATA      wsaData;

   WSAStartup(MAKEWORD(1,1),&wsaData);
#endif

   cardNum=E1803_set_connection("192.168.2.254");
   if (cardNum>0)
   {
//      E1803_set_password(cardNum,"123456"); // used only in case of Ethernet connection; requires the password "123456" to be set in card configuration file
      ret=E1803_load_correction(cardNum,"",0); // set correction file, for no/neutral correction use "" or NULL here
      if (ret==E1803_OK)
      {
          E1803_set_laser_mode(cardNum,E1803_LASERMODE_CO2);    // configure for CO2
          E1803_set_standby(cardNum,20000.0,5.0);               // 20kHz and 5 usec standby frequency/period
          E1803_set_laser_timing(cardNum,20000.0,25.0);         // 20kHz and 50% duty cycle marking frequency/period

         // perform a normal mark operation
          E1803_set_laser_mode(cardNum,E1803_LASERMODE_CO2); // configure for CO2
          E1803_set_standby(cardNum,20000.0,5.0);            // 20kHz and 5 usec standby frequency/period
          E1803_set_laser_timing(cardNum,20000.0,25.0);      // 20kHz and 50% duty cycle marking frequency/period

          E1803_set_scanner_delays(cardNum,0,100.0,100.0,10.0); // some delay values, have to be adjusted for used scanhead
          E1803_set_laser_delays(cardNum,20.0,30.0);          // laser on/off delay in microseconds, have to be adjusted for used scan-system
          E1803_set_speeds(cardNum,67108864.0,67108.864);     // speeds in bits per milliseconds within an 26 bit range, here jump speed is 1000 times faster than mark speed

          E1803_digi_set_motf(cardNum,0.0,0.0);               // no marking on-the-fly enabled

          E1803_set_trigger_point(cardNum);                   // wait for external trigger

          E1803_jump_abs(cardNum,-10000000,-10000000,0); // jump to mark start position (using unit bits within an 26 bit range)

          E1803_mark_abs(cardNum,-10000000,10000000,0); // mark a square
          E1803_mark_abs(cardNum,10000000,10000000,0); // mark a square
          E1803_mark_abs(cardNum,10000000,-10000000,0); // mark a square
          E1803_mark_abs(cardNum,-10000000,-10000000,0); // mark a square

          E1803_execute(cardNum);

         // wait until marking has started
         while ((E1803_get_card_state(cardNum) & (E1803_CSTATE_MARKING|E1803_CSTATE_PROCESSING))==0)
         {
#ifdef ENV_WINDOWS
             Sleep(10);
#else
             usleep(10000);
#endif
         }

         E1803_release_trigger_point(cardNum);               // release waiting for external trigger by software command

         // wait until marking is finished
         while ((E1803_get_card_state(cardNum) & (E1803_CSTATE_MARKING|E1803_CSTATE_PROCESSING))!=0)
         {
#ifdef ENV_WINDOWS
             Sleep(500);
#else
             usleep(500000);
#endif
         }

         /*** Motion functions with Motion Extension Board ******************************************************/
         double pos0;

         E1803_motion_reference(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0,E1803_MOTION_REFSTEP_P|E1803_MOTION_REFSTEP_INV_SWITCH,
                                        200,
                                        850,40);                                     
         waitMotionEnd();
         
         E1803_motion_get_pos(cardNum,0,&pos0);
         if (pos0==-1.0)
          E1803_motion_set_pos(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0,2000);
         else
          printf("Error: referencing failed!");

         E1803_motion_set_steps(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0|E1803_COMMAND_FLAG_AXIS_1|E1803_COMMAND_FLAG_AXIS_2|E1803_COMMAND_FLAG_AXIS_3,300);

         E1803_motion_set_limits(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0|E1803_COMMAND_FLAG_AXIS_1|E1803_COMMAND_FLAG_AXIS_2|E1803_COMMAND_FLAG_AXIS_3,
                                         -500.0,500.0,600.0);
         E1803_motion_set_accel(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0,2.0);
         E1803_motion_set_accel(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_1,4.0);
         E1803_motion_set_accel(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_2,8.0);
         E1803_motion_set_accel(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_3,16.0);
         E1803_motion_set_speed(cardNum,E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0|E1803_COMMAND_FLAG_AXIS_1|E1803_COMMAND_FLAG_AXIS_2|E1803_COMMAND_FLAG_AXIS_3,
                                        10);
                                        

         E1803_motion_move_abs(cardNum, E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0,129.0,0,0,0);
         E1803_motion_move_rel(cardNum, E1803_COMMAND_FLAG_STREAM|E1803_COMMAND_FLAG_AXIS_0,10.0,0,0,0);
         waitMotionEnd();

         /*** End of motion functions with Motion Extension Board ******************************************************/

         Sleep(1000);
         E1803_close(cardNum);
      }
      else printf("ERROR: opening connection failed with error %d\r\n",ret);
   }
   else printf("ERROR: Could not initialise!\r\n");
   return 0;
}

