using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

namespace ScadaShell.Model
{
    public class ChartItemModel:IComparable
    {
        public DateTime Timestamp
        {
            get;
            protected set;
        }
        public double Value
        {
            get;
            protected set;
        }
        public ChartItemModel(DateTime d, double value)
        {
            Timestamp = d;
            Value = value;
        }



        public int CompareTo(object obj)
        {
            ChartItemModel chim = (ChartItemModel)obj;
            if (chim.Value > this.Value)
                return -1;
            if (chim.Value < this.Value)
                return 1;
            
            return 0;
        }
    }
}
