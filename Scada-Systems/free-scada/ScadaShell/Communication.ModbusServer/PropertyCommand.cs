using FreeSCADA.Common;

namespace FreeSCADA.Communication.ModbusServer
{
	class PropertyCommand : BaseCommand
	{
		Plugin plugin;

		public PropertyCommand(Plugin plugin)
		{
			this.plugin = plugin;
			canExecute = true;
		}

		public override string Name { get { return StringConstants.PropertyCommandName; } }
		public override string Description { get { return StringConstants.PropertyCommandName; } }
		public override System.Drawing.Bitmap Icon { get { return null; } }

		public override void Execute(object o)
		{
			
		}
	}
}
