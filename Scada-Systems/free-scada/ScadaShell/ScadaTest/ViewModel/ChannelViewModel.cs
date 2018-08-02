using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using ScadaShell.Model;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Windows.Data;
using FreeSCADA.Interfaces;
using FreeSCADA.Common;
using FreeSCADA.Common.Events.Rules;
using FreeSCADA.Common.Events;

namespace ScadaShell.ViewModel
{
    public class ChannelViewModel:ViewModelBase,IDisposable, IEditable
    {
        IChannel _channel;
        IEventRule _ar;
        double _min, _max;
        private ICommand _setValueCommand;
        public IChannel Channel
        {
            get { return _channel; }
        }
        public ChannelViewModel(String chName):this(DataAccess.ChannelsRepository.GetChannel(chName),0,0)
        {
            
        }

        public ChannelViewModel(IChannel ch)
            : this(ch, 0, 0)
        {

        }
        public ChannelViewModel(IChannel ch,double min,double max)
        {
            _channel = ch;
            _min=min;
            _max = max;
            ch.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(ch_PropertyChanged);
            _ar= new AuditEventRule(new AuditEventDefinition("Параметр " + ch.Name + " изменен. Новое"),
                new Func<object>(() => { return Value; }));
            Env.Current.EventManager.RegisterEvent(_ar);

        }

        void ch_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if(e.PropertyName=="Value")
                OnPropertyChanged("Value");
            if(e.PropertyName=="StatusFlag")
                OnPropertyChanged("Color");
        
        }

        public string Description
        {
            get
            {
                return _channel.Description;
            }
        }
        public object  Value
        {
            get { return _channel.Value;}
            set 
            {
                
                if (_min!=_max && (Convert.ToDouble(value) < _min || Convert.ToDouble(value) > _max))
                    throw new ValidationException();
                _channel.Value = value;
                OnPropertyChanged("Value");
            }
        }
        
        public Brush Color
        {
            get
            {
                
               if (_channel.StatusFlags!=ChannelStatusFlags.Good)
                {
                    return Brushes.Gray;
                }
                return Brushes.Green;
            }
        }
        public ICommand SetValueCommand
        {
            get
            {
                if (_setValueCommand == null)
                {
                    _setValueCommand = new AskRelayCommand(
                        param =>
                        {
                            Value=Convert.ToDouble(param);
                            _ar.UpdateState();
                        },
                        param => { return true; },
                        "Control");
                }
                return _setValueCommand;
            }
        
        }

        protected override void OnDispose()
        {
             _channel.PropertyChanged -= new System.ComponentModel.PropertyChangedEventHandler(ch_PropertyChanged);
            base.OnDispose();
        }
    }
}
