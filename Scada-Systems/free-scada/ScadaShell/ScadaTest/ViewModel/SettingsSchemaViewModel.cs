using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;
using System.Windows.Input;
using System.Windows;
using ScadaShell.Infrastructure;
using System.Security;

namespace ScadaShell.ViewModel
{
    public class SettingsSchemaViewModel : WorkspaceViewModel
    {
        Schema _schema;
        ICommand _nextCommand;
        ICommand _prevCommand;
        int _pos;
        public List<ComplexParametrViewModel> ChannelSettings
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel CurrentParam
        {
            get;
            protected set;
        }
        public ICommand NextCommand
        {
            get
            {
                if (_nextCommand == null)
                {
                    _nextCommand = new RelayCommand(
                        param =>
                        {
                            if (_pos < ChannelSettings.Count-1)
                                _pos++;
                            CurrentParam = ChannelSettings[_pos];
                            OnPropertyChanged("CurrentParam");
                        },
                        param => { return true; }
                        );
                }
                return _nextCommand;
            }
         
        }

        public ICommand PrevCommand
        {
            get
            {

                if (_prevCommand == null)
                {
                    _prevCommand = new RelayCommand(
                        param =>
                        {
                            if (_pos > 0)
                                _pos--;
                            CurrentParam = ChannelSettings[_pos];
                            OnPropertyChanged("CurrentParam");
                        },
                        param => { return true; }
                        );
                }
                return _prevCommand;
            }
        }
        public SettingsSchemaViewModel(Schema sh)
        {
            _schema = sh;
            DisplayName = _schema.Name;
            ChannelSettings = _schema.Elements.Where(e=>e is ComplexParametrElement).Cast<ComplexParametrElement>().Select(e=>new ComplexParametrViewModel(e)).ToList();
                //new List<ComplexParametrViewModel>(
            //ChannelSettings = new List<ComplexParametrViewModel>(_schema.Channels.Select(ch => new ComplexParametrViewModel(new ComplexParametrElement(ch.Split(',')[0], ch.Split(',')[1]))));
            if(ChannelSettings.Count>0)
                CurrentParam = ChannelSettings[0];
           
        }

    }
}
