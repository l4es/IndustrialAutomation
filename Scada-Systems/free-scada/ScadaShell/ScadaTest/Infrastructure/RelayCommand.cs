using System;
using System.Windows;
using System.Diagnostics;
using System.Windows.Input;
using System.Security;

namespace ScadaShell.Infrastructure
{
    /// <summary>
    /// A command whose sole purpose is to 
    /// relay its functionality to other
    /// objects by invoking delegates. The
    /// default return value for the CanExecute
    /// method is 'true'.
    /// </summary>
    public class RelayCommand : ICommand
    {
        #region Fields

        readonly Action<object> _execute;
        readonly Predicate<object> _canExecute;
private  string _access;
        
        #endregion // Fields

        #region Constructors

        /// <summary>
        /// Creates a new command that can always execute.
        /// </summary>
        /// <param name="execute">The execution logic.</param>
        public RelayCommand(Action<object> execute)
            : this(execute, null)
        {
        }

        /// <summary>
        /// Creates a new command.
        /// </summary>
        /// <param name="execute">The execution logic.</param>
        /// <param name="canExecute">The execution status logic.</param>

        public RelayCommand(Action<object> execute, Predicate<object> canExecute):this(execute,canExecute,"View")
        {
        }
        public RelayCommand(Action<object> execute, Predicate<object> canExecute,String access)
        {
            if (execute == null)
                throw new ArgumentNullException("execute");
            _access=access;
            _execute = execute;
            _canExecute = canExecute;           
        }

        #endregion // Constructors

        #region ICommand Members

        [DebuggerStepThrough]
        virtual public bool CanExecute(object parameter)
        {
            bool res =_canExecute == null ? true : _canExecute(parameter);
            if(!Security.SecurityHelper.Authorized(_access))
                res=false;

            return res;
            
            /*if (_canExecute == null)
                return true;
            bool temp = _canExecute(parameter);
            if (canExecuteCache != temp)
            {
                canExecuteCache = temp;
                if (CanExecuteChanged != null)
                {
                    CanExecuteChanged(this, new EventArgs());
                }
            }
            return canExecuteCache;*/
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        virtual public void Execute(object parameter)
        {
            try
            {

                _execute(parameter);
            }
            catch(Infrastructure.ValidationException e)
            {
                MessageBox.Show(e.Message);
            }
            
        }

        #endregion // ICommand Members
    }
}