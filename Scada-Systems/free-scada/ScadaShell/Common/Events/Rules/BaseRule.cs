using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events.States;
namespace FreeSCADA.Common.Events.Rules
{
    public abstract class BaseRule:IEventRule
    {
        public IEventDefinition Definition
        {
            get;
            protected set;
        }

        abstract public void UpdateState();

        public BaseRule(IEventDefinition def)
        {
            Definition = def;
        }
        
        
        protected void EmitEvent(IEventState state)
        {
            if (EventOccured != null)
                EventOccured(Definition, state);
        }


        public event EventOccured EventOccured;


        virtual public IChannel ChannelForRegister
        {
            get { throw new NotImplementedException(); }
        }
    }
}
