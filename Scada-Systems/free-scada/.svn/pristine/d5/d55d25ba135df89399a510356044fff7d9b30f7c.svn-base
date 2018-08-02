using System;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using System.Collections.ObjectModel;
using System.Collections.Generic;


namespace ScadaShell.ViewModel
{
    /// <summary>
    /// This ViewModelBase subclass requests to be removed 
    /// from the UI when its CloseCommand executes.
    /// This class is abstract.
    /// </summary>
    public abstract class WorkspaceViewModel : ViewModelBase
    {
        #region Fields
        ReadOnlyCollection<CommandViewModel> _commands;

        #endregion // Fields

        #region Constructor

        protected WorkspaceViewModel()
        {
        }

        #endregion // Constructor

        
        #region WorkspaceCommands


        /// <summary>
        /// Returns a read-only list of commands 
        /// that the UI can display and execute.
        /// </summary>
        public ReadOnlyCollection<CommandViewModel> WorkspaceCommands
        {
            get
            {
                if (_commands == null)
                {
                    List<CommandViewModel> cmds = this.CreateCommands();
                    _commands = new ReadOnlyCollection<CommandViewModel>(cmds);
                }
                return _commands;
            }
        }

        virtual protected List<CommandViewModel> CreateCommands()
        {
            List<CommandViewModel> commands = new List<CommandViewModel>();
            commands.Add(new CommandViewModel("Закрити", CloseViewCommand));
            return commands;
        }
        

        #endregion

    }
}