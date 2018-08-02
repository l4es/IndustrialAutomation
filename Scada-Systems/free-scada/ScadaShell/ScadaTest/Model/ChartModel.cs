using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;
 
using ScadaShell.DataAccess;

namespace ScadaShell.Model
{
    public class ChartModel
    {
        public string Name
        {
            get
            {
                if (Channel.Description != String.Empty)
                    return Channel.Description;
                else
                    return Channel.Name;
            }
            
        }
        public FreeSCADA.Interfaces.IChannel Channel
        {
            get;
            protected set;

        }
        public Brush Color
        {
            get;
            protected set;
        }
        public MyObservableCollection<ChartItemModel> ChartData
        {
            get;
            protected set;
        }

        public ChartModel(FreeSCADA.Archiver.ChannelInfo ch, List<ChartItemModel> data,Brush color)
        {
            
            Channel=ChannelsRepository.GetChannel(ch.PluginId+"."+ch.ChannelName);
            ChartData = new MyObservableCollection<ChartItemModel>(data);
            Color = color;
            
        }

    }
}
 