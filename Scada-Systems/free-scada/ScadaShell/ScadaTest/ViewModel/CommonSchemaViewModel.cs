using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;

namespace ScadaShell.ViewModel
{
    public class CommonSchemaViewModel : WorkspaceViewModel
    {
        public Dictionary<string,ChannelViewModel> Channels
        {
            get;
            protected set;
        }
        public CommonSchemaViewModel(Schema sch)
        {
            Channels = new  Dictionary<string,ChannelViewModel>();
            sch.Channels.ForEach(c=>Channels[c.Name/*.Split('.')[c.Name.Split('.').Length-1]*/] = new ChannelViewModel(c));
        }
    }
}
