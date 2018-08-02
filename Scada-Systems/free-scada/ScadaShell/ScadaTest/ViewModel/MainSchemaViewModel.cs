using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;
using System.Windows;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Diagnostics;

namespace ScadaShell.ViewModel
{
    public class MainSchemaViewModel : WorkspaceViewModel
    {
        MainSchema _schema;
        
        public KranViewModel Kran1
        {
            get;
            protected set;
        }
        public KranViewModel Kran2
        {
            get;
            protected set;
        }
        public KranViewModel Kran3
        {
            get;
            protected set;
        }
        public KranViewModel Kran4
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel  Pin
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel Pout
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel Tin
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel  Tout
        {
            get;
            protected set;
        }
        public ChannelViewModel  Pwork
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel Q1
        {
            get;

            protected set;
        }
        public ComplexParametrViewModel Q2
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel Q3
        {
            get;
            protected set;
        }
        public ComplexParametrViewModel Q4
        {
            get;
            protected set;
        }

                     
        public MainSchemaViewModel(MainSchema sh)
        {
            _schema = sh;
            DisplayName = _schema.Name;
            Kran1 = new KranViewModel(_schema.Kran1);
            Kran2 = new KranViewModel(_schema.Kran2);
            Kran3 = new KranViewModel(_schema.Kran3);
            Kran4 = new KranViewModel(_schema.Kran4);
            Pin = new ComplexParametrViewModel(_schema.Pin);
            Pout = new ComplexParametrViewModel(_schema.Pout);
            Tin = new ComplexParametrViewModel(_schema.Tin);
            Tout = new ComplexParametrViewModel(_schema.Tout);
            Pwork = new ChannelViewModel(_schema.Pwork,0,8);
            

            Q1 = new QComplexChannelViewModel(_schema.Q1);
            Q2 = new QComplexChannelViewModel(_schema.Q2);
            Q3 = new QComplexChannelViewModel(_schema.Q3);
            Q4 = new QComplexChannelViewModel(_schema.Q4);

        }
        override public ICommand CloseViewCommand
        {
            get
            {
                if (_closeViewCommand == null)
                    _closeViewCommand = new RelayCommand(
                        param => {
                            Process.Start("explorer.exe");
                            App.Current.Shutdown();
                        },
                        param => true,
                        "Tune"
                        );

                return _closeViewCommand;
            }
        }
        override protected List<CommandViewModel> CreateCommands()
        {
            List<CommandViewModel> commands = new List<CommandViewModel>();
            commands.Add(new CommandViewModel("Закрыть", CloseViewCommand));
            commands.Add(new CommandViewModel("Среда Разработки", new RelayCommand(
                        param => {

                            Process.Start("c:\\MELSEC\\GX IEC Developer 7.04\\mmp.exe");
                            App.Current.Shutdown();
                        },
                        param => true,
                        "Tune"
                        )));
            
            return commands;
        }
    }
}
