package com.oapc;

import com.oapc.liboapc.*;
import com.oapc.jcommon.*;



public class FlowWalkerThread 
{
   static final int THREAD_MODE_DIGI=1;
   static final int THREAD_MODE_NUM =2;
   static final int THREAD_MODE_CHAR=3;
   static final int THREAD_MODE_BIN =4;
		
           long           creationTime;
           flowObject     object;
           FlowConnection connection;
   private Int            flowThreadID=new Int(0);
   private int            mode;
   private byte           digiValue;
   private double         numValue;
   private String         charValue;
   private oapc_bin_head  binValue=null;
   private long           lastInput;
   private flowObject     m_sourceObject;
   private boolean        firstCall;
   
   
	
   FlowWalkerThread(flowObject object,FlowConnection connection,int mode,long creationTime,long lastInput)
   {
	   flowThreadID.val=MainWin.g_flowPool.getFlowThreadID();
	   this.object=object;
	    
	   this.lastInput=lastInput;
      if (connection!=null) this.connection=connection;
      else
	   {
	      connection=object.getFlowConnection(null);
      }
	   this.creationTime=creationTime;
	   this.mode=mode;
	   firstCall=true;
   }
   
   
   
   void close()
   {
	   
   }

   
   void setMode(int mode)
   {
      this.mode=mode;
   }

   
   /**
   This method has to be called directly after thread creation. It has to be used, when an output already
   has been used and the returned value is available: in this case the value is set here and the thread
   starts with walking down the flow connection and setting the next input value instead of checking the
   output again
   @param[in] val the digital value that has to be used for the next objects input
   */
   void setFirstOutValue(boolean val,flowObject object)
   {
	  if (val) digiValue=0;
	  else digiValue=1;
      m_sourceObject=object;
   }   


   /**
   This method has to be called directly after thread creation. It has to be used, when an output already
   has been used and the returned value is available: in this case the value is set here and the thread
   starts with walking down the flow connection and setting the next input value instead of checking the
   output again
   @param[in] val the digital value that has to be used for the next objects input
   */
   void setFirstOutValue(byte val,flowObject object)
   {
      digiValue=val;
      m_sourceObject=object;
   }



   /**
   This method has to be called directly after thread creation. It has to be used, when an output already
   has been used and the returned value is available: in this case the value is set here and the thread
   starts with walking down the flow connection and setting the next input value instead of checking the
   output again
   @param[in] val the numerical value that has to be used for the next objects input
   */
   void setFirstOutValue(double val,flowObject object)
   {
      numValue=val;
      m_sourceObject=object;
   }



   /**
   This method has to be called directly after thread creation. It has to be used, when an output already
   has been used and the returned value is available: in this case the value is set here and the thread
   starts with walking down the flow connection and setting the next input value instead of checking the
   output again
   @param[in] val the character value that has to be used for the next objects input
   */
   void setFirstOutValue(String val,flowObject object)
   {
      charValue=val;
      m_sourceObject=object;
   }



   /**
   This method has to be called directly after thread creation. It has to be used, when an output already
   has been used and the returned value is available: in this case the value is set here and the thread
   starts with walking down the flow connection and setting the next input value instead of checking the
   output again
   @param[in] val the binary value that has to be used for the next objects input
   */
   void setFirstOutValue(oapc_bin_head val,flowObject object)
   {
      binValue=val;
// TODO:      if (firstCall) ctx.g_dataCtrl.setBinDataUsed(binValue);
      firstCall=false;
      m_sourceObject=object;
   }



   void setNextConnection(FlowConnection connection)
   {
      this.connection=connection;
   }



   void setFlowThreadID(Int flowThreadID)
   {
       this.flowThreadID.val=flowThreadID.val;
   }



   int exitThread()
   {
//TODO:      if (binValue) ctx.g_dataCtrl.setBinDataUnused(binValue,true);
      return 0;
   }



   int execute()
   {
      if ((connection==null) || (MainWin.g_breakThreads)) return exitThread();

      if (connection.data.targetID==0) return exitThread();
      object=MainWin.g_objectList.getObject(connection.data.targetID);
   //   wxASSERT(object);
      if (object==null) return exitThread();
      if (mode==THREAD_MODE_DIGI)
      {
         if (!connection.isTempConnection) MainWin.g_flowPool.sendDigiValueToIS(connection.data.targetInput,digiValue,object,false);
//         if (!object.setDigitalInput(connection,digiValue,flowThreadID,m_sourceObject)) return exitThread();
         object.sendDigitalInput(connection,digiValue,flowThreadID,object);
         if (MainWin.g_breakThreads) return exitThread();
      }
      else if (mode==THREAD_MODE_NUM)
      {
         if (!connection.isTempConnection) MainWin.g_flowPool.sendNumValueToIS(connection.data.targetInput,numValue,object,false);
         if (!object.setNumInput(connection,numValue,flowThreadID,m_sourceObject)) return exitThread();
      }
      else if (mode==THREAD_MODE_CHAR)
      {
         if (!connection.isTempConnection) MainWin.g_flowPool.sendCharValueToIS(connection.data.targetInput,charValue,object,false);
         if (!object.setCharInput(connection,charValue,flowThreadID,m_sourceObject)) return exitThread();
      }
      else if (mode==THREAD_MODE_BIN)
      {
//         g_dataCtrl.setBinDataUsed(binValue);
         if (!connection.isTempConnection) MainWin.g_flowPool.sendBinValueToIS(connection.data.targetInput,binValue,object,false);
         if (!object.setBinInput(connection,binValue,flowThreadID,m_sourceObject)) return exitThread();
//         binValue=g_dataCtrl.setBinDataUnused(binValue,false);
      }
      lastInput=connection.data.targetInput;
      MainWin.g_flowPool.startFlows(object,this,connection.data.targetInput,new Int(0),creationTime,lastInput); // after this call this threads owns the new connection that has to be processed next

      if (creationTime+MainWin.g_objectList.m_projectData.flowTimeout<System.currentTimeMillis())
      {
         return exitThread();
      }
      return 1;
   }

   
   
}
