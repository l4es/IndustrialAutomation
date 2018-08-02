using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using FreeSCADA.Interfaces;
using ScadaShell.DataAccess;

namespace ScadaShell.Model
{
    public class KranElement : SchemaElementBase
    {
      
        public IChannel OpenStateChannel
        {
            get;
            protected set;
        }
        public IChannel CloseStateChannel
        {
            get;
            protected set;
        }

        public IChannel OpenCommandChannel
        {
            get;
            protected set;
        }
        public IChannel CloseCommandChannel
        {
            get;
            protected set;
        }

        
        public IChannel  CommandChannel
        {
            get;
            protected set;
        }
        public IChannel TimeoutChannel
        {
            get;
            protected set;
        }
        public IChannel Prioritet
        {
            get;
            protected set;
        }
        public IChannel Prioritet2
        {
            get;
            protected set;
        }
        public IChannel V110Channel
        {
            get;
            protected set;
        }
        public IChannel CoilOpenError
        {
            get;
            protected set;
        }
        public IChannel CoilCloseError
        {
            get;
            protected set;
        }
        public KranElement(String name,String description)
        {
            Name = description;
            String prefix= "opc_connection_plug.Zachist."+name;
                
            OpenStateChannel=ChannelsRepository.GetChannel(prefix+".Opened");
            CloseStateChannel=ChannelsRepository.GetChannel(prefix+".Closed");
            OpenCommandChannel=ChannelsRepository.GetChannel(prefix+".OpenCMD");
            CloseCommandChannel=ChannelsRepository.GetChannel(prefix+".CloseCMD");
            CommandChannel = ChannelsRepository.GetChannel(prefix+".Command");
            TimeoutChannel = ChannelsRepository.GetChannel(prefix+".Timeout");
            Prioritet = ChannelsRepository.GetChannel(prefix+".Prioritet");
            Prioritet2 = ChannelsRepository.GetChannel(prefix+".Prioritet2");
            V110Channel = ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Discrets.V110");
            CoilOpenError = ChannelsRepository.GetChannel(prefix + ".CoilOpenError");
            CoilCloseError = ChannelsRepository.GetChannel(prefix + ".CoilCloseError");
        }

    }
}
