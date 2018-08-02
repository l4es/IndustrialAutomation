using System;
using System.Windows.Input;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows.Media;
using ScadaShell.Infrastructure;
using ScadaShell.DataAccess;
using FreeSCADA.Interfaces;
using FreeSCADA.Common.Events;
using FreeSCADA.Common;
using FreeSCADA.Common.Events.Filters;
using System.Collections.Specialized;

namespace ScadaShell.ViewModel
{
    public class EventsSummaryViewModel : WorkspaceViewModel
    {
        IEventsSource _evsource;
        ObservableCollectionEx<EventViewModel> _events; 
        IEventFilter _filter;
        public SolidColorBrush StateColor
        {
            get;
            protected set;
        }
        public EventViewModel SelectedEvent
        {
            get;
            set;
        }
        public ICommand ShowTrend
        {
            get
            {
                return new RelayCommand(p =>
                    {
                        EventViewModel evm=p as  EventViewModel;
                        if(evm!=null)
                            ((App)App.Current).MainWindowModel.SetActiveWorkspace(new TrendsViewModel(evm.StartTime));
                    });
            }
        }
        public ObservableCollectionEx<EventViewModel> Events
        {
            get {
                return _events; 
            }
        }
        public EventsSummaryViewModel(IEventsSource es, IEventFilter filter)
        {
            _evsource = es;
            _events = new ObservableCollectionEx<EventViewModel>(_evsource.Events.Where(ev=> filter.Apply(ev.Definition)).Select(ev => new EventViewModel(ev)));
            //_evsource.EventChanged += new EventChanged(_evsource_EventChanged);
            _evsource.Events.CollectionChanged += new System.Collections.Specialized.NotifyCollectionChangedEventHandler(ActiveEvents_CollectionChanged);
            _filter = filter;
            StateColor= Brushes.Green;
            System.Timers.Timer _timer = new System.Timers.Timer(1000);
            _timer.Elapsed += new System.Timers.ElapsedEventHandler(_timer_Elapsed);
            _timer.Start();
        }

        void _evsource_EventChanged(IEventDefinition ev, IEventState state)
        {
            
        }

        
        void _timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            StateColor = Brushes.Gray;
            foreach (EventViewModel ev in _events)
                if (!ev.Event.IsAcked)
                {
                    switch (ev.Definition.Level)
                    {
                        case AlarmLevel.Green:
                            if(StateColor==Brushes.Gray)
                                StateColor = Brushes.Green;
                            break;
                        case AlarmLevel.Yellow:
                            if (StateColor == Brushes.Gray || StateColor == Brushes.Green)
                                StateColor = Brushes.Yellow;
                            break;
                        case AlarmLevel.Red:
                            StateColor = Brushes.Red;
                            break;
                        default:
                            StateColor = Brushes.Gray;
                            break;
                    }
                    
                }
            OnPropertyChanged("StateColor");
        }

        void ActiveEvents_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            switch (e.Action)
            {
                case   NotifyCollectionChangedAction.Add:
                    foreach (IEvent ev in e.NewItems)
                        if (_filter.Apply(ev.Definition))
                            _events.Insert(0,new EventViewModel(ev));
                    break;
                case NotifyCollectionChangedAction.Remove:
                    foreach (IEvent ev in e.OldItems)
                        foreach(EventViewModel evm in _events)
                            if (evm.Definition.Id == ev.Definition.Id)
                            {
                                _events.Remove(evm);
                                break;
                            }
                            
                    break;
                    
                case NotifyCollectionChangedAction.Reset:
                    _events.Clear();
                    break;
            }
            
        }
        protected override void OnDispose()
        {
            _evsource.Events.CollectionChanged -= new System.Collections.Specialized.NotifyCollectionChangedEventHandler(ActiveEvents_CollectionChanged);
            foreach (IDisposable ids in Events)
                ids.Dispose();
            base.OnDispose();
        }

    }
}
