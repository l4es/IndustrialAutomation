using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Filters
{
    public class GlobalFilter:IEventFilter
    {
        public bool Apply(IEventDefinition def)
        {
            return true;
        }
    }
}
