using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using FreeSCADA.Interfaces;
namespace ScadaShell.ViewModel
{
    public class SwitchModeViewModel:ViewModelBase
    {
        IChannel _modeChannel;
        public SwitchModeViewModel(IChannel modeChannel  )
        { 
            _modeChannel=modeChannel;
            ToServiceCommand = new CommandViewModel("Режим наладки", new AskRelayCommand(
                x => { ToService(); OnRequestClose(); },
                x => { return Convert.ToInt32(_modeChannel.Value) != 0; },
                "Control"));

            ToManualCommand = new CommandViewModel("Ручной Режим", new AskRelayCommand(
                x => { ToManual(); OnRequestClose(); },
                x => { return Convert.ToInt32(_modeChannel.Value) != 1; },
                "Control"));

            ToAutoCommand = new CommandViewModel("Автоматический Режим", new AskRelayCommand(
                x => { ToAuto(); OnRequestClose(); },
                x => { return Convert.ToInt32(_modeChannel.Value) != 2; },
                "Control"));

        

        }
        public CommandViewModel ToServiceCommand
        {
            get;
            protected set;
        }
        public CommandViewModel ToAutoCommand
        {
            get;
            protected set;
        }
        public CommandViewModel ToManualCommand
        {
            get;
            protected set;
        }
        
        protected void ToService()
        {
            _modeChannel.Value = 0;
        }
        protected void ToAuto()
        {
            _modeChannel.Value = 2;
        }
        protected void ToManual()
        {
            _modeChannel.Value = 1;
        }
    }
}
