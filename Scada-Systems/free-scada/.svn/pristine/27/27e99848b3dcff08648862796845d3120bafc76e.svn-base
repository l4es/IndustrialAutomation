using System;
using System.Drawing;
using FreeSCADA.Interfaces;
using System.Windows.Input;

namespace FreeSCADA.Common
{
    public enum CommandType
    {
        Separator,
        Submenu,
        Standard,
        DropDownBox
    }
    public abstract class BaseCommand : ICommand
	{
		protected bool canExecute = false;
		int priority = -1;

		public BaseCommand()
		{
		}

		public event EventHandler CanExecuteChanged;

		public virtual string Name { get { throw new NotImplementedException(); } }
		public virtual string Description { get { throw new NotImplementedException(); } }
		public virtual Bitmap Icon { get { return null; } }
		public virtual CommandType Type { get { return CommandType.Standard; } }
		public virtual ICommandItems DropDownItems { get { return null; } }
		public int Priority
		{
			get
			{
				return priority;
			}
			set
			{
				priority = value;
			}
		}
		public bool CanExecute(object o)
		{
			 return canExecute;
		}

		public virtual void Execute(object o)
		{
		}

		protected void FireCanExecuteChanged()
		{
			if (CanExecuteChanged != null)
				CanExecuteChanged(this, new EventArgs());
		}
	}
}
