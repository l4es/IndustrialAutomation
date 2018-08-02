package com.oapc.iface;

import java.util.*;

public interface OAPCListener extends EventListener 
{
   public static final int STATE_NEW_CONNECTION   =1; /** a new client has connected to the interface */
   public static final int STATE_CONNECTION_CLOSED=2; /** a client has closed its connection to the interface */
   public static final int STATE_CONNECTION_ERROR =3; /** an error occurred for a client */
   public static final int STATE_DATA_RECEIVED    =4; /** data have been received at a clients socket */
   
   
   
  /**
   * Notification method for received data of type string
   * @param cmd the command that is assigned to the data
   * @param value the string value that belongs to the command
   * @socket an identifier of the connection that submitted the new data to the server,
   *         it can be used e.g. to send a response directly
   */
   public void commandReceived(String cmd,String value,SocketEntry socket);

   
   
  /**
   * Notification method for received data of type number
   * @param cmd the command that is assigned to the data
   * @param value the numerical value that belongs to the command
   * @socket an identifier of the connection that submitted the new data to the server,
   *         it can be used e.g. to send a response directly
   */
   public void commandReceived(String cmd,float value,SocketEntry socket);

   
   
  /**
   * Notification method for received data of type digital
   * @param cmd the command that is assigned to the data
   * @param value the digital value that belongs to the command
   * @socket an identifier of the connection that submitted the new data to the server,
   *         it can be used e.g. to send a response directly
   */
   public void commandReceived(String cmd,boolean value,SocketEntry socket);	

   
   
  /**
   * Notification method for changes in connection state
   * @param state information flag of type STATE_xxx that informs about the state change
   * @param message informational, human-readable message that is related to the state change
   * @param socket the client connection this state information belongs to; this object can be
   *        for direct responses to this client only in case no connection error state was
   *        signalled
   */
   public void stateChanged(int state,String message,SocketEntry socket);
}


