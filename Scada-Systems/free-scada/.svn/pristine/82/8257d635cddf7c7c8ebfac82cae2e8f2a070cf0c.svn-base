using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace FreeSCADA.Interfaces
{
    public delegate void EventOccured(IEventDefinition def,IEventState state);
    public interface IEventRule
    {
        void  UpdateState();
        IEventDefinition Definition{get;}
        event EventOccured EventOccured;
        IChannel ChannelForRegister
        {
            get;
        }
    }
}
