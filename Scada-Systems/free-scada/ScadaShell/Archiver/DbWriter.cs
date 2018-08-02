using System;
using System.Collections.Generic;
using System.Data.Common;
using FreeSCADA.Common;
using FreeSCADA.Interfaces;

namespace FreeSCADA.Archiver
{
    class DbWriter
    {
        DbAccessor _accessor;
        public DbWriter(DatabaseSettings settings)
            : this(new DbAccessor(settings))
        {
        }
        public DbWriter(DbAccessor accesor)
        {
            _accessor = accesor;
            
        }

        public bool Open()
        {
            if(!_accessor.Open())
                return false;
            if (_accessor.IsTableExists("Channels") == false)
            {
                string cmd;

                cmd = "CREATE TABLE Channels (";
                cmd += "PluginId VARCHAR(255), ";
                cmd += "ChannelName VARCHAR(255), ";
                cmd += "Time DATETIME, ";
                cmd += "Value VARCHAR(255), ";
                cmd += "Status SMALLINT";
                cmd += ");";
                _accessor.ExeсCommand(cmd);

                _accessor.ExeсCommand("CREATE INDEX IF NOT EXISTS ChIndex ON Channels (Time);");
                _accessor.ExeсCommand("CREATE TRIGGER fifo_limit_channels after insert on channels\n"
                +"WHEN 100<(SELECT Count() FROM channels WHERE time<= datetime('now','localtime','-45 day'))"
                + "BEGIN\n"
                + "DELETE FROM channels WHERE Time<= datetime('now','localtime','-45 day');"
                + "END;"
                );
               // _accessor.ExeсCommand("PRAGMA max_page_count = 300000");
            }
            if (_accessor.IsTableExists("Events") == false)
            {
                string cmd;
                cmd = "CREATE TABLE Events (";
                cmd += "EventID INT, ";
                cmd += "Text VARCHAR(255), ";
                cmd += "Time DATETIME, ";
                cmd += "Cause SMALLINT";
                cmd += ");";
                _accessor.ExeсCommand(cmd);

                _accessor.ExeсCommand("CREATE INDEX IF NOT EXISTS EvIndex ON Events (Time);");
                _accessor.ExeсCommand("CREATE TRIGGER fifo_limit_events after insert on events\n"
                   + "WHEN 100<(SELECT Count() FROM channels WHERE time<= datetime('now','localtime','-45 day'))"
                   + "BEGIN\n"
                   + "DELETE FROM events WHERE Time<= datetime('now','localtime','-45 day');"
                   + "END;"
                   );
            }
            _accessor.ExeсCommand("PRAGMA journal_mode=off");
            return true;
        }



        public void Close()
        {
            _accessor.Close();
        }
        public bool WriteEvent(IEventDefinition def, IEventState state)
        {
            lock (_accessor)
            {
                string cmd;
                cmd = "INSERT INTO Events VALUES (";
                cmd += "'" + def.Id + "', ";
                cmd += "'" + state.FormatText(def.Text) + "', ";
                cmd += "'" + state.Time.ToString("yyyy-MM-dd HH:mm:ss.fff") + "', ";
                cmd += "'" + (int)state.GetCause() + "');";

                _accessor.ExeсCommand(cmd);
            }
        
            return true;
        }
        public bool WriteChannels(List<ChannelInfo> channels)
        {
            string cmd;
            lock (_accessor)
            {
                _accessor.BeginTransaction();
                foreach (ChannelInfo channelInfo in channels)
                {
                    IChannel[] pluginChannels = Env.Current.CommunicationPlugins[channelInfo.PluginId].Channels;
                    IChannel channel = null;
                    foreach (IChannel ch in pluginChannels)
                    {
                        if (ch.Name == channelInfo.ChannelName)
                        {
                            channel = ch;
                            break;
                        }
                    }

                    if (channel != null && channel.Value != null)
                    {
                        cmd = "INSERT INTO Channels VALUES (";
                        cmd += "'" + channel.PluginId + "', ";
                        cmd += "'" + channel.Name + "', ";
                        cmd += "'" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + "', ";
                        cmd += "'" + channel.Value.ToString() + "', ";
                        cmd += ((int)channel.StatusFlags).ToString() + ");";

                        _accessor.ExeсCommand(cmd);

                    }

                }
                _accessor.EndTransaction();
            }
            return true;
        }


    }
}
