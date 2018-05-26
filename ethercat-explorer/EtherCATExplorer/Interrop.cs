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
using System.Runtime.InteropServices;

namespace SOEM
{            
    public class SoemInterrop
    {
        [DllImport("SoemWrapper", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StartActivity(String s, int DelayUpMs);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern void StopActivity();

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Run(SlaveState State = SlaveState.Operational);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RefreshSlavesState();

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetDevicesNumber();

        [DllImport("SoemWrapper", EntryPoint = "GetDeviceInfo", CallingConvention = CallingConvention.Cdecl)]
        private static extern Int32 _GetDeviceInfo(UInt32 deviceNum, Int32 ParamNum, StringBuilder sTmp);

        public static Int32 GetDeviceInfo(UInt32 deviceNum, DeviceInfoParam Param, StringBuilder sTmp)
        {
            // ensure a minimal size
            if (sTmp.Length < 100)
                sTmp.Length = 100;

            return _GetDeviceInfo(deviceNum, (int)Param, sTmp);
        }

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern void WriteState(UInt32 deviceNum, SlaveState State);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Reconfigure(UInt32 deviceNum);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetInput(UInt32 deviceNum, byte[] buf);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern int SetOutput(UInt32 deviceNum, byte[] buf);
        
        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern int ReadPDO(UInt32 deviceNum, Int32 Index,
                                    Int32 SubIndex, ref Int32 size, byte[] buf);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern int WritePDO(UInt32 deviceNum, Int32 Index,
                                    Int32 SubIndex, Int32 size, byte[] buf);

        [DllImport("SoemWrapper", CallingConvention = CallingConvention.Cdecl)]
        public static extern int EEprom_Write(UInt32 deviceNum, Int32 start, 
                                    Int32 length, byte[] buf);

        [DllImport("SoemWrapper", EntryPoint ="EEprom_Read", CallingConvention = CallingConvention.Cdecl)]
        private static extern int _EEprom_Read(UInt32 deviceNum, Int32 start,
                                    Int32 length, byte[] buf);

        public static int EEprom_Read(UInt32 deviceNum, Int32 start,
                            Int32 length, byte[] buf)
        {
            // ensure a minimal size
            if (buf.Length < length) return -1;
            // ensure %8
            if ((buf.Length%8)!=0) return -1;
            if ((start % 8) != 0) return -1;

            return _EEprom_Read(deviceNum, start, length, buf);

        }
    }

    public enum DeviceInfoParam : int
    {
        Name = 0,
        OutputSize= 1,
        InputSize = 2,
        State = 4,
        Delay = 5,
        Config_Address = 6,
        ManufacturerId = 7,
        TypeId = 8,
        Rev = 9,
        MailboxProtocol = 10,
    }

    public enum SlaveState : ushort
    {
        Unknow = 0x00,
        Init = 0x01,
        PreOperational = 0x02,
        Boot = 0x03,
        SafeOperational = 0x04,
        Operational = 0x08,
        Ack = 0x10,                       // Not really a state
        Init_ErrorActive = 0x11,
        PreOperational_ErrorActive = 0x12,
        SafeOperational_ErrorActive = 0x14,
        Operational_ErrorActive = 0x18,  // Don't think it can exist
    }

    public enum MailBoxProto : ushort
    {
        AoE=0x0001,
        EoE=0x0002,
        CoE=0x0004,
        FoE=0x0008,
        SoE=0x0010,
        VoE=0x0020
    }
}
