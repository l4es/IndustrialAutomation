using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Reflection;
using System.Windows.Input;
using System.Windows.Media;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;
using ScadaShell.DataAccess;
using ScadaShell.Infrastructure;
using ScadaShell.Model;
using System.Windows;


namespace ScadaShell.ViewModel
{
    /// <summary>
    /// The ViewModel for the application's main window.
    /// </summary>
    public class MainWindowViewModel : WorkspaceViewModel
    {
        #region Fields

        ReadOnlyCollection<CommandViewModel> _commands;
        readonly SchemaRepository _schemaRepository;
        WorkspaceViewModel _currentWorkspace;
        
        
        LoginCommandViewModel _loginCommandViewModel;

        IChannel _statusChanel;
        ICommand _openEvents;
        IChannel _autoModeChannel;
        private RelayCommand _switchModeCommand;
        #endregion // Fields

        #region Constructor


        public  MainWindowViewModel()
        {
            base.DisplayName = Strings.MainWindowViewModel_DisplayName;
            _schemaRepository = new SchemaRepository();

            
           _loginCommandViewModel= new LoginCommandViewModel();
           CommonEventSummaries = new EventsSummaryViewModel(Env.Current.EventsSummary, new FreeSCADA.Common.Events.Filters.GlobalFilter());
           _statusChanel= ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.Pwork");
           if (_statusChanel == null)
               throw new Exception("Status channel does not exists");
           _statusChanel.PropertyChanged += new PropertyChangedEventHandler(_chanel_PropertyChanged);
           _autoModeChannel= ChannelsRepository.GetChannel("opc_connection_plug.Zachist.Auto.Mode");
           _autoModeChannel.PropertyChanged += new PropertyChangedEventHandler(_chanel_PropertyChanged);
           ShowSchema(new MainSchema());
         
        }
        
          
        void _chanel_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            OnPropertyChanged("StatusString");
            OnPropertyChanged("StatusColor");
            OnPropertyChanged("AutoModeString"); 

        }
        

        #endregion // Constructor

        #region Properties
        public ChannelViewModel GrsName
        {
            get;
            protected set;
        }

        public String AutoModeString
        {
            get
            {

                switch (Convert.ToInt32(_autoModeChannel.Value))
                {
                    case 0:
                        return "Наладка";
                    case 1:
                        return "Ручний";
                    case 2:
                        return "Автомат";
                }
                return "Помилка";
            }
        }
        public String StatusString
        {
            get
            {

                if (_statusChanel.Status == "Bad" || _statusChanel.Status == "Unknown")
                {
                    return "Немає зв'язку з контроллером";
                }
                return "Зв'язок з контроллером встановлено";
            }
        }
        public Brush StatusColor
        {
            get
            {

                if (_statusChanel.Status == "Bad" || _statusChanel.Status == "Unknown")
                {
                    return Brushes.Red;
                }
                return Brushes.Green;
            }
        }

        public EventsSummaryViewModel CommonEventSummaries
        {
            get;
            protected set;
        }

        public WorkspaceViewModel CurrentWorkspace
        {
            get
            {
                return _currentWorkspace;
            }
            protected set
            {
                if (_currentWorkspace != null)
                    _currentWorkspace.RequestClose -= OnWorkspaceRequestClose;
                _currentWorkspace=value;
                if (_currentWorkspace != null)
                    _currentWorkspace.RequestClose += OnWorkspaceRequestClose;

                OnPropertyChanged("CurrentWorkspace");

            }

        }

        
        public LoginCommandViewModel LoginCommand
        {
            get{
                return _loginCommandViewModel;
            }
        }
        #endregion

        #region Commands

        /// <summary>
        /// Returns a read-only list of commands 
        /// that the UI can display and execute.
        /// </summary>
        public ReadOnlyCollection<CommandViewModel> Commands
        {
            get
            {
                if (_commands == null)
                {
                    List<CommandViewModel> cmds = this.CreateGLobalCommands();
                    _commands = new ReadOnlyCollection<CommandViewModel>(cmds);
                }
                return _commands;
            }
        }

        List<CommandViewModel> CreateGLobalCommands()
        {
            List<CommandViewModel> commands = new List<CommandViewModel>();
            foreach (Schema sch in _schemaRepository.GetSchemas())
            {
                var s = sch;
                commands.Add(new CommandViewModel(
                     sch.Name,
                    new RelayCommand(param => this.ShowSchema(s),
                        x => { return true; },
                        s.SecurityRule
                        )));
            }

            commands.Add(new CommandViewModel(
                     "Графіки",
                    new RelayCommand(param => this.SetActiveWorkspace(new TrendsViewModel()))));
            commands.Add(new CommandViewModel(
                     "Події",
                    new RelayCommand(param => this.SetActiveWorkspace(new EventsViewerViewModel()))));
            commands.Add(new CommandViewModel("Сервіс",new RelayCommand(param => this.SetActiveWorkspace(new ServiceViewModel()),
                        param => true,
                        "Tune"
                        )));
            commands.Add(new CommandViewModel(
                     "Информація",
                    new RelayCommand(param => {
                        MessageBox.Show(System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString());                   
                    } )));
            return commands;
        }
        
        

        public ICommand SwitchModeCommand
        {
            get
            {
                if (_switchModeCommand== null)
                    _switchModeCommand = new RelayCommand(param => SwitchMode());
                return _switchModeCommand ;
            }
        }

        public ICommand OpenEvents
        {
            get
            {
                if(_openEvents==null)
                    _openEvents=new RelayCommand(param => this.SetActiveWorkspace(new EventsViewerViewModel()));
                return _openEvents;
            }

        }
        #endregion // Commands

        #region Workspaces

        

        void OnWorkspaceRequestClose(object sender, EventArgs e)
        {
            WorkspaceViewModel workspace = sender as WorkspaceViewModel;
            workspace.Dispose();
            CurrentWorkspace= null;
        }

        #endregion // Workspaces

        #region Private Helpers
        
        void ShowSchema(Schema schema)
        {
            foreach (Type t in Assembly.GetExecutingAssembly().GetTypes())
            {
                if (t.Name == schema.GetType().Name + "ViewModel")
                {

                    
                    WorkspaceViewModel workspace = (WorkspaceViewModel)Activator.CreateInstance(t, new object[] { schema });
                    this.SetActiveWorkspace(workspace);
                }
            }

          }


        public void SetActiveWorkspace(WorkspaceViewModel workspace)
        {
            
            if (CurrentWorkspace != null)
            {
                CurrentWorkspace.Dispose();
                CurrentWorkspace.RequestClose -= workspace_RequestClose;
            }
            CurrentWorkspace = workspace;
            CurrentWorkspace.RequestClose += workspace_RequestClose;   
        }
        
        void workspace_RequestClose(object sender, EventArgs e)
        {
            ShowSchema(new MainSchema());
            
            
        }
        void SwitchMode()
        {
            View.PopupView p = new View.PopupView(100, 200, new SwitchModeViewModel(_autoModeChannel));
        }
        protected override void OnDispose()
        {
            foreach (IDisposable ids in Commands)
                ids.Dispose();
            base.OnDispose();
        }

        #endregion // Private Helpers
        public void ResetLogin()
        {
            LoginModel.Login("Гість", "0");
            ShowSchema(new MainSchema());

        }
    }
}