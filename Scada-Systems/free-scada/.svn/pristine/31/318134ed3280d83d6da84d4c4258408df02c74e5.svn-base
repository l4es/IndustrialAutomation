using System;
using System.Globalization;
using System.Collections.Generic;
using System.Data;
using System.Data.Common;
using System.Threading;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;


namespace FreeSCADA.Archiver
{
    public class ArchiverMain : IDisposable
    {
        RulesSettings channelSettings;
        DatabaseSettings databaseSettings;
        Project _project;
        DbWriter dbWriter;

        Thread channelUpdaterThread;

        #region Initialization and singleton implementation
        static ArchiverMain _instance = null;
        static public ArchiverMain Current
        {
            get
            {
                if (_instance == null)
                    _instance = new ArchiverMain(Env.Current.Project);
                return _instance;

            }
        }

        #endregion
        public IArchiverAccessor Accessor
        {
            get;
            protected set;
        }



        ArchiverMain(Project prj)
        {
            _project = prj;
            channelSettings = new RulesSettings(_project);
            databaseSettings = new DatabaseSettings(_project);
            databaseSettings.Load();
            channelSettings.Load();
            Accessor = new ArchiverAccessor(this);
            
            Accessor.Open();

            SubscribeEvents();
            if (Env.Current.Mode == EnvironmentMode.Designer)
            {
                ICommandContext context = Env.Current.Commands.GetContext(PredefinedContexts.Project);
                context.AddCommand(new PropertyCommand());
            }
        }




        public void Dispose()
        {
            channelSettings.Clear();
            UnsubscribeEvents();
            Stop();
            Accessor = null;
        }


        void SubscribeEvents()
        {
            Env.Current.EventsSummary.EventChanged += new EventChanged(EventsSummary_EventChanged);
        }

        void EventsSummary_EventChanged(IEventDefinition ev, IEventState state)
        {

            dbWriter.WriteEvent(ev, state);

        }


        void UnsubscribeEvents()
        {
            Env.Current.EventsSummary.EventChanged -= new EventChanged(EventsSummary_EventChanged);
        }

        public RulesSettings RulesSettings
        {
            get
            {
                return channelSettings;
            }
        }

        public DatabaseSettings DatabaseSettings
        {
            get
            {
                return databaseSettings;
            }
        }

        public bool IsRunning
        {
            get { return channelUpdaterThread != null && channelUpdaterThread.IsAlive; }
        }


        private static void ChannelUpdaterThreadProc(object obj)
        {
            ArchiverMain self = (ArchiverMain)obj;

            try
            {
                for (; ; )
                {

                    foreach (Rule rule in self.channelSettings.Rules)
                    {
                        if (rule.Enable)
                        {
                            foreach (BaseCondition cond in rule.Conditions)
                                cond.Process();

                            if (rule.Archive)
                                self.dbWriter.WriteChannels(rule.Channels);
                        }
                    }


                    Thread.Sleep(100);
                }
            }
            catch (ThreadAbortException)
            {

            }
            catch (System.Data.Common.DbException e)
            {
                
                self.dbWriter.Close();
                self.Accessor.Close();
                throw e;

            }
            if (self.dbWriter != null)
                self.dbWriter.Close();
        }

        public bool Start()
        {
            dbWriter = new DbWriter(databaseSettings);
            if (dbWriter.Open() == false)
                return false;
            if (!Accessor.Open())
                return false;
            channelUpdaterThread = new Thread(new ParameterizedThreadStart(ChannelUpdaterThreadProc));
            channelUpdaterThread.Start(this);
            return IsRunning;
        }

        public void Stop()
        {
            if (channelUpdaterThread != null)
            {
                channelUpdaterThread.Abort();
                channelUpdaterThread.Join();
                channelUpdaterThread = null;
                Accessor.Close();
                if (dbWriter != null)
                    dbWriter.Close();
             
            }

        }



    }


}
