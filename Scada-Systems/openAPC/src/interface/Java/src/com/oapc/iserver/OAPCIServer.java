package com.oapc.iserver;

import java.util.*;
import java.net.*;
import java.io.*;

import com.oapc.*;
import com.oapc.liboapc.*;



/**
 * This class offers functionality to contact an Interlock Server, to keep track of changes it announces
 * to this connection, to send new data to the server and to request data from there 
 */
public class OAPCIServer extends Socket implements Runnable
{
   public static final int  MAX_NODENAME_LENGTH=127;
   public static final int  MAX_TEXT_LENGTH=250;
   public static final int  MAX_NUM_IOS=8;
   
   public static final int  OAPC_OK=                     0x00000001; /** everything is OK, an operation was performed successfully */
   public static final int  OAPC_ERROR_CONNECTION=       0x00000002; /** a required connection could not be established */
   public static final int  OAPC_ERROR_NO_DATA_AVAILABLE=0x00000005; /** this value is returned when data are requested for an output where no new data are available at this moment */
   public static final int  OAPC_ERROR_RESOURCE=         0x00000007; /** a required resource could not be created/accessed */

   /** IO flags that define which inputs/outputs of which type are available */
   /** Digital IOs */
   public static final int OAPC_DIGI_IO_MASK=0x000000FF;
   public static final int OAPC_DIGI_IO0    =0x00000001;
   public static final int OAPC_DIGI_IO1    =0x00000002;
   public static final int OAPC_DIGI_IO2    =0x00000004;
   public static final int OAPC_DIGI_IO3    =0x00000008;
   public static final int OAPC_DIGI_IO4    =0x00000010;
   public static final int OAPC_DIGI_IO5    =0x00000020;
   public static final int OAPC_DIGI_IO6    =0x00000040;
   public static final int OAPC_DIGI_IO7    =0x00000080;

   /** Numerical IOs */
   public static final int OAPC_NUM_IO_MASK =0x0000FF00;
   public static final int OAPC_NUM_IO0     =0x00000100;
   public static final int OAPC_NUM_IO1     =0x00000200;
   public static final int OAPC_NUM_IO2     =0x00000400;
   public static final int OAPC_NUM_IO3     =0x00000800;
   public static final int OAPC_NUM_IO4     =0x00001000;
   public static final int OAPC_NUM_IO5     =0x00002000;
   public static final int OAPC_NUM_IO6     =0x00004000;
   public static final int OAPC_NUM_IO7     =0x00008000;

   /** Character IOs */
   public static final int OAPC_CHAR_IO_MASK=0x00FF0000;
   public static final int OAPC_CHAR_IO0    =0x00010000;
   public static final int OAPC_CHAR_IO1    =0x00020000;
   public static final int OAPC_CHAR_IO2    =0x00040000;
   public static final int OAPC_CHAR_IO3    =0x00080000;
   public static final int OAPC_CHAR_IO4    =0x00100000;
   public static final int OAPC_CHAR_IO5    =0x00200000;
   public static final int OAPC_CHAR_IO6    =0x00400000;
   public static final int OAPC_CHAR_IO7    =0x00800000;

   /** Binary IOs */
   public static final int OAPC_BIN_IO_MASK =0xFF000000;
   public static final int OAPC_BIN_IO0     =0x01000000;
   public static final int OAPC_BIN_IO1     =0x02000000;
   public static final int OAPC_BIN_IO2     =0x04000000;
   public static final int OAPC_BIN_IO3     =0x08000000;
   public static final int OAPC_BIN_IO4     =0x10000000;
   public static final int OAPC_BIN_IO5     =0x20000000;
   public static final int OAPC_BIN_IO6     =0x40000000;
   public static final int OAPC_BIN_IO7     =0x80000000;  
   
   private Thread                      sockThread=new Thread(this);
   private boolean                     isRunning=false;
   private Vector<OAPCIServerListener> listenerList=new Vector<OAPCIServerListener>();
   DataOutputStream                    dOut;


   
   /**
    * Default constructor, a connection is established to a local Interlock Server using
    * the default port number
    * @throws IOException in case the connection to the server could not be established
    */
   public OAPCIServer() 
   throws IOException
   {
      super("127.0.0.1",18100);
      dOut=new DataOutputStream(getOutputStream());
      sockThread.start();
      waitForInit();
   }

   
   
   /**
    * Constructor to create a connection using the Interlocks Server default port
    * @param connectAddr IP of the server to connect with
    * @throws IOException in case the connection to the server could not be established
    */
   public OAPCIServer(InetAddress connectAddr)
   throws IOException
   {
      super(connectAddr,18100);
      dOut=new DataOutputStream(getOutputStream());
      sockThread.start();
      waitForInit();
   }
   
   
   
   /**
    * Constructor to create a connection using the Interlocks Server default port
    * @param connectAddr IP of the server to connect with
    * @throws IOException in case the connection to the server could not be established
    */
   public OAPCIServer(String connectAddr)
   throws IOException
   {
      super(connectAddr,18100);
      dOut=new DataOutputStream(getOutputStream());
      sockThread.start();
      waitForInit();
   }
   
   
   
   /**
    * Constructor to create a new connection to an Interlock Server
    * @param port the port number the desired server is listening at
    * @param connectAddr IP of the server to connect with
    * @throws IOException in case the connection to the server could not be established
    */
   public OAPCIServer(int port, InetAddress connectAddr)
   throws IOException
   {
      super(connectAddr,port);
      dOut=new DataOutputStream(getOutputStream());
      sockThread.start();
      waitForInit();
   }

   
   
   private void waitForInit()
   throws IOException
   {
      int cnt=0;
      
      while (cnt<20)
      {
         if (connectionValid()) return;
         try
         {
            Thread.sleep(50);
         }
         catch (InterruptedException ie)
         {
        	 
         }
      }
   }
   
   
   
   /**
    * Closes the connection to the server and releases all resources
    */
   public void close()
   {
	  isRunning=false;
	  try
	  {
         super.close();
	  }
	  catch (IOException ioe)
	  {
	  }
   }

   
   
  /**
   * Adds a listener to keep track of changes within the server and to receive requested
   * data
   * @param listener own implementation of OAPCIServerListener to receive data
   */
   public void addOAPCIServerListener(OAPCIServerListener listener)
   {
      listenerList.add(listener);
   }

   

  /**
   * Removes a listener out of the list of currently registered listeners
   * @param listener the listener to be removed
   */
   public void removeOAPCIServerListener(OAPCIServerListener listener)
   {
      listenerList.remove(listener);
   }

   
   
  /**
   * This is an elementary method, it tells a calling instance if the connection to the
   * server is still valid or not.
   * @return true in case the connection is valid and can be used, false otherwise; in last
   *         case this object has to be dropped and a new object of type OAPCIServer has to
   *         be created in order to re-establish the connection to the Interlock Server
   */
   public boolean connectionValid()
   {
	   return isRunning;
   }

   
   
   private int sendRequest(String nodeName,short cmd)
   {
      byte[] b;
      
      if (!connectionValid()) return OAPC_ERROR_RESOURCE;	   
      b=new byte[MAX_NODENAME_LENGTH+4];     
      try
      {
         dOut.writeInt(1);
         dOut.writeInt(148); // total size of head in bytes
	     dOut.writeInt(0);
	     dOut.writeShort(cmd);
	     dOut.writeShort(0);
	     if (nodeName.length()>MAX_NODENAME_LENGTH) nodeName=nodeName.substring(0,MAX_NODENAME_LENGTH);
	     System.arraycopy(nodeName.getBytes(),0,b,0,nodeName.length());
	     dOut.write(b);
	     dOut.writeByte(0); // padding byte
	     dOut.flush();
      }
      catch (IOException ioe)
      {
    	 System.err.println(ioe.toString());
    	 isRunning=false;
         return OAPC_ERROR_CONNECTION;
      }
      return OAPC_OK;
   }

   
   
  /**
   * Requests the data of a specific node from the server; the response is not given directly
   * but is sent asynchronously to the connected listeners
   * @param nodeName the name of the data node to fetch the data for
   * @return OAPC_OK in case the request could be submitted successfully or an error code otherwise;
   *         in case the operation failed the connection to the server was closed and this object
   *         can't be used any longer for communication
   */
   public int requestData(String nodeName)
   {
      return sendRequest(nodeName,OAPCIServerListener.OAPC_CMD_GET_VALUE);
   }
   
   
   
  /**
   * Requests the data of all nodes from the server; the response is not given directly but is sent
   * asynchronously to the connected listeners; PLEASE NOTE: dependent on the amount of data stored
   * within the server this call may cause a heavy load due to the number of transmitted data
   * @return OAPC_OK in case the request could be submitted successfully or an error code otherwise;
   *         in case the operation failed the connection to the server was closed and this object
   *         can't be used any longer for communication
   */
   public int requestAllData()
   {
      return sendRequest("",OAPCIServerListener.OAPC_CMD_GET_ALL_VALUES);
   }
   
   

  /**
   * Using this method a bunch of data can be sent to the Interlock Server and set for a specific
   * node.
   * @param nodeName the data node where the data have to be set at
   * @param ios a bit mask that consists of or-concatenated OAPC_xxx_IOy constants and specifies
   *        which IOs contain which kind of data; here the IO position (y) corresponds to the index
   *        position of the "values"-array 
   * @param values an array of objects that contains the data to be sent according to the given ios
   * @return OAPC_OK in case the request could be submitted successfully or an error code otherwise;
   *         in case the operation failed the connection to the server was closed and this object
   *         can't be used any longer for communication
   */
   public int setData(String nodeName,long ios,IServerData[] values)
   {
      byte[] b;
      long   bitmask=0x00010101;
      int    i;
	      
      if (!connectionValid()) return OAPC_ERROR_RESOURCE;	   
      b=new byte[MAX_NODENAME_LENGTH+4];     
      try
      {
         dOut.writeInt(1);
         dOut.writeInt(148); // total size of head in bytes
	     dOut.writeInt((int)ios);
	     dOut.writeShort(OAPCIServerListener.CMD_OAPC_CMD_SET_VALUE);
	     dOut.writeShort(0);
	     if (nodeName.length()>MAX_NODENAME_LENGTH) nodeName=nodeName.substring(0,MAX_NODENAME_LENGTH);
	     System.arraycopy(nodeName.getBytes(),0,b,0,nodeName.length());
	     dOut.write(b);
	     dOut.writeByte(0); // padding byte

	     for (i=0; i<MAX_NUM_IOS; i++)
	     {
	     	if ((ios & bitmask)!=0)
	       	{
	       	   if ((values[i]==null) || (values[i].io==0) || ((values[i].io & ios & bitmask)==0)) // client error, a IO flag was set for an IO where no data are provided
                return OAPC_ERROR_NO_DATA_AVAILABLE;
               if ((ios & bitmask & OAPC_DIGI_IO_MASK)!=0)
               {
                  if (values[i].digi) dOut.writeByte(1);
                  else dOut.writeByte(0);
               }
               else if ((ios & bitmask & OAPC_NUM_IO_MASK)!=0)
               {
                  int[]   numValue;
                  short[] numDivider;
                  
                  numValue=new int[1];
                  numDivider=new short[1];
                  
                  Util.dblToNumBlock(values[i].num,numValue,numDivider);
                  dOut.writeInt(numValue[0]);
                  dOut.writeShort(numDivider[0]);
                  dOut.writeShort(0); // reserved
               }			
               else if ((ios & bitmask & OAPC_CHAR_IO_MASK)!=0)
               {
           	      byte[] bs;
            	      
           	      bs=new byte[MAX_TEXT_LENGTH+4];     
                  if (values[i].str.length()>MAX_TEXT_LENGTH) values[i].str=values[i].str.substring(0,MAX_TEXT_LENGTH);
                  System.arraycopy(values[i].str.getBytes(),0,bs,0,values[i].str.length());
            	  dOut.write(bs);
               }
               else if ((ios & bitmask & OAPC_BIN_IO_MASK)!=0)
               {
                  values[i].bin.write(dOut);
               }
	       	   bitmask=bitmask<<1;
	       	}	
	     }
	     dOut.flush();
      }
      catch (IOException ioe)
      {
     	 System.err.println(ioe.toString());
    	 isRunning=false;
         return OAPC_ERROR_CONNECTION;
      }
      return OAPC_OK;
   }
   
   
   /**
    * Using this method a single set of data can be sent to the Interlock Server and set for one input
    * or putput of a specific node.
    * @param nodeName the data node where the data have to be set at
    * @param ios a bit mask that consists of one OAPC_xxx_IOy constant and specifies which IO contains
    *        which kind of data; here the IO position (y) corresponds to the input/output number of the
    *        data node 
    * @param value an objects of type IServerData that contains the data to be sent according to the given
    *        io
    * @return OAPC_OK in case the request could be submitted successfully or an error code otherwise;
    *         in case the operation failed the connection to the server was closed and this object
    *         can't be used any longer for communication
    */
   public int setValue(String nodeName,long io,IServerData value)
   {
      IServerData[] values;
      int           i;
      long          bitmask=0x00010101;
	   
      if (!connectionValid()) return OAPC_ERROR_RESOURCE;	   
      values=new IServerData[MAX_NUM_IOS];
      for (i=0; i<MAX_NUM_IOS; i++)
      {
    	  if ((bitmask & io)!=0)
    	  {
    		values[i]=value;
    		values[i].io=bitmask & io;
    	  }
    	  else values[i]=null;
    	  bitmask=bitmask<<1;
      }
      return setData(nodeName,io,values);
   }
   
  

   public void run()
   {
      DataInputStream                  dIn;
      int                              headVersion,headLength;
      long                             headIos;
      short                            headCmd;
      byte[]                           headNodeName;
      String                           nodeName;
      long                             bitmask;
      int                              i;
      Enumeration<OAPCIServerListener> vecEnum;
      OAPCIServerListener              listener;
      
      headNodeName=new byte[MAX_NODENAME_LENGTH+4];
      try
      {
         dIn=new DataInputStream(this.getInputStream());
         isRunning=true;
         while (isRunning)
         {
        	IServerData[] values;
        	int           cnt;
        	 
        	values=new IServerData[MAX_NUM_IOS];
        	headVersion=dIn.readInt(); headLength=dIn.readInt();
        	headIos=dIn.readInt();
            headCmd=dIn.readShort();
            dIn.readShort(); // reserved
            dIn.read(headNodeName,0,MAX_NODENAME_LENGTH+4);
            nodeName=new String(headNodeName);
            for (cnt=0; cnt<MAX_TEXT_LENGTH; cnt++)
             if (headNodeName[cnt]==0) break;
       	    nodeName=nodeName.substring(0,cnt);
            dIn.readByte(); // pad byte
            
            bitmask=0x00010101; // binary not supported at the moment
            for (i=0; i<MAX_NUM_IOS; i++)
            {
          	   if ((headIos & bitmask)!=0)
               {
          		  values[i]=new IServerData();
                  if ((headIos & bitmask & OAPC_DIGI_IO_MASK)!=0)
                  {            	
            	     if (dIn.readUnsignedByte()==1) values[i].digi=true;
            	     else values[i].digi=false;
            	     values[i].io=headIos & bitmask & OAPC_DIGI_IO_MASK;            	     
            	  }
                  else if ((headIos & bitmask & OAPC_NUM_IO_MASK)!=0)
                  {            	
                	 int numValue;     // the numerical value
                	 short numDivider; // the power of ten to divide by
                	   
                	 numValue=dIn.readInt();
                	 numDivider=dIn.readShort();
                	 dIn.readShort(); // reserved
                	 values[i].num=Util.numBlockToDbl(numValue,numDivider);
            	     values[i].io=headIos & bitmask & OAPC_NUM_IO_MASK;            	     
            	  }
                  else if ((headIos & bitmask & OAPC_CHAR_IO_MASK)!=0)
                  {
                	 byte[] b;
                	 
                	 b=new byte[MAX_TEXT_LENGTH+4];
                	 
                	 dIn.read(b,0,MAX_TEXT_LENGTH+4);
                	 values[i].str=new String(b);
                	 for (cnt=0; cnt<MAX_TEXT_LENGTH; cnt++)
                      if (b[cnt]==0) break;
                	 values[i].str=values[i].str.substring(0,i);
            	     values[i].io=headIos & bitmask & OAPC_CHAR_IO_MASK;            	     
            	  }                  
                  else if ((headIos & bitmask & OAPC_BIN_IO_MASK)!=0)
                  {
                     values[i].bin=new oapc_bin_head();
                     values[i].bin.read(dIn);                	  
                  }
               }
          	   else values[i]=null;
               bitmask=bitmask<<1;
            }
            vecEnum=listenerList.elements();
            while (vecEnum.hasMoreElements())
            {
               listener=(OAPCIServerListener)vecEnum.nextElement();
         	   listener.dataReceived(nodeName,headCmd,headIos,values);
            }
         }
      }
      catch (IOException ioe)
      {
     	 System.err.println(ioe.toString());
    	 isRunning=false;
         return;
      }
      
   }
   
}



