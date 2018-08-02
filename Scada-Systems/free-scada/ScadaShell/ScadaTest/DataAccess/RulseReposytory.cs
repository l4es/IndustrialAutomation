using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Archiver;

namespace ScadaShell.DataAccess
{
    class RulseReposytory
    {

        
        public static Rule GetRule(string id)
        {
            foreach (Rule r in ArchiverMain.Current.RulesSettings.Rules)
                if (r.Name == id)
                    return r;
            return null;
        }

        public static List<Rule> GetRules()
        {
            List<Rule> rules= new List<Rule>();
            foreach (Rule r in ArchiverMain.Current.RulesSettings.Rules)
                rules.Add(r);
            return rules;
        }
    }
}
