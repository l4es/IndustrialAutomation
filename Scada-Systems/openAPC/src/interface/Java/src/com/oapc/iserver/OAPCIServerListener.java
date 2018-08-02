package com.oapc.iserver;

import java.util.*;

public interface OAPCIServerListener extends EventListener 
{  
   static final short CMD_OAPC_CMD_SET_VALUE   =0x0001; /** set the value(s) followed to this structure; this is used in both directions to set new values to the server and to set new values to a connected client */
   static final short OAPC_CMD_GET_VALUE       =0x0002; /** request a value, no data are following here; this is used by a client only, the server doesn't requests data */
   static final short OAPC_CMD_GET_ALL_VALUES  =0x0003; /** request all available values, no data are following here; this is used by a client only, the server doesn't requests data */
   static final short OAPC_CMDERR_DOESNT_EXISTS=0x0100; /** return code submitted in dataReceived() in case data have been requested for a node that does not exist */
	
  /**
   * This method is called whenever new data are received from the Interlock Server, it can be used to
   * get data from the server and to react on state changes
   * @param nodeName the name of the data node that was changes or that is submitted due to a request
   *        to get data
   * @param cmd additional information why this method was called, here OAPC_CMDERR_DOESNT_EXISTS is given
   *        in case data of a node have been requested that is not known to the Interlock Server
   * @param ios bitmask consisting of or-concatenated OAPC_xxx_IOy flags that describe which of the following
   *        values-indices contain which kind of data
   * @param values an array of IServerData objects, this array contains valid objects for these IOs that are
   *        identified by the flags given in parameter "ios"
   */
   public void dataReceived(String nodeName,int cmd,long ios,IServerData[] values);
}


