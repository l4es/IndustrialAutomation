using System;
using System.Windows.Input;
using ScadaShell.Infrastructure;

namespace ScadaShell.ViewModel
{
 
    public class KranControlViewModel: ViewModelBase
    {
        ChannelViewModel _timeout;
        ChannelViewModel _prioritet;
        ChannelViewModel _prioritet2;
        ICommand _closeCommand;
        ICommand _openCommand;
        ICommand _cancelCommand;
        ICommand _unblockCommand;
        ICommand _blockCommand;
        

        public KranViewModel Kran
        {
            get;
            protected set;
        }
        
        public ChannelViewModel Timeout
        {
            get
            {
                if (_timeout == null)
                {
                    _timeout = new ChannelViewModel(Kran.KranElement.TimeoutChannel);

                }
                return _timeout;
            }
        }

        public ChannelViewModel Prioritet
        {
            get
            {
                if (_prioritet== null)
                {
                    _prioritet= new ChannelViewModel(Kran.KranElement.Prioritet);

                }
                return _prioritet;
            }
        }

        public ChannelViewModel Prioritet2
        {
            get
            {
                if (_prioritet2== null)
                {
                    _prioritet2 = new ChannelViewModel(Kran.KranElement.Prioritet2);

                }
                return _prioritet2;
            }
        }
        
        public bool IsBlocked
        {
            get
            {
                if (Convert.ToInt32(Prioritet2.Value)== 0)
                    return true;
                return false;
            }
        
        }
        public bool IsNotBlocked
        {
            get
            {
                if (Convert.ToInt32(Prioritet2.Value) != 0)
                    return true;
                return false;
            }

        }
        
        public KranControlViewModel(KranViewModel kvm)
        {

            Kran = kvm;
            Kran.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(Kran_PropertyChanged);   
        }

        void Kran_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            OnPropertyChanged("IsBlocked");
            OnPropertyChanged("IsNotBlocked");
            CommandManager.InvalidateRequerySuggested();
        }

        #region Commands
        public ICommand BlockCommand
        {
            get
            {
                if (_blockCommand == null)
                {
                    _blockCommand = new RelayCommand(
                        param => { BlockKran(); },
                        param => { return true; },
                        "Control"
                        );
                }

                return _blockCommand;
            }
        }
        public ICommand UnblockCommand
        {
            get
            {
                if (_unblockCommand== null)
                {
                    _unblockCommand = new RelayCommand(
                        param => { UnblockKran(); },
                        param => { return true; },
                        "Control"
                        );
                }

                return _unblockCommand;
            }
        }

        public ICommand CloseCommand
        {
            get
            {
                if (_closeCommand == null)
                {
                    _closeCommand = new RelayCommand(
                        param => { CloseKran(); },
                        param => { return Kran.OpenStateContact == true; },
                        "Control"

                        );
                }
                
                return _closeCommand;
            }
        }
        public ICommand OpenCommand
        {
            get
            {
                if (_openCommand == null)
                {
                    _openCommand = new RelayCommand(
                        param => { OpenKran(); },
                        param => { return Kran.CloseStateContact == true; },
                        "Control"
                        );
                }
                return _openCommand;
            }
        }
        
        public ICommand CancelCommand
        {
            get
            {
                if (_cancelCommand == null)
                {
                    _cancelCommand = new RelayCommand(
                        param => { Cancel(); },
                        param => { return Convert.ToInt32(Kran.KranElement.CommandChannel.Value)!= 0; },
                        "Control"
                        );
                }
                return _cancelCommand;
            }
        }

        
        #endregion


        #region Logic

        public void CloseKran()
        {
            Kran.KranElement.CommandChannel.Value = 2;
        }
        public void OpenKran()
        {
            Kran.KranElement.CommandChannel.Value = 1;
        }
        public void Cancel()
        {
            Kran.KranElement.CommandChannel.Value = false;
            Kran.KranElement.OpenCommandChannel.Value= false;
            Kran.KranElement.CommandChannel.Value = 4;
        }
        public void UnblockKran()
        {
            Kran.KranElement.Prioritet2.Value = Kran.KranElement.Prioritet.Value;
        }
        public void BlockKran()
        {
            Kran.KranElement.Prioritet2.Value = 0;
        }
        #endregion
    }
}
