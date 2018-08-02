using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using ScadaShell.View;
using ScadaShell.DataAccess;
using FreeSCADA.Interfaces;

namespace ScadaShell.Model
{
    public class AutoSettingsSchema : Schema
    {

        public IChannel Pwork
        {
            get;
            protected set;
        }
        public IChannel Kran3Prioritet
        {
            get;
            protected set;
        }
        public IChannel Kran4Prioritet
        {
            get;
            protected set;
        }
        
        public IChannel SetPointMinMin
        {
            get;
            protected set;
        }
        public IChannel SetPointMin
        {
            get;
            protected set;
        }
        public IChannel SetPointMax
        {
            get;
            protected set;
        }
        public IChannel SetPointMaxMax
        {
            get;
            protected set;
        }
        public IChannel SpTimeout
        {
            get;
            protected set;
        }
        public IChannel RepairTime
        {
            get;
            protected set;
        }
        public AutoSettingsSchema()
        {
            Name = "Автомат. режим";
                            
            Kran3Prioritet = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran3.Prioritet");
            Kran4Prioritet = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Kran4.Prioritet");
            Pwork = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.Pwork");
            SetPointMinMin=ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.SetPointMinMIn");
            SetPointMin=ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.SetPointMin");
            SetPointMax=ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.SetPointMax");
            SetPointMaxMax=ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.SetPointMaxMax");
            SpTimeout=ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.SpTimeout");
            RepairTime = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.RepairTime");
            SecurityRule = "Tune";
        }
    }
}
