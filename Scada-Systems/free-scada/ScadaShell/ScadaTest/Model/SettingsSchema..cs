using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using ScadaShell.View;
using ScadaShell.DataAccess;
using FreeSCADA.Interfaces;

namespace ScadaShell.Model
{
    public class SettingsSchema : MainSchema
    {
        public SettingsSchema():base()
        {
            Name = "Налаштування";
            SecurityRule = "Tune";
          
        }
    }
}
