using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Windows.Controls;
using System.Windows.Markup;
using System.Windows.Input;
using System.Windows.Data;

namespace ScadaShell.Infrastructure
{
    public static class EnterPressedInputBindingsEstension
    {

        public static readonly DependencyProperty UpdatePropertySourceWhenEnterPressedProperty = DependencyProperty.RegisterAttached(
            "UpdatePropertySourceWhenEnterPressed", typeof(DependencyProperty), typeof(EnterPressedInputBindingsEstension), new PropertyMetadata(null, OnUpdatePropertySourceWhenEnterPressedPropertyChanged));

        static EnterPressedInputBindingsEstension()
        {

        }

        public static void SetUpdatePropertySourceWhenEnterPressed(DependencyObject dp, DependencyProperty value)
        {
            dp.SetValue(UpdatePropertySourceWhenEnterPressedProperty, value);
        }

        public static DependencyProperty GetUpdatePropertySourceWhenEnterPressed(DependencyObject dp)
        {
            return (DependencyProperty)dp.GetValue(UpdatePropertySourceWhenEnterPressedProperty);
        }

        private static void OnUpdatePropertySourceWhenEnterPressedPropertyChanged(DependencyObject dp, DependencyPropertyChangedEventArgs e)
        {
            UIElement element = dp as UIElement;

            if (element == null)
            {
                return;
            }

            if (e.OldValue != null)
            {
                element.PreviewKeyDown -= HandlePreviewKeyDown;
            }

            if (e.NewValue != null)
            {
                element.PreviewKeyDown += new KeyEventHandler(HandlePreviewKeyDown);
            }
        }


        static void HandlePreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                DoUpdateSource(e.Source);
            }
        }

        static void DoUpdateSource(object source)
        {
            DependencyProperty property =
                GetUpdatePropertySourceWhenEnterPressed(source as DependencyObject);

            if (property == null)
            {
                return;
            }

            UIElement elt = source as UIElement;

            if (elt == null)
            {
                return;
            }

            BindingExpression binding = BindingOperations.GetBindingExpression(elt, property);

            if (binding != null)
            {
                binding.UpdateSource();
            }
        }
    }

    public static class MousceClickInputBindingsEstension
    {

        public static readonly DependencyProperty AttachedCommandPropertry = DependencyProperty.RegisterAttached(
            "AttachedCommand", typeof(ICommand), typeof(MousceClickInputBindingsEstension), new PropertyMetadata(null, OnCommandPropertyChanged));

        static MousceClickInputBindingsEstension()
        {

        }

        public static void SetAttachedCommand(DependencyObject dp, ICommand value)
        {
            dp.SetValue(AttachedCommandPropertry, value);
        }

        public static ICommand GetAttachedCommand(DependencyObject dp)
        {
            return (ICommand)dp.GetValue(AttachedCommandPropertry);
        }

        private static void OnCommandPropertyChanged(DependencyObject dp, DependencyPropertyChangedEventArgs e)
        {
            UIElement element = dp as UIElement;

            if (element == null)
            {
                return;
            }

            if (e.OldValue != null)
            {
                element.PreviewMouseLeftButtonDown -= HandlePreviewMouseDown;
            }

            if (e.NewValue != null)
            {
                element.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(HandlePreviewMouseDown);
            }
        }


        static void HandlePreviewMouseDown(object sender, MouseEventArgs e)
        {
              DoUpdateSource(sender);
        }

        static void DoUpdateSource(object source)
        {
            ICommand command =
                GetAttachedCommand(source as DependencyObject);

            if (command == null)
            {
                return;
            }

            UIElement elt = source as UIElement;

            if (elt == null)
            {
                return;
            }
            if (command.CanExecute(elt))
                command.Execute(elt);

            
        }
    }

}
