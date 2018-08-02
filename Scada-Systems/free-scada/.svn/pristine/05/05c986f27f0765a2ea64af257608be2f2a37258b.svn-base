using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Interfaces;


namespace FreeSCADA.Common.Events
{
    
    public class CommonEventDefinition : IEventDefinition
    {
        
        public String Text
        {
            get;
            protected set;
        }

        public int Id
        {
            get
            {
                return Text.GetHashCode();
            }
        }

        virtual public AlarmLevel Level
        {
            get { return AlarmLevel.Green; }
        }

        virtual public string Type
        {
            get { return "Общее"; }
        }
        public CommonEventDefinition(String text)
        {
            Text = text;

        }
    }
    public class DiagnosticEventDefinition : CommonEventDefinition
    {
        public override string Type
        {
            get
            {
                return "Диагностическое";
            }
        }
        public override AlarmLevel Level
        {
            get
            {
                return AlarmLevel.Orange;
            }
        }
        public DiagnosticEventDefinition(String text)
            : base(text)
        {


        }
    }

    public class TechnologyEventDefinition : CommonEventDefinition
    {
        AlarmLevel _level;
        public override string Type
        {
            get
            {
                return "Технологическое";
            }
        }
        public override AlarmLevel  Level
        {
            get 
            { 
                return _level;
            }
        }
        
        public TechnologyEventDefinition(String text,AlarmLevel level)
            : base(text)
        {
            _level=level;

        }
    }
    public class AuditEventDefinition : CommonEventDefinition
    {
        public AuditEventDefinition(string text)
            : base(text)
        {
        }
    }
        
        

}
