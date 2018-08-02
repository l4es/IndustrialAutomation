using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using FreeSCADA.Interfaces;

namespace ScadaShell.Model
{
    class ValueElement:SchemaElementBase
    {
        public IChannel ValueChannel
        {
            get;
            protected set;
        }
        public ValueElement(IChannel value)
        {
            ValueChannel = value;
        }

       
    }
}
