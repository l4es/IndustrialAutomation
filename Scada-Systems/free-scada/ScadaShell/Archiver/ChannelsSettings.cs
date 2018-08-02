using System.Collections.Generic;
using System.Linq;
using System.Xml.Serialization;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;
 

namespace FreeSCADA.Archiver
{
	public class RulesSettings
	{
        List<Rule> rules = new List<Rule>();
        Project _project;
        public RulesSettings()
        {
        }
        public RulesSettings(Project prj)
        {
            _project = prj;
        }
		public List<Rule> Rules
		{
			get
			{
				return rules;
			}
			
		}

		public void Clear()
		{
			rules.Clear();
		}

		public void AddRule(Rule rule)
		{
			rules.Add( rule);
		}

        public Rule GetRule(string name)
        {

            return rules.FirstOrDefault(r=>r.Name==name);
        }
		public void Load()
		{
			using (System.IO.Stream ms = _project["settings/archiver/rules.cfg"])
			{
				if (ms == null || ms.Length == 0)
					return;

				XmlSerializer serializer = new XmlSerializer(typeof(List<Rule>));
				rules = (List<Rule>)serializer.Deserialize(ms);
			}
		}

		public void Save()
		{
			using (System.IO.MemoryStream ms = new System.IO.MemoryStream())
			{
				XmlSerializer serializer = new XmlSerializer(typeof(List<Rule>));
				serializer.Serialize(ms, rules);

                _project.SetData("settings/archiver/rules.cfg", ms);
			}
		}
	}
}
