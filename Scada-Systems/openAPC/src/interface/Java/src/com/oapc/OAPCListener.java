package com.oapc;

import java.util.*;

public interface OAPCListener extends EventListener 
{
   public static final int STATE_NEW_CONNECTION   =1; /** a new client has connected to the interface */
   public static final int STATE_CONNECTION_CLOSED=2; /** a client has closed its connection to the interface */
   public static final int STATE_CONNECTION_ERROR =3; /** an error occurred for a client */
   public static final int STATE_DATA_RECEIVED    =4; /** data have been received at a clients socket */
   
   public void commandReceived(String cmd,String value,SocketEntry socket);
   public void commandReceived(String cmd,float value,SocketEntry socket);
   public void commandReceived(String cmd,boolean value,SocketEntry socket);	
   public void stateChanged(int state,String message,SocketEntry socket);
}


