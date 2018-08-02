using System;
using System.Windows.Controls;
using System.Windows;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ScadaShell.Infrastructure
{
    class AskRelayCommand:RelayCommand
    {
         public AskRelayCommand(Action<object> execute)
            : this(execute, null)
        {
        }

        /// <summary>
        /// Creates a new command.
        /// </summary>
        /// <param name="execute">The execution logic.</param>
        /// <param name="canExecute">The execution status logic.</param>

        public AskRelayCommand(Action<object> execute, Predicate<object> canExecute):this(execute,canExecute,"View")
        {
        }
        public AskRelayCommand(Action<object> execute, Predicate<object> canExecute,String access):base(execute,canExecute,access)
        {
        }
        public override void Execute(object parameter)
        {
            MessageBoxResult res= MessageBox.Show ("Вы уверены?", "Выполнить изменение", MessageBoxButton.YesNo);
            if(res==MessageBoxResult.Yes)
                base.Execute(parameter);
        }

    }
}
