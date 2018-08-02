using System;
using System.Threading;
using libe1803_dot_net;

namespace libe1803_dot_net_test_console
{
    class Program
    {
        static void Main(string[] args)
        {
            DotNet1803 E1803 = new DotNet1803();
            if (E1803.set_connection("192.168.2.254"))
            {
                //      E1803.set_password("123456"); // used only in case of Ethernet connection; requires the password "123456" to be set in card configuration file
                if (E1803.load_correction("", 0)) // set correction file, for no/neutral correction use "" or NULL here
                {
                    E1803.set_laser_mode(DotNet1803.lasermode.LASERMODE_CO2);    // configure for CO2
                    E1803.set_standby(20000.0, 5.0);               // 20kHz and 5 usec standby frequency/period
                    E1803.set_laser_timing(20000.0, 25.0);         // 20kHz and 50% duty cycle marking frequency/period

                    // perform a normal mark operation
                    E1803.set_laser_mode(DotNet1803.lasermode.LASERMODE_CO2); // configure for CO2
                    E1803.set_standby(20000.0, 5.0);            // 20kHz and 5 usec standby frequency/period
                    E1803.set_laser_timing(20000.0, 25.0);      // 20kHz and 50% duty cycle marking frequency/period

                    E1803.set_scanner_delays(100.0, 100.0, 10.0); // some delay values, have to be adjusted for used scanhead
                    E1803.set_laser_delays(20.0, 30.0);          // laser on/off delay in microseconds, have to be adjusted for used scan-system
                    E1803.set_speeds(67108864.0, 67108.864);     // speeds in bits per milliseconds within an 26 bit range, here jump speed is 1000 times faster than mark speed

                    E1803.digi_set_motf(0.0, 0.0);               // no marking on-the-fly enabled

                    E1803.set_trigger_point();                   // wait for external trigger

                    E1803.jump_abs(-10000000, -10000000, 0); // jump to mark start position (using unit bits within an 26 bit range)

                    E1803.mark_abs(-10000000, 10000000, 0); // mark a square
                    E1803.mark_abs(10000000, 10000000, 0); // mark a square
                    E1803.mark_abs(10000000, -10000000, 0); // mark a square
                    E1803.mark_abs(-10000000, -10000000, 0); // mark a square

                    E1803.execute();

                    // wait until marking has started
                    while ((E1803.get_card_state() & (E1803.CSTATE_MARKING | E1803.CSTATE_PROCESSING)) == 0)
                    {
                        Thread.Sleep(10);
                    }

                    E1803.release_trigger_point();               // release waiting for external trigger by software command

                    // wait until marking is finished
                    while ((E1803.get_card_state() & (E1803.CSTATE_MARKING | E1803.CSTATE_PROCESSING)) != 0)
                    {
                        Thread.Sleep(500);
                    }

                    E1803.close();
                }
                else
                    Console.WriteLine("ERROR: opening connection failed with error " + E1803.ErrorDetail);
            }
            else
                Console.WriteLine("ERROR: Could not initialise!");
        }
    }
}
