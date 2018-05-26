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
using System.ComponentModel;
using System.Diagnostics;
using SOEM;

namespace EtherCATExplorer
{
    public class EthCATDevice
    {
        uint Id;

        public String Name { get; set; }
        public SlaveState State { get; set; }
        public ushort Delay { get; set; }
        public ushort Config_Address { get; set; }

        public uint _ManufacturerId;
        public String ManufacturerId { get; set; }

        public uint _TypeId;
        public String TypeId { get; set; }

        public uint Revision { get; set; }

        public String MailboxProtocol { get; set; }

        //public List<MailBoxProto> MailboxProtocol { get; set; }

        [DescriptionAttribute("Number of bits")]
        public ushort InputSize { get; set; }        
        [DescriptionAttribute("Number of bits")]
        public ushort OutputSize { get; set; }

        public byte[] InputData;
        public byte[] OutputData;

        StringBuilder sExchange = new StringBuilder((int)1000);

        public EthCATDevice(uint Id)
        {
            this.Id = Id;
            GetConstAttribut();
            GetDynamicAttribut();
        }

        private void GetConstAttribut()
        {
            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.Name, sExchange);
            Name = sExchange.ToString();

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.OutputSize, sExchange);
            OutputSize = Convert.ToUInt16(sExchange.ToString());
            if (OutputSize > 0)
                OutputData = new byte[(OutputSize / 8) + (OutputSize % 8)];

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.InputSize, sExchange);
            InputSize = Convert.ToUInt16(sExchange.ToString());
            if (InputSize > 0)
                InputData = new byte[(InputSize / 8) + (InputSize % 8)];

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.Delay, sExchange);
            Delay = Convert.ToUInt16(sExchange.ToString());

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.Config_Address, sExchange);
            Config_Address = Convert.ToUInt16(sExchange.ToString());

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.ManufacturerId, sExchange);
            _ManufacturerId = Convert.ToUInt32(sExchange.ToString());
            ManufacturerId = "0x"+_ManufacturerId.ToString("X8");

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.TypeId, sExchange);
            _TypeId = Convert.ToUInt32(sExchange.ToString());
            TypeId = "0x" + _TypeId.ToString("X8");

            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.Rev, sExchange);
            Revision = Convert.ToUInt32(sExchange.ToString());

            // Maibox Protocol
            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.MailboxProtocol, sExchange);
            ushort mbx = Convert.ToUInt16(sExchange.ToString());

            StringBuilder sb=new StringBuilder();

            foreach (MailBoxProto proto in Enum.GetValues(typeof(MailBoxProto)))
                if ((mbx & (ushort)proto) != 0)
                    sb.Append(proto.ToString()+" ");
            MailboxProtocol = sb.ToString();            

        }
        private void GetDynamicAttribut()
        {
            SoemInterrop.GetDeviceInfo(Id, DeviceInfoParam.State, sExchange);
            State = (SlaveState)(Convert.ToByte(sExchange.ToString())&0x1F);

            SoemInterrop.GetDeviceInfo(Id, (DeviceInfoParam)(-1), sExchange);
        }

        public void Refresh()
        {
            GetDynamicAttribut();
        }

        public void WriteState(SlaveState state)
        {
            SoemInterrop.WriteState(Id, state);
        }
        public void Reconfigure()
        {
            SoemInterrop.Reconfigure(Id);
        }

        public void WriteOutput()
        {
            SoemInterrop.SetOutput(Id, OutputData);
        }

        public void ReadInput()
        {
            SoemInterrop.GetInput(Id, InputData);
        }

        // SubIndex=-1 for all
        public byte[] ReadPDO(int Idx, int SubIndex, out int size)
        {
            byte[] buff = new byte[4096];
            size = 4096;

            int ret = SoemInterrop.ReadPDO(Id, Idx, SubIndex, ref size, buff);
            if (ret != 0)
            {
                Trace.WriteLine("PDO Read OK");
                return buff;
            }
            else
            {
                Trace.WriteLine("PDO Read Fail");
                return null;
            }
        }
        // SubIndex=-1 for all
        public bool WritePDO(int Idx, int SubIndex, int size, byte[] buff)
        {
            int ret = SoemInterrop.WritePDO(Id, Idx, SubIndex, size, buff);
            if (ret != 0)
            {
                Trace.WriteLine("PDO Write OK");
                return true;
            }
            else
            {
                Trace.WriteLine("PDO Write Fail");
                return false;
            }
        }

        public override string ToString()
        {
            return Name+" - " +Config_Address.ToString();
        }
    }

}
