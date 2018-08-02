using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events.Rules
{
    public class TwoChannelComparerRule:BaseRule
    {
        public TwoChannelComparerRule(IEventDefinition def):base(def)
        {
        }


        public override void UpdateState()
        {
            throw new NotImplementedException();
        }
    }
}
