using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;

namespace ScadaShell.ViewModel
{
    public class AutoSettingsSchemaViewModel:CommonSchemaViewModel
    {
        AutoSettingsSchema _schema;
        
        public ChannelViewModel Pwork
        {
            get;
            protected set;
        }
        public ChannelViewModel  Kran3Prioritet
        {
            get;
            protected set;
        }
        public ChannelViewModel Kran4Prioritet
        {
            get;
            protected set;
        }

        public ChannelViewModel  SetPointMinMin
        {
            get;
            protected set;
        }
        public ChannelViewModel  SetPointMin
        {
            get;
            protected set;
        }
        public ChannelViewModel SetPointMax
        {
            get;
            protected set;
        }
        public ChannelViewModel  SetPointMaxMax
        {
            get;
            protected set;
        }
        public ChannelViewModel  SpTimeout
        {
            get;
            protected set;
        }
        public ChannelViewModel RepairTime
        {
            get;
            protected set;
        }

        public AutoSettingsSchemaViewModel(AutoSettingsSchema sch):base(sch)
        {
            _schema = sch;
            Pwork = new ChannelViewModel(sch.Pwork,0,8);

            Kran3Prioritet= new ChannelViewModel(sch.Kran3Prioritet,0,2);
            Kran4Prioritet = new ChannelViewModel(sch.Kran4Prioritet,0,2);
            SetPointMinMin = new ChannelViewModel(sch.SetPointMinMin,0,40);
            SetPointMin = new ChannelViewModel(sch.SetPointMin,0,30);
            SetPointMax = new ChannelViewModel(sch.SetPointMax,0,30);
            SetPointMaxMax= new ChannelViewModel(sch.SetPointMaxMax,0,40);
            SpTimeout = new ChannelViewModel(sch.SpTimeout,0,20);
            RepairTime= new ChannelViewModel(sch.RepairTime,0,120);

        }
    }
}
