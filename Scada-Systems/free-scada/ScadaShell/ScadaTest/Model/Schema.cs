using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Collections.ObjectModel;
using ScadaShell.Interfaces;
using FreeSCADA.Interfaces;
using System.Reflection;

namespace ScadaShell.Model
{
    public abstract class Schema: SchemaElementBase, IWorkspaceElememnt
    {
        public string SecurityRule
        {
            get;
            protected set;
        }
    }
}
