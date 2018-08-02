package com.oapc.iface;

import java.io.*;
import java.net.*;

public class SocketEntry
{
	Socket         sock;
	BufferedReader in;
	BufferedWriter out;
	String         cmd,str;
	boolean        digi;
	float          num;
	boolean        cmd_set=false,str_set=false,digi_set=false,num_set=false;
	
	SocketEntry(Socket sock)
	{
		this.sock=sock;
	}
}
