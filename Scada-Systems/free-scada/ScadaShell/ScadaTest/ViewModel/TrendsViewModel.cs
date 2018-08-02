using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using ScadaShell.Model;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Windows.Data;
using FreeSCADA.Interfaces;
using FreeSCADA.Archiver;
using ScadaShell.DataAccess;

namespace ScadaShell.ViewModel
{
    public class TrendsViewModel:WorkspaceViewModel
    {

        public Dictionary<String,TrendViewModel> Trends
        {
            get;
            protected set;
        }

        public TrendsViewModel()
            : this(DateTime.MinValue)
        {

        }
        public TrendsViewModel(DateTime point)
        {
            Trends = new Dictionary<string, TrendViewModel>();
            Trends[RulseReposytory.GetRules()[0].Name] = new TrendViewModel(RulseReposytory.GetRules()[0], 0, 10);
            Trends[RulseReposytory.GetRules()[1].Name] = new TrendViewModel(RulseReposytory.GetRules()[1], 0, 100);
            Trends[RulseReposytory.GetRules()[2].Name] = new TrendViewModel(RulseReposytory.GetRules()[2], 0, 2.7);
            Trends[RulseReposytory.GetRules()[3].Name] = new TrendViewModel(RulseReposytory.GetRules()[3], -40, 60);
            if (point != DateTime.MinValue)
                foreach (var t in Trends.Values)
                    t.To = point;

        }
       

    }
}
