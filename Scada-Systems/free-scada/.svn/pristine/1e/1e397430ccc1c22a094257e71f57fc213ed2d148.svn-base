using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;

namespace ScadaShell.DataAccess
{
    public class KranRepository
    {
        Dictionary<String, KranElement> _kransDict=new Dictionary<string,KranElement>();
        
        public KranRepository()
        {
            /*_kransDict["Кран №1"] = new KranElement("Кран №1",
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.Opened"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.Closed"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.OpenCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.CloseCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.Command"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran1.Timeout"));
            _kransDict["Кран №2"] = new KranElement("Кран №2",
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.Opened"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.Closed"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.OpenCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.CloseCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.Command"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran2.Timeout"));
            _kransDict["Кран №3"] = new KranElement("Кран №3",
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.Opened"),
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.Closed"),
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.OpenCMD"),
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.CloseCMD"),
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.Command"),
               ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.Timeout"));
            _kransDict["Кран №4"] = new KranElement("Кран №4",
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.Opened"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.Closed"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.OpenCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.CloseCMD"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.Command"),
                ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.Timeout")); 
              */   
        }
        public KranElement GetKran(String name)
        {
            if (_kransDict.ContainsKey(name))
                return _kransDict[name];
            return null;
        }
    }
}
