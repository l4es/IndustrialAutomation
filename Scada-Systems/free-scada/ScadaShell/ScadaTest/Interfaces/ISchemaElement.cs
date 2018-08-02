using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace ScadaShell.Interfaces
{
    public interface ISchemaElement
    {
        String Name
        {
            get;
        }
        
        List<IChannel> Channels
        {
            get;
        }
        List<ISchemaElement> Elements
        {
            get;
        }

        IChannel FindChannel(string name);

    }
}
