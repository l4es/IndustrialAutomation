#include <stdio.h>
#include <stdlib.h>

#include "../libe1701.h"

#ifdef ENV_WINDOWS
 #include <windows.h>
#else
 #include <unistd.h>
#endif

static int cardNum=-1;


// *******************************************************************************************
// set power for a CO2 laser in unit % using frequency of 20 kHz and maximum duty cycle of 50%
// HANDLE WITH CARE!!! This function is called every time the power changes from one pixel to
// an other. so complex clculations done heare cause a big load on host computer, and lots of
// E1701-functions called here cause a lot of load on Ethernet/USB connection and on the
// controller itself!
// *******************************************************************************************
static int set_power(unsigned char n,double power,void *userData)
{
   double         maxDutyCycle=0.5;
   unsigned short halfPeriod;
   unsigned short pwrPeriod1;

   halfPeriod=(unsigned short)(((1.0/20000.0)/2.0)*1000000.0); // usec
   if (halfPeriod<2) halfPeriod=2;
   else if (halfPeriod>65500) halfPeriod=65500;

   pwrPeriod1=(unsigned short)(halfPeriod*2*(power/100.0)*maxDutyCycle);
   return E1701_set_laser_timing(n,20000,pwrPeriod1);
}


int main(int argc, char *argv[])
{
   int          ret;
#ifdef ENV_WINDOWS
   WSADATA      wsaData;

   WSAStartup(MAKEWORD(1,1),&wsaData);
#endif

   cardNum=E1701_set_connection("127.0.0.1");
   if (cardNum>0)
   {
//      E1701M_set_password(cardNum,"123456"); // used only in case of Ethernet connection; requires the password "123456" to be set in card configuration file
      ret=E1701_load_correction(cardNum,"",0); // set correction file, for no/neutral correction use "" or NULL here
      if (ret==E1701_OK)
      {
         // *** set general marking parameters ************************************************************************************************
         E1701_set_laser_mode(cardNum, E1701_LASERMODE_CO2);     // configure for CO2
         E1701_set_standby(cardNum, 20000.0, 5.0);               // 20kHz and 5 usec standby frequency/period
         E1701_set_laser_timing(cardNum, 20000.0, 25.0);         // 20kHz and 50% duty cycle marking frequency/period
         E1701_set_z_correction(cardNum,100000000,0,0);
         E1701_set_scanner_delays(cardNum, 100.0, 100.0, 10.0); // some delay values, have to be adjusted for used scanhead
         E1701_set_laser_delays(cardNum, 20.0, 30.0);           // laser on/off delay in microseconds, have to be adjusted for used scan-system
         E1701_set_speeds(cardNum, 67108864.0, 67108.864);      // speeds in bits per milliseconds within an 26 bit range, here jump speed is 1000 times faster than mark speed
         E1701_digi_set_motf(cardNum, 0.0, 0.0);                // no marking on-the-fly enabled

         // *** toggle the laser for a fixed period of time ***********************************************************************************
         E1701_set_laser(cardNum, E1701_COMMAND_FLAG_DIRECT, 1); // turn on the laser immediately using previously defined laser parameters
#ifdef ENV_WINDOWS
         Sleep(1000);
#else
         sleep(1);
#endif
         E1701_set_laser(cardNum, E1701_COMMAND_FLAG_DIRECT, 0); // turn off the laser after about 1 second

         // *** perform a normal mark operation with vector data ******************************************************************************
         E1701_set_trigger_point(cardNum);                   // wait for external trigger
         E1701_jump_abs(cardNum, -10000000, -10000000, -10000000); // jump to mark start position (using unit bits within an 26 bit range)
         E1701_mark_abs(cardNum, -10000000, 10000000, 10000000);   // mark a square
         E1701_mark_abs(cardNum, 10000000, 10000000, -10000000);   // mark a square
         E1701_mark_abs(cardNum, 10000000, -10000000, 10000000);   // mark a square
         E1701_mark_abs(cardNum, -10000000, -10000000, -10000000); // mark a square

         // *** mark a single pixel line ******************************************************************************************************
         {
            #define NUM_PIXELS 500
            double pixelLine[NUM_PIXELS];
            int    i;

            for (i=0; i<NUM_PIXELS; i++)
             pixelLine[i]=(i/5.0);

            E1701_set_laser_delays(cardNum, 0.0, 0.5);           // laser on/off delay in microseconds, need to be as small as possible for bitmap marking
            E1701_set_scanner_delays(cardNum, 0.0, 0.0, 0.0);    // some delay values, have to be adjusted for used scanhead but are much smaller for bitmaps typically
            E1701_set_pixelmode(cardNum, 0, 2.0, 0);             // fast marking mode with jumps when laser is off and which ignores all power values <= 2%

            E1701_mark_pixelline(cardNum,0,0, 0,
                                 10, 0, 0,
                                 NUM_PIXELS, pixelLine, set_power, NULL);
         }
         // *** ensure marking operation is started *******************************************************************************************
         E1701_execute(cardNum);

         // *** wait until marking has started ************************************************************************************************
         while ((E1701_get_card_state(cardNum) & (E1701_CSTATE_MARKING | E1701_CSTATE_PROCESSING)) == 0)
         {
#ifdef ENV_WINDOWS
            Sleep(10);
#else
            usleep(10000);
#endif
         }

         E1701_release_trigger_point(cardNum);               // release waiting for external trigger by software command

         // *** wait until marking is finished ************************************************************************************************
         while ((E1701_get_card_state(cardNum) & (E1701_CSTATE_MARKING | E1701_CSTATE_PROCESSING)) != 0)
         {
#ifdef ENV_WINDOWS
            Sleep(500);
#else
            usleep(500000);
#endif
         }

         Sleep(1000);
         E1701_close(cardNum);
      }
      else printf("ERROR: opening connection failed with error %d\r\n",ret);
   }
   else printf("ERROR: Could not initialise!\r\n");
   return 0;
}

