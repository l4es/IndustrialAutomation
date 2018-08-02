using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace ScadaShell.Infrastructure
{
    public class IsPressedButton : Button
    {
        public new static readonly DependencyProperty IsPressedProperty =
          DependencyProperty.Register("IsPressed", typeof(bool), typeof(IsPressedButton),
          new FrameworkPropertyMetadata(false, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

        public new bool IsPressed
        {
            get { return (bool)GetValue(IsPressedProperty); }
            set { SetValue(IsPressedProperty, value); }
        }

        protected override void OnIsPressedChanged(DependencyPropertyChangedEventArgs e)
        {
            base.OnIsPressedChanged(e);

            IsPressed = (bool)e.NewValue;
        }
    }

}
