using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.Common;
using System.Globalization;

namespace FreeSCADA.Archiver
{
    public class ArchiverAccessor:FreeSCADA.Archiver.IArchiverAccessor
    {
        DbAccessor dbReader;
        ArchiverMain _archiver;
        public ArchiverAccessor(ArchiverMain arch)
        {
            _archiver = arch;
            

        }
        public bool Open()
        {
            dbReader = new DbAccessor(_archiver.DatabaseSettings);
            return dbReader.Open();
        }
        public void Close()
        {
            if (dbReader != null)
            {
                dbReader.Close();
            }
            
        }


        public DataTable GetEvents(DateTime from, DateTime to)
        {
            
            string datePattern = "yyyy-MM-dd HH:mm:ss";
            string query = "SELECT * FROM Events WHERE ";
            query += string.Format("Time >= '{0}' AND Time <= '{1}' ", from.ToString(datePattern), to.ToString(datePattern));
            query += " ORDER BY Time;";

            return dbReader.ExeсSelectCommand(query);
        }

        public KeyValuePair<double, double> GetChannelMinMax(List<ChannelInfo> channels)
        {
         

            string query = "SELECT min(Value),max(Value) FROM Channels WHERE";
            for (int i = 0; i < channels.Count; i++)
            {
                ChannelInfo ch = channels[i];
                query += string.Format("(PluginId='{0}' AND ChannelName='{1}')", ch.PluginId, ch.ChannelName);
                if (i != channels.Count - 1)
                {
                    query += " OR ";
                }
            }
            DataTable dt = dbReader.ExeсSelectCommand(query);
            KeyValuePair<double, double> res;
            if (dt.Rows.Count > 0)
            {
                res = new KeyValuePair<double, double>(ParseDoublle(dt.Rows[0][0].ToString()), ParseDoublle(dt.Rows[0][1].ToString()));

            }
            else
                res = new KeyValuePair<double, double>(0, 0);
            return res;

        }

        public DataTable GetChannelData(DateTime from, DateTime to, List<ChannelInfo> channels)
        {
            string datePattern = "yyyy-MM-dd HH:mm:ss";
            string query = "SELECT ChannelName, Time, Value FROM Channels WHERE ";
            query += string.Format("Time >= '{0}' AND Time <= '{1}' ", from.ToString(datePattern), to.ToString(datePattern));
            query += "AND (";
            for (int i = 0; i < channels.Count; i++)
            {
                ChannelInfo ch = channels[i];
                query += string.Format("(PluginId='{0}' AND ChannelName='{1}')", ch.PluginId, ch.ChannelName);
                if (i != channels.Count - 1)
                {
                    query += " OR ";
                }
            }
            query += ") ORDER BY Time;";

            return dbReader.ExeсSelectCommand(query);
        }
        public void Clean(int days)
        {
            string datePattern = "yyyy-MM-dd HH:mm:ss";
            DateTime dt = GetChannelsOlderDate(null).AddDays(days);
            string query = String.Format("DELETE FROM Channels WHERE Time<'{0}'", dt.ToString(datePattern));
            dbReader.ExeсCommand(query);
            //query = "VACUUM";
            //dbReader.ExeсCommand(query);

        }

        public DateTime GetChannelsOlderDate(List<ChannelInfo> channels)
        {

            string query = "SELECT min(Time) FROM Channels";
            if (channels != null)
            {
                query += "WHERE (";
                for (int i = 0; i < channels.Count; i++)
                {
                    ChannelInfo ch = channels[i];
                    query += string.Format("(PluginId='{0}' AND ChannelName='{1}')", ch.PluginId, ch.ChannelName);
                    if (i != channels.Count - 1)
                    {
                        query += " OR ";
                    }
                }
                query += ")";
            }

            DataTable dt = dbReader.ExeсSelectCommand(query);
            DateTime date = new DateTime();
            if (dt.Rows.Count > 0)
                DateTime.TryParse(dt.Rows[0].ItemArray[0].ToString(), out date);
            return date;

        }
        public static double ParseDoublle(string s)
        {
            String res = s.Replace(".", CultureInfo.CurrentCulture.NumberFormat.NumberDecimalSeparator);
            res = res.Replace(",", CultureInfo.CurrentCulture.NumberFormat.NumberDecimalSeparator);
            return Convert.ToDouble(res);
        }
    }
}
