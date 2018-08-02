using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;

using System.Runtime.InteropServices;

public unsafe class libe1701
{
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern byte E1701_set_connection(string lpString);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern void E1701_set_password(byte n, string ethPwd);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_load_correction(byte n, string filename, byte tableNum);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_laser_delays(byte n, double ondelay, double offdelay);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_laser_mode(byte n, uint mode);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_standby(byte n, double frequency, double pulse);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_laser_timing(byte n, double frequency, double pulse);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_scanner_delays(byte n, double jumpdelay, double markdelay, double polydelay);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_jump_abs(byte n, int x, int y, int z);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_mark_abs(byte n, int x, int y, int z);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_set_speeds(byte n, double jumpspeed, double markspeed);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_digi_set_motf(byte n, double motfX, double motfY);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern int E1701_execute(byte n);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint E1701_get_card_state(byte n);
   [DllImport(@"libe1701.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
    public static extern void E1701_close(byte n);

   [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
   delegate int E1701_power_callback(byte n,double power, IntPtr userdata);
   [DllImport("libe1701.dll", CallingConvention = CallingConvention.Cdecl)]
   extern static int E1701_mark_pixelline(byte n,int x, int y,int z,int pixWidth,int pixHeight,int pixDepth,uint pixNum,Double[] pixels,E1701_power_callback callback,IntPtr userData);

   static int E1701_OK=0;

   static uint E1701_LASERMODE_CO2=0x84000001;

   static uint E1701_CSTATE_MARKING=0x0001;
   static uint E1701_CSTATE_PROCESSING = 0x0080;

   private int set_power(byte n,double power, IntPtr userdata)
   {
      double maxDutyCycle=0.5;
      ushort halfPeriod;
      ushort pwrPeriod1;

      halfPeriod=(ushort)(((1.0/20000.0)/2.0)*1000000.0); // usec
      if (halfPeriod<2) halfPeriod=2;
      else if (halfPeriod>65500) halfPeriod=65500;

      pwrPeriod1=(ushort)(halfPeriod*2*(power/100.0)*maxDutyCycle);
      return E1701_set_laser_timing(n,20000,pwrPeriod1);
   }

   public void test()
   {
      byte cardNum = 0;
      int ret;

      try
      {
         cardNum = E1701_set_connection("192.168.2.254"); //COM3");
      }
      catch (System.Exception e)
      {
         Console.WriteLine("Error loading libe1701.dll: " + e.ToString());
      }
      if (cardNum>0)
      {
//      E1701_set_password(cardNum,"123456"); // used only in case of Ethernet connection; requires the password "123456" to be set in card configuration file
         ret=E1701_load_correction(cardNum,"strong.bco",0); // set correction file, for no/neutral correction use "" or NULL here
         if (ret==E1701_OK)
         {
            E1701_set_laser_mode(cardNum,E1701_LASERMODE_CO2); // configure for CO2
            E1701_set_standby(cardNum,20000.0,5.0);            // 20kHz and 5 usec standby frequency/period
            E1701_set_laser_timing(cardNum,20000.0,25.0);      // 20kHz and 50% duty cycle marking frequency/period

            E1701_set_scanner_delays(cardNum,100.0,100.0,10.0); // some delay values, have to be adjusted for used scanhead
            E1701_set_laser_delays(cardNum,20.0,30.0);          // laser on/off delay in microseconds, have to be adjusted for used scan-system
            E1701_set_speeds(cardNum,67108864.0,67108.864);     // speeds in bits per milliseconds within an 26 bit range, here jump speed is 1000 times faster than mark speed

            E1701_digi_set_motf(cardNum,0.0,0.0);               // no marking on-the-fly enabled

            //E1701_set_trigger_point(cardNum);                   // wait for external trigger

            E1701_jump_abs(cardNum,-10000000,-10000000,0); // jump to mark start position (using unit bits within an 26 bit range)

            E1701_mark_abs(cardNum,-10000000, 10000000,0); // mark a square
            E1701_mark_abs(cardNum, 10000000, 10000000,0); // mark a square
            E1701_mark_abs(cardNum, 10000000,-10000000,0); // mark a square
            E1701_mark_abs(cardNum,-10000000,-10000000,0); // mark a square

            E1701_jump_abs(cardNum, -10000000, 11000000, 0); // jump to bitmap line start position
            E1701_set_laser_delays(cardNum,0.0,0.1); // for bitmap marking no laser on/off delay has to be used so set it to smallest possible values
            { // mark a bitmap-line
               double[] pixels=new double[200];
               int      i;

               for (i=0; i<200; i++) pixels[i]=i/2.0; // fade from minimum to maximum

               E1701_mark_pixelline(cardNum,-10000000,-11000000,0,100000,0,0,200,pixels,set_power,IntPtr.Zero);
            }
             
            E1701_execute(cardNum); // ensure all data are flushed and marking is started

            //E1701_release_trigger_point(cardNum);               // release waiting for external trigger by software command

            // wait until marking has started
            while ((E1701_get_card_state(cardNum) & (E1701_CSTATE_MARKING | E1701_CSTATE_PROCESSING)) != 0)
            {
               Thread.Sleep(100);
            }

            // wait until marking is finished
            while ((E1701_get_card_state(cardNum) & (E1701_CSTATE_MARKING|E1701_CSTATE_PROCESSING))!=0)
            {
               Thread.Sleep(500);
            }

            E1701_close(cardNum);
         }
         else Console.WriteLine("ERROR: opening connection failed with error %d",ret);
      }
      else Console.WriteLine("ERROR: Could not initialise!");
   }
}

namespace libe1701_test_c_sharp
{
    class Program
    {
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        internal struct WSAData
        {
            internal Int16 version;
            internal Int16 highVersion;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 257)]
            internal String description;

            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 129)]
            internal String systemStatus;

            internal Int16 maxSockets;
            internal Int16 maxUdpDg;
            internal IntPtr vendorInfo;
        }

        [DllImport("ws2_32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern Int32 WSAStartup(Int16 wVersionRequested, out WSAData wsaData);

        static void Main(string[] args)
        {
            WSAData wsaData;
            libe1701 e1701=new libe1701();

            wsaData.version = 0;
            wsaData.highVersion = 0;
            int resp = WSAStartup(2 << 8 | 2, out wsaData); // to initialise windows socket, not required when E1701 is accessed via USB serial interface
            if (resp != 0 || wsaData.version != (2 << 8 | 2)) 
            {
               Console.WriteLine("ERROR: Could not initialise WinSock!");
            }
            else e1701.test();
        }
    }
}
