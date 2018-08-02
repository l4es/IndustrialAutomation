package com.oapc;

import com.oapc.jcommon.Int;
import com.oapc.liboapc.*;

public class hmiObjectMessage
{
   static final int DIGI_MESSAGE=1;
   static final int NUM_MESSAGE =2;
   static final int CHAR_MESSAGE=3;
   static final int BIN_MESSAGE =4;
   
   Int            flowThreadID;
   flowObject     srcObject;
   FlowConnection connection;
   byte           digiValue;
   double         numValue;
   String         charValue;
   oapc_bin_head  binValue;
   flowObject     object;
   
   hmiObjectMessage(flowObject srcObject,Int flowThreadID,FlowConnection connection,flowObject object)
   {
      this.srcObject=srcObject;
      this.flowThreadID=flowThreadID;
      this.connection=connection;
      this.object=object;
   }
   
}
