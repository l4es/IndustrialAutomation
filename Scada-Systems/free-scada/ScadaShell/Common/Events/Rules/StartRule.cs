using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events.States;
namespace FreeSCADA.Common.Events.Rules
{
    public class StartRule:BaseRule
    {


        public StartRule():base(new CommonEventDefinition("Панель запущена"))
        {
            UpdateState();
        }
        override public void UpdateState()
        {
            EmitEvent(new StartedEventState(0, DateTime.Now));
            EmitEvent(new EndedEventState(0, DateTime.Now));
            EmitEvent(new AcknowlegedEventState(0, DateTime.Now));

        }

    }
}
