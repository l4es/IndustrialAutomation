using System;
namespace FreeSCADA.Interfaces
{
    public enum EventState
    {
        End = 0,
        Start = 1,
        Ack = 3
    }
    public interface IEventState
    {   
        void UpdateEvent(IEvent e);
        String FormatText(String text);
        EventState GetCause();
        object Value
        {
            get;
        }
        DateTime Time
        {
            get;
        }
    }
}
