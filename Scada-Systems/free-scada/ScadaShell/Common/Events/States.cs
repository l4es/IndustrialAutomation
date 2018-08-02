using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.States
{

    public abstract class BaseEventState:IEventState
    {
        public BaseEventState(object value, DateTime time)
        {
            Value = value;
            Time = time;
        }
        public object Value
        {
            get;
            protected set;

        }

        public DateTime Time
        {
            get;
            protected set;
        }
        abstract public EventState GetCause();

        abstract public void UpdateEvent(IEvent e);
        virtual  public String FormatText(String text)
        {
            return text + " значение=" + Value;
        }

    }
    public class StartedEventState : BaseEventState
    {
        public StartedEventState(object value, DateTime time):base(value,time)
        {
        }
        override  public void UpdateEvent(IEvent e)
        {
            e.StartTime = DateTime.Now;
            e.EndTime = DateTime.MinValue;
            
        }
        override public  EventState GetCause()
        {
            return EventState.Start;
        }


    }

    public class EndedEventState : BaseEventState
    {
        public EndedEventState(object value, DateTime time):
            base(value,time)
        {
        }
        override  public void UpdateEvent(IEvent e)
        {
            e.EndTime = DateTime.Now;
            
        }
        override  public EventState GetCause()
        {
            return EventState.End;

        }
        
        
    }
    public class AcknowlegedEventState : BaseEventState
    {
        public AcknowlegedEventState(object value, DateTime time)
            : base(value, time)
        {
        }
        override  public void UpdateEvent(IEvent e)
        {
            e.AckTime = DateTime.Now;

        }
        override  public EventState GetCause()
        {
            return EventState.Ack;
        }
        
    }

}
