using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Filters
{
    public class LevelFilter:IEventFilter
    {
        AlarmLevel _level;
        public LevelFilter(AlarmLevel level)
        {
            _level = level;
        }
        public bool Apply(IEventDefinition def)
        {
            if (def.Level == _level)
                return true;
            return false;
        }
    }
    
}
