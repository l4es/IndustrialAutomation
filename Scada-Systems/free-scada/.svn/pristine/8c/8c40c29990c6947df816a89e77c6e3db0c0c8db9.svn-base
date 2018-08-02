using System.Windows;
using System.Security.Principal;
namespace FreeSCADA.Interfaces
{
	public enum EnvironmentMode
	{
		Designer,
		Runtime
	}

	public interface IEnvironment
	{
		string Version
		{
			get;
		}

		ICommands Commands
		{
			get;
		}

		Window MainWindow
		{
			get;
		}

		FreeSCADA.Common.Project Project
		{
			get;
		}
        IPrincipal CurrentPrincipal
        {
            get;
            set;
        }
		EnvironmentMode Mode
		{
			get;
		}
	}
}
