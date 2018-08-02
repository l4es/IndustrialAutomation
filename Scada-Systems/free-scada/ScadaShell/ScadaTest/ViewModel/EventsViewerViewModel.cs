using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using FreeSCADA.Common;
using ScadaShell.Infrastructure;

namespace ScadaShell.ViewModel
{
    public class EventsViewerViewModel:WorkspaceViewModel
    {
        public Dictionary<String, EventsSummaryViewModel> EventsSummaries 
        { 
            get;
            protected set;
        }

        public EventsViewerViewModel()  
        {
            EventsSummaries = new Dictionary<string, EventsSummaryViewModel>();
            EventsSummaries["Поточні"] = new EventsSummaryViewModel(Env.Current.EventsSummary, new FreeSCADA.Common.Events.Filters.GlobalFilter());
            EventsSummaries["Всі"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.GlobalFilter());
            EventsSummaries["Технологичні"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.TechnologyFilter());
            EventsSummaries["Диагностичні"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.DiagnosticFilter());
            EventsSummaries["Дії Оператора"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.AuditFilter());

 //           EventsSummaries["Аварийные"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.LevelFilter(FreeSCADA.Interfaces.AlarmLevel.Red));
//            EventsSummaries["Предупредительные"] = new EventsSummaryViewModel(new ArchiveEventsSource(), new FreeSCADA.Common.Events.Filters.LevelFilter(FreeSCADA.Interfaces.AlarmLevel.Orange));
        }
        protected override void OnDispose()
        {
            foreach (IDisposable ds in EventsSummaries.Values)
                ds.Dispose();
            base.OnDispose();
        }
    }
}
