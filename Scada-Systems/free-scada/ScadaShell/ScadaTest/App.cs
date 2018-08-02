using System;
using System.Configuration;
using System.Windows;
using System.Windows.Input;
using System.Windows.Markup;
using System.Globalization;
using ScadaShell.ViewModel;
using FreeSCADA.Common;
using ScadaShell.GSMLogger;
using System.IO;
using System.Diagnostics;
using System.Security.Principal;
using System.Reflection;


namespace ScadaShell
{
    class Commands : FreeSCADA.Interfaces.ICommands
    {
        #region ICommands Members
        public Commands()
        {
        }
        public FreeSCADA.Interfaces.ICommandContext GetContext(string contextName)
        {
            throw new NotImplementedException();
        }

        public bool RegisterContext(string contextName, FreeSCADA.Interfaces.ICommandContext context)
        {
            throw new NotImplementedException();
        }

        public ICommand FindCommandByName(string contextName, string name)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region ICommands Members


        FreeSCADA.Interfaces.ICommands FreeSCADA.Interfaces.ICommands.RegisterContext(string contextName, FreeSCADA.Interfaces.ICommandContext context)
        {
            throw new NotImplementedException();
        }

        public System.Collections.Generic.List<string> GetRegisteredContextes()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    /// 
    public partial class App : Application
    {
        MainWindowViewModel _viewModel;
        
        public MainWindowViewModel MainWindowModel
        {
            get { return _viewModel; }
        }
        System.Timers.Timer _timer;
        GsmLogger _gsmLogger;
        static App()
        {
            // This code is used to test the app when using other cultures.
            //
            //System.Threading.Thread.CurrentThread.CurrentCulture =
            System.Threading.Thread.CurrentThread.CurrentUICulture =new System.Globalization.CultureInfo("en-us");


            // Ensure the current culture passed into bindings is the OS culture.
            // By default, WPF uses en-US as the culture, regardless of the system settings.
            //
            FrameworkElement.LanguageProperty.OverrideMetadata(
              typeof(FrameworkElement),
              new FrameworkPropertyMetadata(XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag)));
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            var splash = new SplashScreen("splash.jpg");

            splash.Show(true);


            MainWindow window = new MainWindow();

            // Create the ViewModel to which 
            // the main window binds.

            AppDomain.CurrentDomain.SetPrincipalPolicy(PrincipalPolicy.WindowsPrincipal);

            Env.Initialize(window, new Commands(), FreeSCADA.Interfaces.EnvironmentMode.Runtime);
            string appPath = System.IO.Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName);
            Directory.SetCurrentDirectory(appPath);


            Env.Current.Project.Load("project.fs2");
            
            FreeSCADA.Archiver.ArchiverMain.Current.Start();

            CommunationPlugs plugs = Env.Current.CommunicationPlugins;
            if (plugs.Connect() == false)
            {
                Env.Deinitialize();
                return;
            }

            MainWindowViewModel.LoginModel.Login("Гість", "0");
            _viewModel = new MainWindowViewModel();

            // When the ViewModel asks to be closed, 
            // close the window.
            EventHandler handler = null;
            handler = delegate
            {
                _viewModel.RequestClose -= handler;
                window.Close();
            };

            // Force a reload of the changed section.
            // This makes the new values available for reading.

            /*
            GsmLoggerSettings settings = (GsmLoggerSettings)System.Configuration.ConfigurationManager.GetSection("gsmsettings");
            try
            {
                _gsmLogger = new GsmLogger(Env.Current.EventsSummary, settings);
            }
            catch (Exception ex)
            {
                //Env.Current.Logger (string.Format("Unable to start Gsm Logger. {0}", ex.Message));
            }
            */

            window.DataContext = _viewModel;
            window.AddHandler(Mouse.PreviewMouseDownEvent, new MouseButtonEventHandler(MousePressed));
            _timer = new System.Timers.Timer(600000);
            _timer.Elapsed += new System.Timers.ElapsedEventHandler(_timer_Elapsed);




            window.Show();

            
        }
        void _timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            _viewModel.ResetLogin();

        }
        void MousePressed(object sender, MouseEventArgs e)
        {
            _timer.Stop();
            _timer.Start();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            CommunationPlugs plugs = Env.Current.CommunicationPlugins;
            FreeSCADA.Archiver.ArchiverMain.Current.Stop();
            plugs.Disconnect();
            Env.Current.CommunicationPlugins["mdbssrv"].SaveSettings();
            Env.Current.Project.Save("project_copy.fs2");
            Env.Deinitialize();


            base.OnExit(e);

        }

        [STAThread]
        public static void Main()
        {

            App app = new App();
            AppDomain.CurrentDomain.UnhandledException += UnhandledException;


            app.Run();

        }

        static void UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            //FreeSCADA.Archiver.ArchiverMain.Current.Stop();
            Env.Current.Logger.LogError(string.Format("Exceprtion {0}", e.ExceptionObject.ToString()));

            System.Data.Common.DbException ex;
            if ((ex = e.ExceptionObject as System.Data.Common.DbException) != null)
            {
                System.Diagnostics.ProcessStartInfo psi = new ProcessStartInfo("dump.bat");
                psi.CreateNoWindow = true;
                psi.UseShellExecute = false;
                System.Diagnostics.Process proc = new System.Diagnostics.Process();
                proc.StartInfo = psi;
                proc.Start();
                proc.WaitForExit();

                if (System.IO.File.Exists("d:\\archive.sql"))
                {
                    System.IO.File.Delete("d:\\archive.db3");
                    proc.StartInfo.FileName = "clone.bat";
                    proc.Start();
                    proc.WaitForExit();
                    //System.IO.File.Move("archive.sql", "..\\archive.sql");
                }
                Env.Current.Logger.LogError("Базаданных была повреждена, и предпринята попытка восстановления.\nДамп сохранен в файле archive.sql");
            }
         
            //Process.Start("ScadaShell.exe");
            App.Current.Shutdown();
        }
    }
}
