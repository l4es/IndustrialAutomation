using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using ScadaShell.Model;
using ScadaShell.Infrastructure;
using ScadaShell.DataAccess;
using System.Windows.Media;
using ScadaShell.Interfaces;
using System.Windows.Controls.Primitives;
using FreeSCADA.Interfaces;
using System.Security.Permissions;

namespace ScadaShell.ViewModel
{
    public class KranViewModel : ViewModelBase
    {
        
        #region Fields
        ICommand _showUiCommand;
        bool _isUiShowed;
        ChannelViewModel _timeout;

        #endregion
        
        #region Constructors
        
        public KranViewModel(String kranName):this(kranName,new KranRepository())
        {
            
        }
        public KranViewModel(String kranName, KranRepository repos):this(repos.GetKran(kranName))
        {
            
        }
        public KranViewModel(KranElement kran)
        {
            KranElement = kran;
            
            foreach (IChannel ch in KranElement.Channels)
                ch.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(ch_PropertyChanged);
        }


        #endregion
        
        #region OnChannelsChanged
        void ch_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            base.OnPropertyChanged("StateColor");
            base.OnPropertyChanged("CommandStateColor");
            base.OnPropertyChanged("KranOpacity");

        }
        #endregion

        #region Properties
        public KranElement KranElement{
            get;
            protected set;
        }

        public String KranName
        {
            get
            {
                return KranElement.Name;
            }
         }
        public String Description
        {
            get
            {
                if (KranElement.CommandChannel.Description != String.Empty)
                    return KranElement.CommandChannel.Description;
                else
                    return KranElement.Name;
            }
        }
        public ChannelViewModel Timeout
        {
            get
            {
                if (_timeout == null)
                {
                    _timeout = new ChannelViewModel(KranElement.CommandChannel);

                }
                return _timeout;
            }
        }
        public bool OpenStateContact
        {
            get { return Convert.ToBoolean(KranElement.OpenStateChannel.Value); }
        }
        public bool CloseStateContact
        {
            get { return Convert.ToBoolean(KranElement.CloseStateChannel.Value); }
        }
        
        public bool OpenCommandSolenoid
        {
            get { return Convert.ToBoolean(KranElement.OpenCommandChannel.Value); }
            //[PrincipalPermission(SecurityAction.Demand, Role = "Operator")]
            set { KranElement.OpenCommandChannel.Value = value; }
        }
        
        public bool CloseCommandSolenoid
        {

            get { return Convert.ToBoolean(KranElement.CloseCommandChannel.Value); }
            //[PrincipalPermission(SecurityAction.Demand, Role = "Operator")]
            set { KranElement.CloseCommandChannel.Value = value; }
        }
        public bool V110State
        {
            get { return Convert.ToBoolean(KranElement.V110Channel.Value); }
            set { KranElement.V110Channel.Value = value; }
        }

        public Brush SolenoidOpenColor
        {
            get
            {
                if (Convert.ToBoolean(KranElement.CoilOpenError.Value) == true)
                    return Brushes.Red;
                return Brushes.Green;

            }
        }
        public Brush SolenoidCloseColor
        {
            get
            {
                if (Convert.ToBoolean(KranElement.CoilCloseError.Value) == true)
                    return Brushes.Red;
                return Brushes.Green;

            }
        }
        public double KranOpacity
        {
            get
            {
                if (Convert.ToInt32(KranElement.Prioritet2.Value) == 0)
                    return 0.5;
                return 1;
            }
        }
        public SolidColorBrush StateColor
        {
            get
            {
                

                if (OpenStateContact && !CloseStateContact)
                    return Brushes.Green;
                else if (!OpenStateContact && CloseStateContact)
                    return Brushes.Red;
                if (OpenStateContact && CloseStateContact)
                    return Brushes.Blue;
                if (!OpenStateContact && !CloseStateContact)
                    return Brushes.Gray;
                return Brushes.White;
            }
        }

        public Boolean IsUiShowed
        {
            get{return _isUiShowed;}
            set{
                _isUiShowed=value;
                OnPropertyChanged("IsUiShowed");
            }
        }
        

        public SolidColorBrush CommandStateColor
        {
            get
            {
                if (OpenCommandSolenoid && !CloseCommandSolenoid)
                    return Brushes.Green;
                else if (!OpenCommandSolenoid && CloseCommandSolenoid)
                    return Brushes.Red;
                return Brushes.Gray;
            }
        }
        

        #endregion

        #region Commands
        public ICommand ShowUiCommand
        {
            get
            {
                if (_showUiCommand == null)
                {
                    _showUiCommand = new RelayCommand(
                        param => { 
                            View.PopupView p = new View.PopupView(400,450,new KranControlViewModel(this));

                            IsUiShowed = true; 
                        },
                        param => { return true; }
                        );
                }
                return _showUiCommand;
            }
        }
        protected override void OnDispose()
        {
            foreach (IChannel ch in KranElement.Channels)
                ch.PropertyChanged -= ch_PropertyChanged;
            base.OnDispose();
        }
    }
        #endregion
}

