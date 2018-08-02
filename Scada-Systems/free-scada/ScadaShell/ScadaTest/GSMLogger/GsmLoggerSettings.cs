using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace ScadaShell.GSMLogger
{
    public class GsmLoggerSettings : ConfigurationSection
    {
        [ConfigurationProperty("ComPort", DefaultValue = "COM1")]
        public string ComPort
        {
            get { return ((string)(base["ComPort"])); }

        }
        [ConfigurationProperty("PinCode", DefaultValue = "0000")]
        public string PinCode
        {
            get { return ((string)(base["PinCode"])); }

        }
        [ConfigurationProperty("TelNumbers")]
        public TelNumbersCollection TelNumbers
        {
            get { return ((TelNumbersCollection)(base["TelNumbers"])); }

        }
        public GsmLoggerSettings()
        {
        }

    }
    [ConfigurationCollection(typeof(TelNumElement))]

    public class TelNumbersCollection : ConfigurationElementCollection
    {
        protected override ConfigurationElement CreateNewElement()
        {
            return new TelNumElement();
        }

        protected override object GetElementKey(ConfigurationElement element)
        {
            return ((TelNumElement)(element)).TelNum;
        }

        public TelNumElement this[int idx]
        {
            get
            {
                return (TelNumElement)BaseGet(idx);
            }
        }
    }

    public class TelNumElement : ConfigurationElement
    {
        
        [ConfigurationProperty("telnum", DefaultValue = "", IsKey = true, IsRequired = true)]
        public string TelNum
        {
            get
            {
                return ((string)(base["telnum"]));
            }
            set
            {
                base["telnum"] = value;
            }
        }

    }
}
