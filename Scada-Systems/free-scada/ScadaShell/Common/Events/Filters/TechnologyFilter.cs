using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Filters
{
    public class TechnologyFilter: IEventFilter
    {
        public bool Apply(IEventDefinition def)
        {
            if (def is TechnologyEventDefinition)
                return true;
            else return false;
        }
    }
}
