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

namespace Acyclic
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
        static void DeviceInfo()
        {
            StringBuilder sb = new StringBuilder(200); // Buffer large enough to get back slave parameters

            SoemInterrop.GetDeviceInfo(1, DeviceInfoParam.State, sb);
            SlaveState s = (SlaveState)Convert.ToUInt16(sb.ToString());
            Console.WriteLine("State : " + s.ToString());
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void Main(string[] args)
        {

            // Interfaces list
            List<Tuple<String, String, String>> Interfaces = GetAvailableInterfaces();

            // Start network activity on the first interface (indice 0)
            String PcapInterfaceName = "\\Device\\NPF_" + Interfaces[0].Item3;
            int NumberofSlavesDetected = SoemInterrop.StartActivity(PcapInterfaceName, 1);

            // Until now if the parameter is missing it's Operational, otherwise slaves are
            // in there default mode (SafeOperational is not used).
            SoemInterrop.Run(SlaveState.SafeOperational);

            byte[] buf=new byte[50]; // must be large enough
            int size=buf.Length;
   
            // some Acyclic Read/Write
            // Could be also realize in operational, init, .... state
            SoemInterrop.ReadPDO(1,0x6000,-1, ref size, buf);
            SoemInterrop.WritePDO(1, 0x7000, 0, 4, buf);

            DeviceInfo();

            // Change to Operational
            SoemInterrop.WriteState(1, SlaveState.Operational);

            for (; ; )
            {
                SoemInterrop.RefreshSlavesState();
                DeviceInfo();
                Thread.Sleep(2000);
            }

        }
    }
}
