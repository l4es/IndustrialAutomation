using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events.States;
using ScadaShell.Infrastructure;

namespace ScadaShell.ViewModel
{
    public class EventViewModel : ViewModelBase
    {
        IEvent _event;
        ICommand _ackCommand;
        public IEvent Event
        {
            get
            {
                return _event;
            }
        }
        public EventViewModel(IEvent ev)
        {
            _event = ev;
            INotifyPropertyChanged enf = _event as INotifyPropertyChanged;
            if (enf != null)
                enf.PropertyChanged += new PropertyChangedEventHandler(enf_PropertyChanged);
        }

        void enf_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            OnPropertyChanged(e.PropertyName);
            OnPropertyChanged("StatusColor");
            OnPropertyChanged("LevelColor");
        }


        public IEventDefinition Definition
        {
            get { return _event.Definition; }
        }
        public String Text
        {
            get
            {
                return _event.Text;
            }
        }
        public DateTime StartTime
        {
            get { return _event.StartTime; }

        }
        public DateTime EndTime
        {
            get { return _event.EndTime; }
        }
        public DateTime AckTime
        {
            get { return _event.AckTime; }

        }

        public ICommand AckEvent
        {
            get
            {
                if (_ackCommand == null)
                {
                    _ackCommand = new RelayCommand(
                        param =>
                        {
                            if (_event.AckTime == DateTime.MinValue)
                                _event.Source.UpdateEvent(
                                    _event.Definition, 
                                    new AcknowlegedEventState(
                                        Env.Current.EventManager[_event.Definition.Id].ChannelForRegister
                                        , DateTime.Now));
                        },
                        param => { return true; },
                        "Control"
                        );
                }
                return _ackCommand;
            }



        }
        public Brush StatusColor
        {
            get
            {
                if (StartTime != DateTime.MinValue && AckTime != DateTime.MinValue && EndTime == DateTime.MinValue)
                    return Brushes.Black;
                if (StartTime != DateTime.MinValue && EndTime != DateTime.MinValue)
                    return Brushes.Green;
                if (StartTime != DateTime.MinValue)
                    return Brushes.Red;
                
                
                return Brushes.Gray;
            }
        }
        public Brush LevelColor
        {
            get
            {
                switch (Event.Definition.Level)
                {
                    case AlarmLevel.Green:
                        return Brushes.Green;
                    case AlarmLevel.Yellow:
                        return Brushes.Yellow;
                    case AlarmLevel.Orange:
                        return Brushes.Orange;
                    case AlarmLevel.Red:
                        return Brushes.Red;
                }

                return Brushes.Gray;
            }
        }

        protected override void OnDispose()
        {
            INotifyPropertyChanged enf = _event as INotifyPropertyChanged;
            if (enf != null)
                enf.PropertyChanged -= new PropertyChangedEventHandler(enf_PropertyChanged);
            base.OnDispose();
        }
    }
}
