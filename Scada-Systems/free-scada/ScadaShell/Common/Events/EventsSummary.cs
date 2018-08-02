using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events
{
    public class EventsSummary : IEventsSource
    {
        ObservableCollection<IEvent> _events = new ObservableCollection<IEvent>();
        
        public ObservableCollection<IEvent> Events
        {
            get { return _events ; }
        }
        public EventsSummary(EventsManager em)
        {
            foreach(IEventRule rule in em.Events)
                rule.EventOccured += new EventOccured(UpdateEvent);
            em.EventAdded += new EventsManager.EventAddedDelegate(em_EventAdded);
        }

        void em_EventAdded(IEventRule er)
        {
            er.EventOccured += new EventOccured(UpdateEvent);
        }

        

        public void UpdateEvent(IEventDefinition def,IEventState state)
        {
            var ev = _events.Where(evm => evm.Definition.Id == def.Id).SingleOrDefault();
            if (ev == null)
                _events.Insert(0,ev = new Event(def,this)); 
            state.UpdateEvent(ev);
            
            if (!ev.IsActive)
                _events.Remove(ev);
            if (EventChanged != null)
                EventChanged(ev.Definition, state);
        }
        public event EventChanged EventChanged;
         

        public IEvent GetEvent(IEventDefinition def)
        {
            return _events.Where(evm => evm.Definition.Id == def.Id).SingleOrDefault();
        }
    }
}
