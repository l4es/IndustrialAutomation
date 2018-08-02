using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using System.Reflection;
using FreeSCADA.Interfaces;

namespace ScadaShell.Model
{
    public abstract class SchemaElementBase:ISchemaElement
    {
    
        public String Name
        {
            get;
            protected set;
        }
        List<ISchemaElement> _elements;
        public List<ISchemaElement> Elements
        {
            get
            {
                if (_elements == null)
                {
                    _elements = new List<ISchemaElement>();
                    Type t = this.GetType();
                    foreach (PropertyInfo pi in t.GetProperties())
                        if (pi.PropertyType.IsSubclassOf(typeof(SchemaElementBase)))
                            _elements.Add((ISchemaElement)pi.GetValue(this, null));
                }
                return _elements;
            }
        }

        protected List<IChannel> _channels;
        virtual public List<IChannel> Channels
        {
            get
            {
                if (_channels == null)
                {
                    _channels = new List<IChannel>();
                    Type t = this.GetType();
                    
                    Elements.ForEach(e => _channels.AddRange(e.Channels));

                    foreach (PropertyInfo pi in t.GetProperties())
                    {
                        if (pi.PropertyType == typeof(IChannel))
                            _channels.Add((IChannel)pi.GetValue(this, null));
                    }
                }
                return _channels;
            }

        }
     

        public IChannel FindChannel(string name)
        {
            throw new NotImplementedException();
        }
    }
}
