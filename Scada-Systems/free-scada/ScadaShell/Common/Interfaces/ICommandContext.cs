using System.Collections.Generic;
using System.Windows.Input;

namespace FreeSCADA.Interfaces
{
	public interface ICommandContext
	{
		void AddCommand(ICommand cmd);
		void RemoveCommand(ICommand cmd);
        List<ICommand> GetCommands();
	}
}
 