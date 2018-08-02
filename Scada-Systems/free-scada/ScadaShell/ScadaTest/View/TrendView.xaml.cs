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
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.Charts;
using Microsoft.Research.DynamicDataDisplay.Charts.Axes;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using System.Windows.Controls.DataVisualization.Charting;
using Microsoft.Research.DynamicDataDisplay.PointMarkers;
using ScadaShell.Model;


namespace ScadaShell.View
{
    /// <summary>
    /// Interaction logic for TrendView.xaml
    /// </summary>
    public partial class TrendView : UserControl
    {
        
        public TrendView()
        {
            InitializeComponent();
           // Loaded += new RoutedEventHandler(TrendView_Loaded);
            this.DataContextChanged += new DependencyPropertyChangedEventHandler(TrendView_DataContextChanged);
        }

        void TrendView_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            ViewModel.TrendViewModel tv = (ViewModel.TrendViewModel)DataContext;
            if(e.OldValue!=null)
                ((ViewModel.TrendViewModel)e.OldValue).PropertyChanged -=tv_PropertyChanged;
            tv.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(tv_PropertyChanged);
            tv.UpdateCommand.Execute(null);
            
        }

        /*void TrendView_Loaded(object sender, RoutedEventArgs e)
        {
            ViewModel.TrendViewModel tv = (ViewModel.TrendViewModel)DataContext;
            tv.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(tv_PropertyChanged);
            tv.Home.Execute(null);

        }*/

        void tv_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Data")
            {
                ViewModel.TrendViewModel tv = (ViewModel.TrendViewModel)sender;

                _trend.Children.RemoveAll(typeof(LineGraph));
                _trend.Viewport.Visible = new Rect(_timeAxis.ConvertToDouble(tv.From), tv.Min, _timeAxis.ConvertToDouble(tv.To) - _timeAxis.ConvertToDouble(tv.From), tv.Max);
                int i = 0;
                foreach (ChartModel cm in tv.Data)
                {

                    EnumerableDataSource<ChartItemModel> eds = new EnumerableDataSource<ChartItemModel>(cm.ChartData);
                    cm.ChartData.CollectionChanged += new System.Collections.Specialized.NotifyCollectionChangedEventHandler(TrendView_CollectionChanged);

                    eds.SetXYMapping(dfc => new Point(_timeAxis.ConvertToDouble(dfc.Timestamp), dfc.Value));

                    _trend.AddLineGraph(eds, new Pen(cm.Color, 2.0), new PenDescription(cm.Name));
                    
                 

                }
                
            }
       }

        void TrendView_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            ViewModel.TrendViewModel tv = (ViewModel.TrendViewModel)DataContext;
            _trend.Viewport.Visible = new Rect(_timeAxis.ConvertToDouble(tv.From),tv.Min , _timeAxis.ConvertToDouble(tv.To) - _timeAxis.ConvertToDouble(tv.From),tv.Max);

        }

    }
}

