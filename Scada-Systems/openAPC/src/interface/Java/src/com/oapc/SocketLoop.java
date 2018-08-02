package com.oapc;

import java.util.*;
import java.io.*;


public class SocketLoop extends Thread 
{
   private Vector        socketList;
   private boolean       isRunning=true;
   private OAPCInterface oapcInterface;
	
   SocketLoop(Vector socketList,OAPCInterface oapcInterface)
   {
      this.socketList=socketList;
      this.oapcInterface=oapcInterface;
	  start();
   }
	
   
   
   void close()
   {
      isRunning=false;
   }
	
   
   
   public void run()
   {
	  Enumeration vecEnum;
	  SocketEntry socketEntry;
	  String      line;
	  boolean     dataReceived;
	  
      while (isRunning)
	  {
         if (socketList.size()>0) synchronized(socketList)
         {
        	dataReceived=false;
            vecEnum=socketList.elements();
            while (vecEnum.hasMoreElements())
            {
        	   socketEntry=(SocketEntry)vecEnum.nextElement();
        	   try
        	   {
        		  if (!socketEntry.sock.isConnected())
        		  {
                     oapcInterface.stateChanged(OAPCListener.STATE_CONNECTION_CLOSED,"",socketEntry);
                     socketEntry.sock.close();
        	         break;
        		  }
        		  else if (socketEntry.in.ready())
        	      {
        			 dataReceived=true;
                     line=socketEntry.in.readLine();
                     oapcInterface.stateChanged(OAPCListener.STATE_DATA_RECEIVED,line,socketEntry);
                     if (line.startsWith("CMD "))
                     {
                        socketEntry.cmd=line.substring(4);
                        socketEntry.cmd_set=true;
                     }
                     else if (line.startsWith("CHAR "))
                     {
                        socketEntry.str=line.substring(5);
                        socketEntry.str_set=true;
                        socketEntry.digi_set=false;
                        socketEntry.num_set=false;
                     }
                     else if (line.startsWith("DIGI 1"))
                     {
                        socketEntry.digi=true;
                        socketEntry.digi_set=true;
                        socketEntry.num_set=false;
                        socketEntry.str_set=false;
                     }
                     else if (line.startsWith("DIGI 0"))
                     {
                        socketEntry.digi=false;
                        socketEntry.digi_set=true;
                        socketEntry.num_set=false;
                        socketEntry.str_set=false;
                     }
                     else if (line.startsWith("NUM "))
                     {
                        socketEntry.num=Float.valueOf(line.substring(4)).floatValue();
                        socketEntry.num_set=true;
                        socketEntry.digi_set=false;
                        socketEntry.str_set=false;
                     }
                     if (socketEntry.cmd_set)
                     {
                        if (socketEntry.str_set)
                        {
                           oapcInterface.commandReceived(socketEntry.cmd,socketEntry.str,socketEntry);
                           socketEntry.str_set=false;
                           socketEntry.cmd_set=false;
                        }
                        else if (socketEntry.num_set)
                        {
                            oapcInterface.commandReceived(socketEntry.cmd,socketEntry.num,socketEntry);                        	
                            socketEntry.num_set=false;
                            socketEntry.cmd_set=false;
                        }
                        else if (socketEntry.digi_set)
                        {
                            oapcInterface.commandReceived(socketEntry.cmd,socketEntry.digi,socketEntry);                        	
                            socketEntry.digi_set=false;
                            socketEntry.cmd_set=false;
                        }
                     }
        	      }
        	   }
               catch (IOException ioe)
               {
        	   }
            }
            if (!dataReceived) try
            {
           	   Thread.sleep(100);
            }
            catch (InterruptedException ie)
            {
            }
         }
         else try
         {
        	 Thread.sleep(250);
         }
         catch (InterruptedException ie)
         {
        	 
         }
		
	  }
   }

   
   
}
