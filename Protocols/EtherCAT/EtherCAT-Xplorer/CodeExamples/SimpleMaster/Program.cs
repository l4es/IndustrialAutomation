/**************************************************************************
*                           MIT License
* 
* Copyright (C) 2016 Frederic Chaxel <fchaxel@free.fr>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using SOEM;

namespace SimpleMaster
{
    class Program
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get the Ethernet/Wifi Up Interfaces list
        public static List<Tuple<String, String, String>> GetAvailableInterfaces()
        {
            List<Tuple<String, String, String>> ips = new List<Tuple<String, String, String>>();
            System.Net.NetworkInformation.NetworkInterface[] interfaces = System.Net.NetworkInformation.NetworkInterface.GetAllNetworkInterfaces();
            foreach (System.Net.NetworkInformation.NetworkInterface inf in interfaces)
            {
                if (!inf.IsReceiveOnly && inf.OperationalStatus == System.Net.NetworkInformation.OperationalStatus.Up && inf.SupportsMulticast && inf.NetworkInterfaceType != System.Net.NetworkInformation.NetworkInterfaceType.Loopback)
                    ips.Add(new Tuple<string, string, string>(inf.Description, inf.Name, inf.Id));
            }
            return ips;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        static bool Open()
        {
            try
            {
                // Interfaces list
                List<Tuple<String, String, String>> Interfaces = GetAvailableInterfaces();

                // a wonderfull menu
                int i = 0;
                Console.WriteLine("Choose an Interface : ");
                foreach (Tuple<String, String, String> it in Interfaces)
                    Console.WriteLine("\t" + (i++).ToString() + " : " + it.Item1);

                ConsoleKeyInfo key = Console.ReadKey();

                i = Convert.ToInt32(key.KeyChar - 48);

                // Get the pcap name of this interface
                String PcapInterfaceName = "\\Device\\NPF_" + Interfaces[i].Item3;
                Console.WriteLine("Interface technical name is : " + PcapInterfaceName);

                // Try start with a tiemout delay of 1s
                int NumberofSlavesDetected=SoemInterrop.StartActivity(PcapInterfaceName, 1);
                // Put up (Operational) all slaves
                if (NumberofSlavesDetected > 0)
                {
                    SoemInterrop.Run();
                    return true;
                }
            }
            catch
            {

            }
            return false;

        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void Main(string[] args)
        {
            if (Open() == true)
            {
                StringBuilder sb=new StringBuilder(200); // Buffer to get back slave parameters

                // Assume two slave devices
                for (uint i=1;i<3;i++)
                {
                    // We can get some information ... for nothing, just to show
                    SoemInterrop.GetDeviceInfo(i, DeviceInfoParam.OutputSize, sb);
                    SoemInterrop.GetDeviceInfo(i, DeviceInfoParam.InputSize, sb);

                    SoemInterrop.GetDeviceInfo(i, DeviceInfoParam.Config_Address, sb);
                    Console.WriteLine("Slave @ : ", sb.ToString());
                }

                // Assume two devices, copy two bytes from the input into the other output
                // echange buffers with the SOEM stack
                byte[] bin1 = new byte[50]; // must be equal or more than InputSize in the corresponding slave
                byte[] bout2 = new byte[50]; // must be strikcly equal to OutputSize in the corresponding slave

                for (; ; )
                {
                    // Get Input from the first slave
                    SoemInterrop.GetInput(1, bin1);

                    // Some action ! 
                    bout2[0] = bin1[0];
                    bout2[1] = bin1[1];

                    // Set Output in the second slave
                    SoemInterrop.SetOutput(2, bout2);

                    // Quite tired with this work
                    Thread.Sleep(200);
                }
            }

        }
    }
}
