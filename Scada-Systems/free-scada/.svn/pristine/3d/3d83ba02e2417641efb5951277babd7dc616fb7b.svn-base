using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;
using FreeSCADA.Archiver;
using ScadaShell.DataAccess;

namespace ScadaShell.Infrastructure
{
    class ArchiveEventsSource:IEventsSource
    {

        public ArchiveEventsSource( )
        {
             
            Events=new ObservableCollectionEx<IEvent>(ArchiveAccess.GetEvents(DateTime.Now.AddMonths(-2),DateTime.Now,this));

        }
        public System.Collections.ObjectModel.ObservableCollection<IEvent> Events
        {
            get;
            protected set;
        }

        public IEvent GetEvent(IEventDefinition def)
        {
            throw new NotImplementedException();
        }


        public void UpdateEvent(IEventDefinition def, IEventState state)
        {
            
        }

        public event EventChanged EventChanged;
    }
}
