using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Filters
{
    public class DiagnosticFilter: IEventFilter
    {
        public bool Apply(IEventDefinition def)
        {
            if (def is DiagnosticEventDefinition)
                return true;
            else return false;
        }
    }
}
