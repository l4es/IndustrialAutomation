package com.oapc;

import java.util.*;
import java.net.*;
import java.io.*;



public class OAPCInterface extends ServerSocket implements Runnable
{
   private Thread              sockThread=new Thread(this);
   private boolean             isRunning=true;
   private String              uname=null,pwd=null;
   private Vector              socketList=new Vector(),listenerList=new Vector();
   private SocketLoop          socketLoop;

   
   
   public OAPCInterface() 
   throws IOException
   {
      super(1810);
      sockThread.start();
   }

   
   
   public OAPCInterface(InetAddress bindAddr)
   throws IOException
   {
      super(1810,3,bindAddr);
      sockThread.start();
   }
   
   
   
   public OAPCInterface(int port, InetAddress bindAddr)
   throws IOException
   {
      super(port,3,bindAddr);
      sockThread.start();
   }
   
   
   
   public void setAuthentication(String uname,String pwd)
   {
      this.uname=uname;
      this.pwd=pwd;
   }
   
   
   
   public void close()
   {
	  isRunning=false;
	  if (socketLoop!=null) socketLoop.close();
	  try
	  {
         super.close();
	  }
	  catch (IOException ioe)
	  {
	  }
   }

   
   
   public void addOAPCListener(OAPCListener listener)
   {
      listenerList.add(listener);
   }

   

   public void removeOAPCListener(OAPCListener listener)
   {
      listenerList.remove(listener);
   }



   void commandReceived(String cmd,String value,SocketEntry socket)
   {
   	  Enumeration  vecEnum;
   	  OAPCListener listener;
          
   	  vecEnum=listenerList.elements();
      while (vecEnum.hasMoreElements())
      {
   	     listener=(OAPCListener)vecEnum.nextElement();
   	     listener.commandReceived(cmd,value,socket);
      }
   }
   
   

   void commandReceived(String cmd,float value,SocketEntry socket)
   {
   	  Enumeration  vecEnum;
   	  OAPCListener listener;
          
   	  vecEnum=listenerList.elements();
      while (vecEnum.hasMoreElements())
      {
   	     listener=(OAPCListener)vecEnum.nextElement();
   	     listener.commandReceived(cmd,value,socket);
      }
   }
   
   

   void commandReceived(String cmd,boolean value,SocketEntry socket)
   {
   	  Enumeration  vecEnum;
   	  OAPCListener listener;
	          
   	  vecEnum=listenerList.elements();
      while (vecEnum.hasMoreElements())
      {
   	     listener=(OAPCListener)vecEnum.nextElement();
   	     listener.commandReceived(cmd,value,socket);
      }
   }
   
   

   void stateChanged(int state,String message,SocketEntry socket)
   {
   	  Enumeration  vecEnum;
   	  OAPCListener listener;
	          
   	  vecEnum=listenerList.elements();
      while (vecEnum.hasMoreElements())
      {
   	     listener=(OAPCListener)vecEnum.nextElement();
   	     listener.stateChanged(state,message,socket);
      }
   }
   
   

   public void sendCommand(String cmd,String value,SocketEntry socket)
   {
	  String line;

	  if (socket==null)
	  {
	   	  Enumeration  vecEnum;
		          
	   	  vecEnum=socketList.elements();
	      while (vecEnum.hasMoreElements())
	      {
	   	     socket=(SocketEntry)vecEnum.nextElement();
	   	     sendCommand(cmd,value,socket);
	      }		  
		  return;
	  }
      try
      {
         line="CMD "+cmd+"\n";
         socket.out.write(line,0,line.length());
         line="CHAR "+value+"\n";
         socket.out.write(line,0,line.length());
         socket.out.flush();
      }
      catch (IOException ioe)
      {
         stateChanged(OAPCListener.STATE_CONNECTION_CLOSED,"",socket);
    	 synchronized(socketList)
    	 {
            socketList.remove(socket);
    	 }
      }
   }
   
   

   public void sendCommand(String cmd,float value,SocketEntry socket)
   {
	  String line;
		  
	  if (socket==null)
	  {
	   	  Enumeration  vecEnum;
		          
	   	  vecEnum=socketList.elements();
	      while (vecEnum.hasMoreElements())
	      {
	   	     socket=(SocketEntry)vecEnum.nextElement();
	   	     sendCommand(cmd,value,socket);
	      }		  
		  return;
	  }
      try
      {
         line="CMD "+cmd+"\n";
         socket.out.write(line,0,line.length());
         line="NUM "+value+"\n";
         socket.out.write(line,0,line.length());
         socket.out.flush();
      }
      catch (IOException ioe)
      {
         stateChanged(OAPCListener.STATE_CONNECTION_CLOSED,"",socket);
    	 synchronized(socketList)
    	 {
            socketList.remove(socket);
    	 }
      }
   }
   
   

   public void sendCommand(String cmd,boolean value,SocketEntry socket)
   {
	  String line;
		  
	  if (socket==null)
	  {
	   	  Enumeration  vecEnum;
		          
	   	  vecEnum=socketList.elements();
	      while (vecEnum.hasMoreElements())
	      {
	   	     socket=(SocketEntry)vecEnum.nextElement();
	   	     sendCommand(cmd,value,socket);
	      }		  
		  return;
	  }
      try
      {
         line="CMD "+cmd+"\n";
         socket.out.write(line,0,line.length());
    	 if (value) line="DIGI 1\n";
    	 else line="DIGI 0\n";
         socket.out.write(line,0,line.length());
         socket.out.flush();
      }
      catch (IOException ioe)
      {
         stateChanged(OAPCListener.STATE_CONNECTION_CLOSED,"",socket);
    	 synchronized(socketList)
    	 {
            socketList.remove(socket);
    	 }
      }
   }
   
   

   public void run()
   {
      Socket      s;
      SocketEntry entry;
      String      line,uname,pwd;
      
      socketLoop=new SocketLoop(socketList,this);
	  while (isRunning)
	  {
		 s=null;
	     try
	     {
	        s=accept();
	        
        	entry=new SocketEntry(s);
        	entry.in=new BufferedReader(new InputStreamReader(s.getInputStream()));
        	entry.out=new BufferedWriter(new OutputStreamWriter(s.getOutputStream(),"UTF-8"));//"ISO-8859-15"));
	        if ((this.uname!=null) && (this.pwd!=null))
	        {
	           line=entry.in.readLine();
	           if (!line.startsWith("UNAME ")) throw new IOException("Wrong format on incoming data (UNAME not found)");
	           uname=line.substring(6);
	           line=entry.in.readLine();
	           if (!line.startsWith("PWD ")) throw new IOException("Wrong format on incoming data (PWD not found)");
	           pwd=line.substring(4);
	           if ((this.uname.compareTo(uname)!=0) || (this.pwd.compareTo(pwd)!=0))
	           {
	        	  line="FAIL authentication error\n";
	        	  entry.out.write(line,0,line.length());
	        	  entry.out.flush();
	              throw new IOException("Authentication failed");
	           }
	           if (!line.startsWith("MODE ")) throw new IOException("Wrong format on incoming data (MODE not found)");
	           line=line.substring(5);
	           if (Integer.valueOf(line).intValue()!=1)
	           {
                  line="FAIL wrong mode\n";
		          entry.out.write(line,0,line.length());
		       	  entry.out.flush();
		          throw new IOException("Wrong connection mode");
	           }	        	   
	        }
	        socketList.add(entry);
	        stateChanged(OAPCListener.STATE_NEW_CONNECTION,s.getInetAddress().getHostAddress(),entry);
	     }
	     catch (IOException ioe)
	     {
	    	ioe.printStackTrace();
	    	try
	    	{
               stateChanged(OAPCListener.STATE_CONNECTION_ERROR,ioe.toString(),null);
	    	   if (s!=null)
	    	   {
                  stateChanged(OAPCListener.STATE_CONNECTION_CLOSED,"",null);
	    	      s.close();
	    	   }
	    	}
	    	catch (IOException ioe2)
	    	{
	    	}
	     }
      }
   }
   
}
