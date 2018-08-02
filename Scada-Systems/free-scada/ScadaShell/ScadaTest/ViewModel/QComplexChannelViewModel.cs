using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;
using ScadaShell.Model;
using FreeSCADA.Interfaces;

namespace ScadaShell.ViewModel
{
    class QComplexChannelViewModel : ComplexParametrViewModel
    {
        public QComplexChannelViewModel(ComplexParametrElement _chel)
            : base(_chel)
        {

        }

        public override System.Windows.Media.Brush Color
        {
            get
            {
                if (Value.Channel.StatusFlags !=  ChannelStatusFlags.Good)
                {
                    return Brushes.Gray;
                }
                if (Convert.ToBoolean(Break.Value) == true)
                {
                    return Brushes.Blue;
                }
                if (Convert.ToBoolean(Error.Value) == true)
                {
                    return Brushes.Cyan;
                }
                if (Convert.ToBoolean(SpHigh.Value) == true)
                {
                    return Brushes.Red;
                }
                if (Convert.ToBoolean(SpLow.Value) == true)
                {
                    return Brushes.Orange;
                }


                return Brushes.Green;
            }

        }
    }
}
