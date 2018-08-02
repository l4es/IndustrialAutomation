using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;


namespace ScadaShell.DataAccess
{
    class ChannelsRepository
    {
        public static IChannel GetChannel(String name)
        {
            if (Env.Current.CommunicationPlugins.GetChannel(name) == null)
                throw new Exception("Channel " + name + " does not exists");
            return Env.Current.CommunicationPlugins.GetChannel(name);
        }

    }
}
