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
    public class ComplexParametrElement : SchemaElementBase
    {

        public String Name
        {
            get;
            protected set;
        }
        
        public String Description
        {
            get;
            protected set;
        }

        public IChannel Value
        {
            get;
            protected set;
        }
        public IChannel Discrets
        {
            get;
            protected set;
        }

        public IChannel DiscretsLow
        {
            get;
            protected set;
        }
        public IChannel DiscretsHigh
        {
            get;
            protected set;
        }

        
        public IChannel  RangeLow
        {
            get;
            protected set;
        }
        public IChannel RangeHigh
        {
            get;
            protected set;
        }
        public IChannel SetpointLow
        {
            get;
            protected set;
        }
        public IChannel SetpointHigh
        {
            get;
            protected set;
        }

        public IChannel BreakLow
        {
            get;
            protected set;
        }
        public IChannel BreakHigh
        {
            get;
            protected set;
        }

        public IChannel Break
        {
            get;
            protected set;
        }
        public IChannel SpLow
        {
            get;
            protected set;
        }
        
        public IChannel SpHigh
        {
            get;
            protected set;
        }

        public IChannel Gisteresis
        {
            get;
            protected set;
        }
        public IChannel Error
        {
            get;
            protected set;
        }


        public ComplexParametrElement(String name, string desc)
        {
            String prefix="opc_connection_plug.Zachist.Analog."+name;
            Name = name;
            Value= ChannelsRepository.GetChannel(prefix+".Value");
            SetpointLow= ChannelsRepository.GetChannel(prefix+".SetpointLow");
            SetpointHigh= ChannelsRepository.GetChannel(prefix+".SetpointHigh");
            Gisteresis= ChannelsRepository.GetChannel(prefix+".Gisteresis");
            DiscretsLow= ChannelsRepository.GetChannel(prefix+".DiscretsLow");
            DiscretsHigh= ChannelsRepository.GetChannel(prefix+".DiscretsHigh");
            RangeLow = ChannelsRepository.GetChannel(prefix + ".RangeLow");
            RangeHigh = ChannelsRepository.GetChannel(prefix + ".RangeHigh");
            Discrets = ChannelsRepository.GetChannel(prefix+".Discrets");
            BreakLow= ChannelsRepository.GetChannel(prefix+".BreakLow");
            BreakHigh= ChannelsRepository.GetChannel(prefix+".BreakHigh");
            Break = ChannelsRepository.GetChannel(prefix + ".Break");
            SpLow = ChannelsRepository.GetChannel(prefix + ".SpLow");
            SpHigh = ChannelsRepository.GetChannel(prefix + ".SpHigh");
            Gisteresis = ChannelsRepository.GetChannel(prefix + ".Gisteresis");
            Error = ChannelsRepository.GetChannel(prefix + ".Error");
            Description = desc;
           
        }

    }
}
