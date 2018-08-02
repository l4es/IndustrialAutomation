using System;
using System.Collections.Generic;
using System.Windows.Media;
using System.Linq;
using System.Text;
using ScadaShell.Interfaces;
using ScadaShell.Model;
using System.Windows;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Windows.Data;
using FreeSCADA.Interfaces;
using ScadaShell.DataAccess;

namespace ScadaShell.ViewModel
{
    public class ComplexParametrViewModel:ViewModelBase
    {

        ComplexParametrElement _ccelm;
        public ComplexParametrViewModel(ComplexParametrElement chEl)
        {

            _ccelm = chEl;
            Value = new ChannelViewModel(_ccelm.Value);
            Discrets = new ChannelViewModel(_ccelm.Discrets);
            DiscretsLow=new ChannelViewModel(_ccelm.DiscretsLow);
            DiscretsHigh = new ChannelViewModel(_ccelm.DiscretsHigh);
            RangeLow=new ChannelViewModel(_ccelm.RangeLow);
            RangeHigh = new ChannelViewModel(_ccelm.RangeHigh);
            SetpointLow =new ChannelViewModel(_ccelm.SetpointLow);
            SetpointHigh=new ChannelViewModel(_ccelm.SetpointHigh);
            BreakLow = new ChannelViewModel(_ccelm.BreakLow);
            BreakHigh =new ChannelViewModel(_ccelm.BreakHigh);
            Gisteresis=new ChannelViewModel(_ccelm.Gisteresis);
            Break=new ChannelViewModel(_ccelm.Break);
            SpLow=new ChannelViewModel(_ccelm.SpLow);
            SpHigh=new ChannelViewModel(_ccelm.SpHigh);
            Error = new ChannelViewModel(_ccelm.Error); 

            foreach(IChannel ch in _ccelm.Channels)
                ch.PropertyChanged+=new System.ComponentModel.PropertyChangedEventHandler(ch_PropertyChanged);
        }
     
        void ch_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            IChannel ch = sender as IChannel;
            string[] name= ch.Name.Split('.');
            OnPropertyChanged(name[name.Count() - 1]);
            OnPropertyChanged("Color");
            OnPropertyChanged("State");
            OnPropertyChanged("ValueVisibility");
            OnPropertyChanged("StateVisibility");

        }

        /*void ch_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            OnPropertyChanged("Value");
            OnPropertyChanged("Color");
            OnPropertyChanged("State");

        }*/

        public Visibility ValueVisibility
        {
            get{
                if (Convert.ToBoolean(Break.Value) == true||
                    Convert.ToBoolean(Error.Value))
                {
                    return Visibility.Hidden;
                }
                return Visibility.Visible;
            }
        }
        public Visibility StateVisibility
        {
            get
            {
                if (Convert.ToBoolean(Break.Value) == true ||
                        Convert.ToBoolean(Error.Value))
                {
                    return Visibility.Visible;
                }
                return Visibility.Hidden;
            }
        }

        public String State
        {
            get
            {
                if ( Convert.ToBoolean(Break.Value)==true)
                {
                    return "Обрыв";
                }
                if (Convert.ToBoolean(Error.Value) == true)
                {
                    return "Ошибка";
                }

                return "Норм.";
            }
        }

        virtual public Brush Color
        {
            get
            {
                if (Value.Channel.StatusFlags!= ChannelStatusFlags.Good)
                {
                    return Brushes.Gray;
                }
                if (Convert.ToBoolean(Break.Value) == true)
                 {
                     return Brushes.Blue;
                 }
                if (Convert.ToBoolean(Error.Value) == true)
                {
                    return Brushes.Cyan;
                }
                if (Convert.ToBoolean(SpHigh.Value) == true || Convert.ToBoolean(SpLow.Value) == true)
                {
                    return Brushes.Red;
                }
                return Brushes.Green;
            }
        }

        public String Name
        {
            get
            {
                return _ccelm.Name;
            }
        }
        public String Description
        {
            get
            {
                return _ccelm.Description;
            }
        }
        
        public ChannelViewModel  Value
        {
            get ;
            protected set;
        
          
        }
        public ChannelViewModel  Discrets
        {
            get;
            protected set;
        
           
        }
        public ChannelViewModel DiscretsLow
        {
            get;
            protected set;
        }
        public ChannelViewModel DiscretsHigh
        {
            get;
            protected set;

        }
        public ChannelViewModel RangeLow
        {
            get;
            protected set;
        
            
        }
        public ChannelViewModel RangeHigh
        {
            get;
            protected set;
        }

        public ChannelViewModel SetpointLow
        {
            get;
            protected set;

        }
        public ChannelViewModel  SetpointHigh 
        {
            get;
            protected set;
        }

        public ChannelViewModel BreakLow
        {
            get;
            protected set;
        }
        public ChannelViewModel BreakHigh
        {
            get;
            protected set;
        }

        public ChannelViewModel Gisteresis
        {
            get;
            protected set;
        }

        public ChannelViewModel  Break
        {
            get;
            protected set;
        }
        public ChannelViewModel  SpLow
        {
            get;
            protected set;
        
        }

        public ChannelViewModel  SpHigh
        {
            get;
            protected set;
        }

        public ChannelViewModel  Error
        {
            get;
            protected set;
        
        }

        public ICommand CalibrateLow
        {
            get
            {
                return new AskRelayCommand(
                        param => { _ccelm.DiscretsLow.Value = _ccelm.Discrets.Value; },
                        param => { return true; },
                        "Tune"
                        );
                
            }
        }
        public ICommand CalibrateHigh
        {
            get
            {
                return new AskRelayCommand(
                        param => { _ccelm.DiscretsHigh.Value = _ccelm.Discrets.Value; },
                        param => { return true; },
                        "Tune"
                        );
                
            }
            
        }
        protected override void OnDispose()
        {
            foreach (IChannel ch in _ccelm.Channels)
                ch.PropertyChanged -= ch_PropertyChanged;
            base.OnDispose();
        }
    }
}
