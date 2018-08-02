using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Common.Events
{
    public class Event:IEvent,INotifyPropertyChanged
    {
        IEventDefinition _def;
        DateTime _startTime;
        DateTime _endTime;
        DateTime _ackTime;
        public Event(IEventDefinition def, IEventsSource source,String text)
        {
            Text = text;
            _def = def;
            Source = source;
        }

        public Event(IEventDefinition def,IEventsSource source):this(def,source,def.Text)
        {
            
        }
        public IEventDefinition Definition
        {
            get { return _def; }
        }

        public DateTime StartTime
        {
            get{return _startTime;}
            set
            {
                _startTime=value;
                RaisePropertyChanged("StartTime");
            }
        }

        public DateTime AckTime
        {
            get { return _ackTime; }
            set
            {
                _ackTime = value;
                RaisePropertyChanged("AckTime");
                
            }
        }

        public DateTime EndTime
        {
            get { return _endTime; }
            set
            {
                _endTime = value;
                RaisePropertyChanged("EndTime");
            }
        }

        public bool IsActive
        {
            get
            {

                if (EndTime != DateTime.MinValue && AckTime != DateTime.MinValue && StartTime != DateTime.MinValue)
                {
                    /*EndTime = DateTime.MinValue;
                    AckTime = DateTime.MinValue;
                    StartTime = DateTime.MinValue;*/
                    return false;
                }
                else if (StartTime == DateTime.MinValue)
                    return false;
                return true;

            }
        }


        public string Text
        {
            get;
            protected set;
        }

    
        public event PropertyChangedEventHandler  PropertyChanged;
        void RaisePropertyChanged(String prop)
        {
            if(PropertyChanged!=null)
                PropertyChanged(this,new PropertyChangedEventArgs(prop));
            
        }




        public IEventsSource Source
        {
            get;
            protected set;
        }


        public bool IsAcked
        {
            get { return AckTime != DateTime.MinValue; }
        }
    }
}
