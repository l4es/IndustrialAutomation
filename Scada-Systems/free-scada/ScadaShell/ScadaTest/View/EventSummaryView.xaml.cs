using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;

namespace ScadaShell.View
{
    /// <summary>
    /// Interaction logic for EventSummaryView.xaml
    /// </summary>
    public partial class EventSummaryView : UserControl
    {
        Dictionary<String, String> props = new Dictionary<string, string>() { { "Начало", "StartTime" }, { "Конец", "EndTime" }, { "Подтверждение", "AckTime" } };

        public EventSummaryView()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(EventSummaryView_Loaded);
        }

        void EventSummaryView_Loaded(object sender, RoutedEventArgs e)
        {
            Sort("StartTime", ListSortDirection.Ascending);
            _lastDirection = ListSortDirection.Ascending;

            ViewModel.ViewModelBase vm = (ViewModel.ViewModelBase)DataContext;
            vm.UpdateView += new ScadaShell.ViewModel.ViewModelBase.UpdateViewDelegate(vm_UpdateView);
        }

        void vm_UpdateView(ViewModel.ViewModelBase source, string state)
        {
            Sort("StartTime", ListSortDirection.Ascending);
        }

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            foreach (Object o in e.AddedItems)
            {
                ICommand cmd = (ICommand)o.GetType().GetProperty("AckEvent").GetValue(o, null);
                cmd.Execute(null);
            }
        }

        GridViewColumnHeader _lastHeaderClicked = null;
        ListSortDirection _lastDirection = ListSortDirection.Ascending;

        void GridViewColumnHeaderClickedHandler(object sender,
                                                RoutedEventArgs e)
        {
            GridViewColumnHeader headerClicked =e.OriginalSource as GridViewColumnHeader;
            ListSortDirection direction;

            if (headerClicked != null)
            {
                string header = headerClicked.Column.Header as string;
                if (!props.ContainsKey(header))
                    return;
                header = props[header];
                if (headerClicked.Role != GridViewColumnHeaderRole.Padding)
                {
                    if (headerClicked != _lastHeaderClicked)
                    {
                        direction = ListSortDirection.Ascending;
                    }
                    else
                    {
                        if (_lastDirection == ListSortDirection.Ascending)
                        {
                            direction = ListSortDirection.Descending;
                        }
                        else
                        {
                            direction = ListSortDirection.Ascending;
                        }
                    }

        
                    Sort(header, direction);

                    if (direction == ListSortDirection.Ascending)
                    {
                        headerClicked.Column.HeaderTemplate =
                          Resources["HeaderTemplateArrowUp"] as DataTemplate;
                    }
                    else
                    {
                        headerClicked.Column.HeaderTemplate =
                          Resources["HeaderTemplateArrowDown"] as DataTemplate;
                    }

                    // Remove arrow from previously sorted header
                    if (_lastHeaderClicked != null && _lastHeaderClicked != headerClicked)
                    {
                        _lastHeaderClicked.Column.HeaderTemplate = null;
                    }


                    _lastHeaderClicked = headerClicked;
                    _lastDirection = direction;
                }
            }

        }
        private void Sort(string sortBy, ListSortDirection direction)
        {
            
            ICollectionView dataView =
              CollectionViewSource.GetDefaultView(EventsListView.ItemsSource);

            dataView.SortDescriptions.Clear();
            SortDescription sd = new SortDescription(sortBy, direction);
            dataView.SortDescriptions.Add(sd);
            dataView.Refresh();
        }
    }
}