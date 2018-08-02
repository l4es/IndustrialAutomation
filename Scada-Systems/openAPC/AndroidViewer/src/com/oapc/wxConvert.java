package com.oapc;

public class wxConvert 
{
	static int fromWxColour(int wxColour)
	{
      return (0xFF000000 | (wxColour & 0xFF0000)>>16) | (wxColour & 0x00FF00) | ((wxColour & 0x0000FF)<<16);
	}

}
