using System;
namespace FreeSCADA.Interfaces
{
    public interface IEvent
    {
        DateTime StartTime { get; set; }
        DateTime AckTime { get; set; }
        DateTime EndTime { get; set; }
        bool IsActive { get; }
        bool IsAcked { get; }
        string Text { get; }
        IEventDefinition Definition { get;}
        IEventsSource Source {get;}
    }
}
