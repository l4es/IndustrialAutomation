using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Filters
{
    public class AuditFilter:IEventFilter
    {
        public bool Apply(IEventDefinition def)
        {
            if (def is AuditEventDefinition)
                return true;
            else return false;
        }
    }
}
