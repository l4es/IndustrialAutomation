package com.oapc.iface;

import java.util.*;
import java.net.*;
import java.io.*;



/**
 * This class provides connectivity functionality to establish a standardised OAPC interface connection
 * via TCP/IP to submit pairs of data consisting of a command and a digital/numeric/text value each.
 * User authentication is supported to to offer full compatibility with the related plug-ins.
 * This class creates a server socket that can be connected from OAPC-client plug-ins.
 */
public class OAPCInterface extends ServerSocket implements Runnable
{
   private Thread              sockThread=new Thread(this);
   private boolean             isRunning=true;
   private String              uname=null,pwd=null;
   private Vector              socketList=new Vector(),listenerList=new Vector();
   private SocketLoop          socketLoop;

   
   
  /**
   * Default constructor to create a server socket that accepts all incoming
   * connections using the default port 
   * @throws IOException in case no server socket could be created and bound
   */
   public OAPCInterface() 
   throws IOException
   {
      super(1810);
      sockThread.start();
   }

   

  /**
   * Constructor to create a new interface server socket using the default port 
   * @param bindAddr the local address to bind the server to 
   * @throws IOException in case no server socket could be created and bound
   */
   public OAPCInterface(InetAddress bindAddr)
   throws IOException
   {
      super(1810,3,bindAddr);
      sockThread.start();
   }
   
   
   
  /**
   * Constructor to create a new communication server socket using a custom address
   * and port number.  
   * @param port the port number to bind the server to
   * @param bindAddr the local address to listen at for incoming connections
   * @throws IOException in case no server socket could be created and bound
   */
   public OAPCInterface(int port, InetAddress bindAddr)
   throws IOException
   {
      super(port,3,bindAddr);
      sockThread.start();
   }
   
   
   
  /**
   * Set authentication data to restrict access to this interface server socket. Incoming
   * connections have to authenticate using these data, elsewhere they are rejected
   * @param uname the user name part of the authentication data
   * @param pwd the password part of the authentication data
   */
   public void setAuthentication(String uname,String pwd)
   {
      this.uname=uname;
      this.pwd=pwd;
   }
   
   
   
  /**
   * Closes all incoming connections, shuts down the interface and releases
   * all resources so that no more communication is possible
   */
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

   
   
  /**
   * Adds a listener object that is used to notify about received data and to inform
   * about the current connection state
   * @param listener the listener that has to be added for future notification about all
   *        communication events
   */
   public void addOAPCListener(OAPCListener listener)
   {
      listenerList.add(listener);
   }

   

  /**
   * Removes a listener from the list of registered listener-objects
   * @param listener the listener to be removed
   */
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
   
   

  /**
   * Send a data set consisting of command and string value to a connected client
   * @param cmd the command that is assigned to the value
   * @param value the string value 
   * @param socket the socket connection to be used for data transmission, if set
   *        to null the data are sent to all connected clients
   */
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
   
   

   /**
    * Send a data set consisting of command and number value to a connected client
    * @param cmd the command that is assigned to the value
    * @param value the number value 
    * @param socket the socket connection to be used for data transmission, if set
    *        to null the data are sent to all connected clients
    */
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
   
   

   /**
    * Send a data set consisting of command and digital value to a connected client
    * @param cmd the command that is assigned to the value
    * @param value the digital value 
    * @param socket the socket connection to be used for data transmission, if set
    *        to null the data are sent to all connected clients
    */
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
