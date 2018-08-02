using System;
using System.Data;
using System.Collections.Generic;

namespace FreeSCADA.Archiver
{
    public interface IArchiverAccessor
    {
        bool Open();
        void Close();
        DataTable GetChannelData(DateTime from, DateTime to, List<ChannelInfo> channels);
        KeyValuePair<double, double> GetChannelMinMax(List<ChannelInfo> channels);
        DateTime GetChannelsOlderDate(List<ChannelInfo> channels);
        DataTable GetEvents(DateTime from, DateTime to);
    }
}
