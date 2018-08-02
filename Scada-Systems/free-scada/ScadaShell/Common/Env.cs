using System;
using System.Windows;
using FreeSCADA.Common.Scripting;
using FreeSCADA.Interfaces;
using System.Security.Principal;
namespace FreeSCADA.Common
{
	public class Env : IEnvironment
	{
		const string version = "2.0.0.9";

		ICommands commands;
		Window mainWindow;
        CommunationPlugs communicationPlugins;
        FreeSCADA.Common.Project project;
		EnvironmentMode mode;
	    ILogger logger;
		ScriptManager scriptManager;
        Events.EventsManager eventsManager;
        Events.EventsSummary eventsSummary;

		#region Initialization and singleton implementation
		static Env environmentInstance = null;
		public static void Initialize(Window mainWindow, ICommands commands,  EnvironmentMode mode)
		{
			if (environmentInstance == null)
			{
				environmentInstance = new Env();

				environmentInstance.mode = mode;
				environmentInstance.logger = new Logging.ELLogger();
				environmentInstance.CreateNewProject();
				environmentInstance.commands = commands;
				environmentInstance.mainWindow = mainWindow;
                environmentInstance.communicationPlugins = new CommunationPlugs();
                environmentInstance.scriptManager = new ScriptManager();
                environmentInstance.communicationPlugins.Load();
                environmentInstance.eventsManager = new Events.EventsManager();    

                environmentInstance.Project.ProjectLoaded += new System.EventHandler(environmentInstance.OnProjectLoad);
				//Should be called after all plugins
				environmentInstance.scriptManager.Initialize();
			}
		}

        void OnProjectLoad(object sender, System.EventArgs e)
        {
            foreach (String pl in communicationPlugins.PluginIds)
            {
                communicationPlugins[pl].LoadSettings();
            }
            eventsManager.LoadSettings();
            
            eventsSummary = new Events.EventsSummary(eventsManager);
        }


		public static void Deinitialize()
		{
			environmentInstance = null;
		}

		public static Env Current
		{
			get
			{
				if (environmentInstance == null)
					throw new System.NullReferenceException();

				return environmentInstance;
			}
		}

		Env() { }

		#endregion

		#region IEnvironment Members

		public string Version
		{
			get { return version; }
		}

		public ICommands Commands
		{
			get { return commands; }
		}

		public Window MainWindow
		{
			get	{ return mainWindow;	}
		}

		public FreeSCADA.Common.Project Project
		{
			get { return project; }
		}

		public EnvironmentMode Mode
		{
			get { return mode; }
		}

		#endregion

        public Events.EventsManager EventManager
        {
            get { return eventsManager; }

        }
        public Events.EventsSummary EventsSummary
        {
            get { return eventsSummary; }

        }
		public CommunationPlugs CommunicationPlugins
		{
			get { return communicationPlugins; }
		}

		
		public ScriptManager ScriptManager
		{
			get { return scriptManager; }
		}

		public void CreateNewProject()
		{
			if (project != null)
				project.Clear();
			else
				project = new FreeSCADA.Common.Project();
		}

		public ILogger Logger
		{
			get { return logger; }
			set { logger = value; }
		}


        IPrincipal _principal;

        public IPrincipal CurrentPrincipal
        {
            get
            {
                lock (this)
                {
                    return _principal;
                }
            }
            set
            {
                lock (this)
                {
                    _principal = value;
                }
            }
        }
    }
}
