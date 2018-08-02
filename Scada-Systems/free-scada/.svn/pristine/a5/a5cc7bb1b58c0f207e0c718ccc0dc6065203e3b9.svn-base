using System;
using System.Windows.Input;
using System.Windows;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security;
using ScadaShell.Infrastructure;

namespace ScadaShell.ViewModel
{
    public class LoginViewModel: WorkspaceViewModel
    {
        ICommand _loginCommand;


        public LoginViewModel()
        {
            User = "anonimus";
            Password = "anonimus";
            
            
        }

        
        
        public List<string> Users
        {
            get{

                return Security.SecurityHelper.GetAllUsers();
            }
            
        }
        public String User
        {
            get;
            set;
        }

        public String Password
        {
            get;
            set;
        }

        public ICommand LoginCommand
        {
            get
            {
                if (_loginCommand == null)
                {
                    _loginCommand = new RelayCommand(
                        param => { Login(User,(String)param); },
                        param => { return true; }
                        );
                }
                return _loginCommand;
            }
        }
        
        public  void Login( string user, string password)
        {
            if (Security.SecurityHelper.Authenticate(user, password))
            {
                OnUserChanged();
                OnRequestClose();
                
            }
            else
            {
                OnRequestClose();
                MessageBox.Show("Incorect User/Password combination", "Login Error");
            }
            
        }
        #region UserChanged [event]

        /// <summary>
        /// Raised when this workspace should be removed from the UI.
        /// </summary>
        public event EventHandler UserChanged;

        protected void OnUserChanged()
        {
            EventHandler handler = this.UserChanged;
            if (handler != null)
                handler(this, EventArgs.Empty);
        }

        #endregion // UserChanged [event]

    }
}
