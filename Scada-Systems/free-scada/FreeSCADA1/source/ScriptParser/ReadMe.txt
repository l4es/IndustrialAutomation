Формат скрипта отчета

Report =
{
	Name	= "";			//Название отчета
	Author	= "";			//Автор (необязательное поле)
	Left	= 30;
	Right	= 15;
	Top		= 15;
	Bottom	= 15;
	
	Band =					//Раздел 1
	{
		Type = (tHeader | tData | tSummary);	// Тип раздела
		DataSource = "";						// Источник данных
		
		Label = {
			Text = "";
			Position = {x,y};
			Position.x = 0;
			Position.y = 0;
			Font = 
			{
				Name = "Arial";
				Height = 24;
				Weight = 400;
				Italic = false;
				Underline = false;
			};
		}
		Field = 
		{
			Data = "";
			Position = {x,y};
			Position.x = 0;
			Position.y = 0;
			Font = 
			{
				Name = "Times New Roman";
				Height = 24;
				Weight = 400;
				Italic = false;
				Underline = false;
			}
		}
		Line =
		{
			Position = {x1,y1,x2,y2};
			x1 = 0;
			y1 = 0;
			x2 = 0;
			y2 = 0;
			Width = 0;
			Style = (sSolid | sDot | sDash);
			Color = 0x000000;
		}
	}
	
	Band =					//Раздел 2
	{
	.
	.
	.
	}
}