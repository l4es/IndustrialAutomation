using System;
using System.Globalization;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ScadaShell.Model;
using FreeSCADA.Archiver;
using System.Data;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events;
using FreeSCADA.Common;


namespace ScadaShell.DataAccess
{
    public class ArchiveAccess
    {
        public static List<ChartItemModel> GetChannelData(DateTime from,DateTime to, ChannelInfo channel)
        {
            List<ChartItemModel> cil = new List<ChartItemModel>();
            List<ChannelInfo> l = new List<ChannelInfo>();
            l.Add(channel);
            DataTable dt=ArchiverMain.Current.Accessor.GetChannelData(from, to, l);
            foreach (System.Data.DataRow row in dt.Rows)
            {
                DateTime date = new DateTime();
                double val = double.NaN;
                DateTime.TryParse(row["Time"].ToString(), out date);
                val = ArchiverAccessor.ParseDoublle(row["Value"].ToString());
                cil.Add(new ChartItemModel(date,val));
            }
            return cil;
        }
        public static List<IEvent> GetEvents(DateTime from, DateTime to, IEventsSource source)
        {
            List<IEvent> evl = new List<IEvent>();

            
 
           DataTable dt = ArchiverMain.Current.Accessor.GetEvents(from, to);
            foreach (System.Data.DataRow row in dt.Rows)
            {
                DateTime date = new DateTime();
                String text= row["Text"].ToString();
                int st;
                int ID;
                int.TryParse(row["EventID"].ToString() ,out ID);
                int.TryParse(row["Cause"].ToString() ,out st);
                EventState state=(EventState)st;
                DateTime.TryParse(row["Time"].ToString(), out date);
                try
                {
                    IEvent ev;
                    switch (state)
                    {
                        case EventState.Start:

                            evl.Insert(0, ev = new Event(Env.Current.EventManager[ID].Definition, source, text));
                            ev.StartTime = date;
                            break;
                        case EventState.End:
                            ev = evl.Where(evm => evm.Definition.Id == ID).FirstOrDefault();
                            if (ev == null || !ev.IsActive)
                                evl.Insert(0, ev = new Event(Env.Current.EventManager[ID].Definition, source, text));

                            ev.EndTime = date;
                            break;
                        case EventState.Ack:
                            ev = evl.Where(evm => evm.Definition.Id == ID).FirstOrDefault();
                            if (ev == null || !ev.IsActive)
                                evl.Insert(0, ev = new Event(Env.Current.EventManager[ID].Definition, source, text));
                            ev.AckTime = date;
                            break;
                    }
                }
                catch (EventNotExistsException e)
                {
                    IEvent ev;
                    ev = new Event(new CommonEventDefinition(e.Message), source, e.Message);
                    ev.StartTime=DateTime.Now;
                    ev.EndTime=DateTime.Now;
                    ev.AckTime=DateTime.Now;
                    evl.Insert(0,ev);
                    
                }
            }
            return evl;
        }

    }


}
