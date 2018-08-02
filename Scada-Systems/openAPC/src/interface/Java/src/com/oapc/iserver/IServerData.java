package com.oapc.iserver;

import com.oapc.liboapc.*;

/**
 * Storage class to keep information about a single data set, it may contain a
 * digital, numerical or string value exlcusively. 
 *
 */
public class IServerData 
{
	public long          io=0;       /** variable storing a flag which identifies the kind of data and the number of the input it belongs to */
	public boolean       digi=false; /** variable to store digital data */
	public double        num=0.0;    /** variable to store numeric data */
	public String        str="";     /** variable to store a string */
	public oapc_bin_head bin=null;
	
	public IServerData()
	{
	}

	public IServerData(boolean digi)
	{
	   this.digi=digi;	   
	}

	public IServerData(double num)
	{
	   this.num=num;	   
	}

	public IServerData(String str)
	{
	   this.str=str;
	}

	public IServerData(oapc_bin_head bin)
	{
	   this.bin=bin;
	}

}
