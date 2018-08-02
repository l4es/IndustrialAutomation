using System;
using System.Reflection;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;

namespace ScadaShell.DataAccess
{
    class SchemaRepository
    {
        public SchemaRepository()
        {
        }
        public List<Schema> GetSchemas()
        {
            List<Schema> schemas =new List<Schema>();
            foreach (Type t in Assembly.GetExecutingAssembly().GetTypes())
            {
                if(t.IsSubclassOf(typeof(Schema)))
                    schemas.Add((Schema)System.Activator.CreateInstance(t));
            }
            return schemas;
        }

        
    }
}
