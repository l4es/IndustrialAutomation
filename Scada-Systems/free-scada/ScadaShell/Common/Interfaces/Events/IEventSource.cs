using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Interfaces
{
    public delegate void EventChanged(IEventDefinition ev, IEventState state);

    public interface IEventsSource
    {
        event EventChanged EventChanged;
        ObservableCollection<IEvent> Events
        {
            get;
        }
        IEvent GetEvent(IEventDefinition def);
        void UpdateEvent(IEventDefinition def, IEventState state);
    }
}
