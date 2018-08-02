using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows.Media;
using ScadaShell.Interfaces;
using ScadaShell.Model;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Windows.Data;
using FreeSCADA.Interfaces;
using FreeSCADA.Archiver;
using ScadaShell.DataAccess;
using System.Windows.Threading;
using Microsoft.Research.DynamicDataDisplay.DataSources;
namespace ScadaShell.ViewModel
{
    public class TrendViewModel:WorkspaceViewModel 
    {
        Rule _rule;
        DateTime _currentPos;
        int _window;
        int _step;
        DispatcherTimer dispatcherTimer;
        private bool _isOnline;
        Brush[] _colors = new Brush[] { Brushes.Red, Brushes.Green, Brushes.Blue, Brushes.Magenta, Brushes.Lime };

        public ObservableCollection<ChartModel> Data
        {
            get;
            protected set;
        }

        public String Name
        {
            get { return _rule.Name; }
        }
        public Boolean IsOnline
        {
            get{
                return _isOnline;
            }
            set
            {
                if (value == true)
                {
                    dispatcherTimer.Start();
                }
                else
                {
                    dispatcherTimer.Stop();
                }
                _isOnline = value;
            }
            
        }
        public DateTime From
        {
            get { return _currentPos.AddMinutes(-_window); }

        }
        public DateTime To
        {
            get { return _currentPos; }
            set { _currentPos = value.AddMinutes(-_window/2); }

        }
        public ICommand Forward
        {
            get
            {
                return new RelayCommand(
                    p => { OnForward(); },
                    p=> _currentPos.AddMinutes(_window)<DateTime.Now
                    );
            }
        }
        public ICommand Rewind
        {
            get
            {
                return new RelayCommand(p => { OnRewind(); });
            }
        }

        public ICommand ScaleUp
        {
            get
            {
                return new RelayCommand(
                    p => { OnScaleUp(); },
                    p =>  _window -_step > 0 );
            }
        }
        public ICommand ScaleDown
        {
            get
            {
                return new RelayCommand(p => { OnScaleDown(); });
                    
            }
        }

        public ICommand UpdateCommand
        {
            get
            {
                return new RelayCommand(p => { Update(); });

            }
        }
        public ICommand Home
        {
            get
            {
                return new RelayCommand(p => { OnHome(); });

            }
        }
        public double Min
        {
            get;
            protected set;
        }
        public double Max
        {
            get;
            protected set;
        }

        public TrendViewModel(Rule r,double min,double max)
        {
            _rule = r;
            _currentPos = DateTime.Now;
            _window = 60;
            _step = 15;
            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += new EventHandler(dispatcherTimer_Tick);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            Data = new ObservableCollection<ChartModel>();
            IsOnline = false;
            Min = min;
            Max = max;
            
        }
        
        void OnRewind()
        {
            _currentPos = _currentPos.AddMinutes(-_window);
            Update();
        }

        void OnForward()
        {
            _currentPos = _currentPos.AddMinutes(_window);
            Update();
        }

        void OnScaleDown()
        {
            RecalcWindow();
            _window+=_step;
            Update();
        }

        void OnScaleUp()
        {
            RecalcWindow();
            
            if (_window-_step > 0)
            {
                _window+=-_step;
                Update();
            }

        }
        void OnHome()
        {

            _window=60;
            _step=5;
            _currentPos=DateTime.Now;
            Update();
          
        }

        

        void dispatcherTimer_Tick(object sender, EventArgs e)
        {
            foreach (ChartModel chM in Data)
            {
                chM.ChartData.Add(
                    new ChartItemModel(
                        DateTime.Now, 
                        Convert.ToDouble(chM.Channel.Value)));
                if (chM.ChartData.Count > 300)
                    chM.ChartData.RemoveAt(0);
            }
            _currentPos = DateTime.Now.AddMinutes(_window / 2);
        }

        void Update()
        {
            dispatcherTimer.Stop();
            
            foreach(ChartModel cm in Data)
                cm.ChartData.RemoveCollectionChangedSubscribersAll();
            Data.Clear();
            int i=0;
            foreach (ChannelInfo ch in _rule.Channels)
            {
                Data.Add(
                    new ChartModel(ch,
                    ArchiveAccess.GetChannelData(_currentPos.AddMinutes(-_window), _currentPos, ch),
                    _colors[i]));
                   i++;
                    if (i ==_colors.Count())
                        break;
            }

            OnPropertyChanged("Data");
        }

        List<ChartItemModel> Interpolate(List<ChartItemModel> inL)
        {
            List<ChartItemModel> outL = new List<ChartItemModel>();
            if (inL.Count > 300)
            {

                TimeSpan ts = new TimeSpan(0, 0, _window * 60 / 300);
                DateTime scanDateTime = _currentPos.AddMinutes(-_window);

                for (; scanDateTime < _currentPos; scanDateTime = scanDateTime + ts)
                {
                    IEnumerable<ChartItemModel> cimc = inL.Where(tcim => tcim.Timestamp >= scanDateTime && tcim.Timestamp <= scanDateTime + ts);
                    ChartItemModel min = new ChartItemModel(scanDateTime,0), max=new ChartItemModel(scanDateTime+ts,0);
                    if (cimc.Count() > 0)
                    {

                        max = cimc.Max();
                        min = cimc.Min();

                    }

                    if (min.Timestamp > max.Timestamp)
                    {
                        outL.Add(max);
                        outL.Add(min);
                    }
                    else
                    {
                        outL.Add(min);
                        outL.Add(max);
                    }
                }
            }
            else 
                outL = inL;
            
            if (outL.Count == 0)
            {
                outL.Add(new ChartItemModel(_currentPos.AddMinutes(-_window), 0));
                outL.Add(new ChartItemModel(_currentPos, 0));
            }
          
            return outL;
        }

        void RecalcWindow()
        {
            _step = 60;
            if (_window <= 360)
                _step = 30;
            if (_window <= 180)
                _step = 15;
            if (_window <= 60)
                _step = 5;
            if (_window <= 15)
                _step = 3;
            if (_window <= 5)
                _step = 1;
            
        }

    }
}
