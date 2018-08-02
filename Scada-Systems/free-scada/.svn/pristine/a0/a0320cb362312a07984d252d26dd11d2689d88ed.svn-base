using System;
using System.Windows;
using System.Windows.Controls;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ScadaShell.Infrastructure
{
    /*public static class PasswordHelper
    {
        public static readonly DependencyProperty PasswordProperty =
            DependencyProperty.RegisterAttached("Password",
            typeof(string), typeof(PasswordHelper),
            new FrameworkPropertyMetadata(string.Empty, OnPasswordPropertyChanged));

        public static readonly DependencyProperty AttachProperty =
            DependencyProperty.RegisterAttached("Attach",
            typeof(bool), typeof(PasswordHelper), new PropertyMetadata(false, Attach));

        private static readonly DependencyProperty IsUpdatingProperty =
           DependencyProperty.RegisterAttached("IsUpdating", typeof(bool),
           typeof(PasswordHelper));


        public static void SetAttach(DependencyObject dp, bool value)
        {
            dp.SetValue(AttachProperty, value);
        }

        public static bool GetAttach(DependencyObject dp)
        {
            return (bool)dp.GetValue(AttachProperty);
        }

        public static string GetPassword(DependencyObject dp)
        {
            return (string)dp.GetValue(PasswordProperty);
        }

        public static void SetPassword(DependencyObject dp, string value)
        {
            dp.SetValue(PasswordProperty, value);
        }

        private static bool GetIsUpdating(DependencyObject dp)
        {
            return (bool)dp.GetValue(IsUpdatingProperty);
        }

        private static void SetIsUpdating(DependencyObject dp, bool value)
        {
            dp.SetValue(IsUpdatingProperty, value);
        }

        private static void OnPasswordPropertyChanged(DependencyObject sender,
            DependencyPropertyChangedEventArgs e)
        {
            PasswordBox passwordBox = sender as PasswordBox;
            passwordBox.PasswordChanged -= PasswordChanged;

            if (!(bool)GetIsUpdating(passwordBox))
            {
                passwordBox.Password = (string)e.NewValue;
            }
            passwordBox.PasswordChanged += PasswordChanged;
        }

        private static void Attach(DependencyObject sender,
            DependencyPropertyChangedEventArgs e)
        {
            PasswordBox passwordBox = sender as PasswordBox;

            if (passwordBox == null)
                return;

            if ((bool)e.OldValue)
            {
                passwordBox.PasswordChanged -= PasswordChanged;
            }

            if ((bool)e.NewValue)
            {
                passwordBox.PasswordChanged += PasswordChanged;
            }
        }

        private static void PasswordChanged(object sender, RoutedEventArgs e)
        {
            PasswordBox passwordBox = sender as PasswordBox;
            SetIsUpdating(passwordBox, true);
            SetPassword(passwordBox, passwordBox.Password);
            SetIsUpdating(passwordBox, false);
        }
        
    }*/
    public static class PasswordBoxAssistant
  {
      public static readonly DependencyProperty BoundPassword =
          DependencyProperty.RegisterAttached("BoundPassword", typeof(string), typeof(PasswordBoxAssistant), new PropertyMetadata(string.Empty, OnBoundPasswordChanged));
 
      public static readonly DependencyProperty BindPassword = DependencyProperty.RegisterAttached(
          "BindPassword", typeof (bool), typeof (PasswordBoxAssistant), new PropertyMetadata(false, OnBindPasswordChanged));
 
      private static readonly DependencyProperty UpdatingPassword =
          DependencyProperty.RegisterAttached("UpdatingPassword", typeof(bool), typeof(PasswordBoxAssistant), new PropertyMetadata(false));
 
      private static void OnBoundPasswordChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
      {
          PasswordBox box = d as PasswordBox;
 
          // only handle this event when the property is attached to a PasswordBox
          // and when the BindPassword attached property has been set to true
          if (d == null || !GetBindPassword(d))
          {
              return;
          }
 
          // avoid recursive updating by ignoring the box's changed event
          box.PasswordChanged -= HandlePasswordChanged;
 
          string newPassword = (string)e.NewValue;
 
          if (!GetUpdatingPassword(box))
          {
              box.Password = newPassword;
          }
 
          box.PasswordChanged += HandlePasswordChanged;
      }
 
      private static void OnBindPasswordChanged(DependencyObject dp, DependencyPropertyChangedEventArgs e)
      {
          // when the BindPassword attached property is set on a PasswordBox,
          // start listening to its PasswordChanged event
 
          PasswordBox box = dp as PasswordBox;
 
          if (box == null)
          {
              return;
          }
 
          bool wasBound = (bool)(e.OldValue);
          bool needToBind = (bool)(e.NewValue);
 
          if (wasBound)
          {
              box.PasswordChanged -= HandlePasswordChanged;
          }
 
          if (needToBind)
          {
              box.PasswordChanged += HandlePasswordChanged;
          }
      }
 
      private static void HandlePasswordChanged(object sender, RoutedEventArgs e)
      {
          PasswordBox box = sender as PasswordBox;
 
          // set a flag to indicate that we're updating the password
          SetUpdatingPassword(box, true);
          // push the new password into the BoundPassword property
          SetBoundPassword(box, box.Password);
          SetUpdatingPassword(box, false);
      }
 
      public static void SetBindPassword(DependencyObject dp, bool value)
      {
          dp.SetValue(BindPassword, value);
      }
 
      public static bool GetBindPassword(DependencyObject dp)
      {
          return (bool)dp.GetValue(BindPassword);
      }
 
      public static string GetBoundPassword(DependencyObject dp)
      {
          return (string)dp.GetValue(BoundPassword);
      }
 
      public static void SetBoundPassword(DependencyObject dp, string value)
      {
          dp.SetValue(BoundPassword, value);
      }
 
      private static bool GetUpdatingPassword(DependencyObject dp)
      {
          return (bool)dp.GetValue(UpdatingPassword);
      }
 
      private static void SetUpdatingPassword(DependencyObject dp, bool value)
      {
          dp.SetValue(UpdatingPassword, value);
      }
  }
}
