using System;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Threading;
using System.Windows.Input;
using ScadaShell.Infrastructure;
using FreeSCADA.Common;

namespace ScadaShell.ViewModel
{
    /// <summary>
    /// Represents an actionable item displayed by a View.
    /// </summary>
    public class LoginCommandViewModel : CommandViewModel

    {
        
        public override string DisplayName
        {
            get
            {
                return Env.Current.CurrentPrincipal.Identity.Name;
            }
            protected set
            {
                base.DisplayName = value;
            }
        }

        public LoginCommandViewModel()
        {
            
            Command = new RelayCommand(x => Login());
            LoginModel.UserChanged += new EventHandler(_loginModel_UserChanged);
        
        }
        
        void _loginModel_UserChanged(object sender, EventArgs e)
        {
            OnPropertyChanged("DisplayName");
        }
        
        override public ICommand Command { get; protected set; }

        void Login()
        {

            View.PopupView p = new View.PopupView(Double.NaN, Double.NaN, LoginModel);
        }

    }
}