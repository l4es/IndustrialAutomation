using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events.States;

namespace FreeSCADA.Common.Events.Rules
{
    public class AuditEventRule:BaseRule
    {
        Func<object> _value;
        public AuditEventRule(IEventDefinition def,Func<object> value)
            : base(def)
        {
            _value = value;
            //UpdateState();
            
        }
        override public void UpdateState()
        {
            EmitEvent(new StartedEventState(_value.Invoke(), DateTime.Now));
            EmitEvent(new EndedEventState(_value.Invoke(), DateTime.Now));
            EmitEvent(new AcknowlegedEventState(_value.Invoke(), DateTime.Now));

        }

    }
}
