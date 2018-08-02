package com.oapc;

import java.util.concurrent.locks.*;
import java.io.*;

import android.os.*;

import com.oapc.iserver.*;
import com.oapc.liboapc.*;
import com.oapc.jcommon.*;



public class FlowWalkerPool extends Handler
{
	private int            currFlowThreadID;
	private ReentrantLock  idSection,inputPollLock,flowSection;
	private boolean        outputPollTimer;
   private OAPCIServer    oapcIServer;
   private MyOAPCListener oapcListener;

	   
	   
	FlowWalkerPool()
	{
	   currFlowThreadID=0;
	   outputPollTimer=false;
	   inputPollLock=new ReentrantLock();
	   flowSection=new ReentrantLock();
	   idSection=new ReentrantLock();
	}
	

	
	int getFlowThreadID()
	{
	   int tmpID;
	   
	   idSection.lock();
	   currFlowThreadID++;
	   tmpID=currFlowThreadID;
	   idSection.unlock();
	   return tmpID;
	}

	

	void exitApplication()
	{
	   MainWin.g_breakThreads=true;
	   MainWin.g_objectList.deleteAll(true);
	   MainWin.g_flowDispatcher=null;
	}

	
	
/*    public void handleMessage(Message msg) // OnTimer
    {
       Message msg2=new Message();
       
	   checkObjectsForInput(MainWin.g_objectList,System.currentTimeMillis(),true);
	   sendMessageDelayed(msg2,MainWin.g_objectList.m_projectData.timerResolution);
	}*/



/*	static void on_timer_flowWalkerPool(void *data,int timerID)
	{
	   FlowWalkerPool *object;

	   object=(FlowWalkerPool*)data;
	   object.OnTimer();
	   object.outputPollTimer=oapc_thread_timer_start(on_timer_flowWalkerPool,g_objectList.m_projectData.timerResolution,object,POOL_ID_TIMER);
	}*/



/*	void startTimer()
	{
	   Message msg;
	   
	   if (outputPollTimer)
	   {
	      assert(false);
	      return;
	   }
	   msg=new Message();
	   sendMessageDelayed(msg,MainWin.g_objectList.m_projectData.timerResolution);
	   outputPollTimer=true;
	}*/




/*	void FlowWalkerPool_oapc_io_callback(unsigned long outputs,unsigned long callbackID)
	{
	   flowObject *object;

	   oapcMutexLocker lock(g_flowPool.inputPollLock,__FILE__,__LINE__);
	#ifndef ENV_PLUGGER
	   object=g_objectList.getObject(callbackID);
	   if (object)
	   {
//	      object.lastPollTime=0;
	      g_flowPool.checkObjectForInput(object,outputs,wxGetLocalTimeMillis(),true);
	   }
	#else
	   g_flowPool.checkObjectForInput(g_object,outputs,wxGetLocalTimeMillis(),true);
	#endif
	}*/



/*   void checkObjectsForInput(ObjectList olist,long currentTime,boolean doLockSection)
   {
	  flowObject object;
	   
	  if (olist==null) return;
	  if (doLockSection) inputPollLock.lock();
	  object=olist.getObject(null);
	  while (object)
      {
	         if (object.data.type==flowObject.FLOW_TYPE_GROUP) // TODO: remove this statement when using ACPX files only, it eats a namable amount of CPU time
	          checkObjectsForInput(((flowGroup)object).getObjectList(),currentTime,false);
	         else
	         {
	            if (object.data.type==FLOW_TYPE_EXTERNAL_LIB)
	            {
	               if ((((flowExternalIOLib*)object).lib.getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK)==0)
	                checkObjectForInput(object,object.data.stdOUT,currentTime,false);
	            }
	            else if (object.data.type==HMI_TYPE_EXTERNAL_LIB)
	            {
	               if ((((hmiExternalIOLib*)object).lib.getLibCapabilities() & OAPC_ACCEPTS_IO_CALLBACK)==0)
	                checkObjectForInput(object,object.data.stdOUT,currentTime,false);
	            }
	         }
         object=olist.getObject(object);
      }
      if (doLockSection) inputPollLock.unlock();
   }*/



/*	bool FlowWalkerPool::sendLogMessageToIS(wxInt32 logID,wxString message)
	{
	   struct oapc_char_value_block charMsgValue;
	   char                         nodeName[MAX_NODENAME_LENGTH+4];
	   wxInt32                      i;

	   if ((g_objectList.m_projectData.flags & PROJECT_FLAG_ISPACE_MASK)==0) return true;
	   if (!g_ispaceHandle) openISConnection(false);
	   if (!g_ispaceHandle) return false;
	   if (logID==0) strncpy(nodeName,"/system/errmsg",MAX_NODENAME_LENGTH);
	   else if (logID==1) strncpy(nodeName,"/system/warnmsg",MAX_NODENAME_LENGTH);
	   else if (logID==2) strncpy(nodeName,"/system/infomsg",MAX_NODENAME_LENGTH);
	   else if (logID==3) strncpy(nodeName,"/system/evtmsg",MAX_NODENAME_LENGTH);
	   else
	   {
	      wxASSERT(0);
	      return false;
	   }
	   memcpy(charMsgValue.charValue,message.c_str(),MAX_TEXT_LEN);

	   for (i=0; i<2; i++)
	   {
	      if (oapc_ispace_set_value(g_ispaceHandle,nodeName,OAPC_CHAR_IO1,&charMsgValue,&g_auth)!=OAPC_OK)
	      {
	         if ((i==1) || (!openISConnection(false)))
	         {
	            // TODO: implement break up/error reaction
	            g_ispaceHandle=NULL;
	            return false;
	         }
	      }
	      else return true;
	   }
	   return true;
	}*/




	
   class MyOAPCListener implements OAPCIServerListener
   {
      public void dataReceived(String nodeName,int cmd,long ios,IServerData[] values)
	  {
         flowObject object;
	     long       bitmask=0x01010101;
	     int        i;
//		 String     objectName; -> nodeName

         if (nodeName.endsWith("/out"))
         {
		    FlowConnection connection;
			      
            nodeName=nodeName.substring(0,nodeName.length()-4);
            object=MainWin.g_objectList.getObject(nodeName,false);
            if (object!=null)
            {
               if ((object.m_flowFlags & flowObject.FLAG_ALLOW_MOD_FROM_ISERVER)==0)
               {
                  return;  // object doesn't allows modification from outside
               }
               connection=object.getFlowConnection(null);
               while (connection!=null)
               {
			      MainWin.g_listMutex.lock();
                  bitmask=0x01010101;
	              for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	              {
	                 if (values[i]!=null)
                     {
	                    if ((ios & bitmask & OAPCIServer.OAPC_DIGI_IO_MASK & connection.data.sourceOutput)!=0)
                        {
	                       FlowWalkerThread digiThread;

                           digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_DIGI,System.currentTimeMillis(),0L);
                           digiThread.setFirstOutValue(values[i].digi,object);
                           MainWin.g_flowDispatcher.appendNewThread(digiThread);
                        }
                        else if ((ios & bitmask & OAPCIServer.OAPC_NUM_IO_MASK & connection.data.sourceOutput)!=0)
                        {
                           FlowWalkerThread numThread;

                           numThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_NUM,System.currentTimeMillis(),0);
	                       numThread.setFirstOutValue(values[i].num,object);
	                       MainWin.g_flowDispatcher.appendNewThread(numThread);
	                    }
                        else if ((ios & bitmask & OAPCIServer.OAPC_CHAR_IO_MASK & connection.data.sourceOutput)!=0)
                        {
                           FlowWalkerThread charThread;

                           charThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_CHAR,System.currentTimeMillis(),0);
	                       charThread.setFirstOutValue(values[i].str,object);
	                       MainWin.g_flowDispatcher.appendNewThread(charThread);
	                    }
                        else if ((ios & bitmask & OAPCIServer.OAPC_BIN_IO_MASK & connection.data.sourceOutput)!=0)
                        {
                           FlowWalkerThread binThread;

                           binThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_BIN,System.currentTimeMillis(),0);
                           binThread.setFirstOutValue(values[i].bin,object);
                           MainWin.g_flowDispatcher.appendNewThread(binThread);
                        }
                     }
                  }
                  bitmask=bitmask<<1;
               }
               MainWin.g_listMutex.unlock();
               connection=object.getFlowConnection(connection);
	        }
	     }
/*	     else
         {
            flowISConnect  *ISCOobject;
			         wxUint64        outputFlag;

			         ISCOobject=(flowISConnect*)g_objectList.getParentISCOObject(objectName);
			         if (ISCOobject)
			         {
			            oapcMutexLocker locker(g_listMutex,__FILE__,__LINE__);
			            bitmask=0x01010101;
			            for (i=0; i<MAX_NUM_IOS; i++)
			            {
			               if (values[i])
			               {
			                  object=ISCOobject.getTargetObjectInfo(objectName,i,&outputFlag);
			                  wxASSERT(object);
			                  if (!object) return;
			                  node=ISCOobject.getFlowConnection((wxNode*)NULL);
			                  while (node)
			                  {
			                     connection=(FlowConnection*)node.GetData();
			                     if (connection)
			                     {
			                        if (connection.data.sourceOutput==outputFlag)
			                        {
			                           if (OAPC_DIGI_IO_MASK & connection.data.sourceOutput)
			                           {
			                              struct oapc_digi_value_block *digiValue;
			                              FlowWalkerThread             *digiThread;

			                              digiValue=(struct oapc_digi_value_block*)values[i];
			                              digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,wxGetLocalTimeMillis(),0);
			                              digiThread.setFirstOutValue(digiValue.digiValue,object);
			                              g_flowDispatcher.appendNewThread(digiThread);
			                           }
			                           else if (OAPC_NUM_IO_MASK & connection.data.sourceOutput)
			                           {
			                              struct oapc_num_value_block *numValue;
			                              wxFloat64                    d;
			                              FlowWalkerThread            *numThread;

			                              numValue=(struct oapc_num_value_block*)values[i];
			                              d=oapc_util_block_to_dbl(numValue);
			                              numThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,wxGetLocalTimeMillis(),0);
			                              numThread.setFirstOutValue(d,object);
			                              g_flowDispatcher.appendNewThread(numThread);
			                           }
			                           else if (OAPC_CHAR_IO_MASK & connection.data.sourceOutput)
			                           {
			                              struct oapc_char_value_block *charValue;
			                              FlowWalkerThread             *charThread;

			                              charValue=(struct oapc_char_value_block*)values[i];
			                              charThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,wxGetLocalTimeMillis(),0);
			                              charThread.setFirstOutValue(g_charToStringUTF8(charValue.charValue,MAX_TEXT_LEN),object);
			                              g_flowDispatcher.appendNewThread(charThread);
			                           }
			                           else if (OAPC_BIN_IO_MASK & connection.data.sourceOutput)
			                           {
			                              struct oapc_bin_head        *binValueIn,*binValue;
			                              FlowWalkerThread            *binThread;

			                              binValueIn=(struct oapc_bin_head*)values[i];
			                              wxASSERT(binValueIn.sizeData+sizeof(struct oapc_bin_head)>0);
			                              wxASSERT(binValueIn.sizeData+sizeof(struct oapc_bin_head)<1000000);
			                              binValue=(struct oapc_bin_head*)malloc(binValueIn.sizeData+sizeof(struct oapc_bin_head));
			                              wxASSERT(binValue);
			                              if (binValue)
			                              {
			                                 memcpy(binValue,binValueIn,binValueIn.sizeData+sizeof(struct oapc_bin_head)-1);
			                                 binValue.int1=0; // reset usage counter
			                                 binThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,wxGetLocalTimeMillis(),0);
			                                 binThread.setFirstOutValue(binValue,object);
			                                 g_flowDispatcher.appendNewThread(binThread);
			                              }
			                           }
			                           break;
			                        }
			                     }
			                     node=object.getFlowConnection(node);
			                  }
			               }
			               bitmask=bitmask<<1;
			            }
			         }
			      }
					return;
				}*/
         if (nodeName.endsWith("/in"))
         {
            object=MainWin.g_objectList.getObject(nodeName,false);
            if (object==null) return; // value changed that doesn't exist within the scope of the player
            if ((object.m_flowFlags & flowObject.FLAG_ALLOW_MOD_FROM_ISERVER)==0)
            {
               return; // object doesn't allows modification from outside
            }
			MainWin.g_listMutex.lock();
            for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
		    {
		       if (values[i]!=null)
               {
		          if ((ios & bitmask & OAPCIServer.OAPC_DIGI_IO_MASK)!=0)
                  {
                     FlowWalkerThread digiThread;
		             FlowConnection   connection;
			      		
                     connection=new FlowConnection();
			         assert(connection!=null);
			         if (connection!=null)
			      	 {
	            		connection.isTempConnection=true;
	            		connection.data.targetInput=ios & bitmask & OAPCIServer.OAPC_DIGI_IO_MASK;
	            		connection.targetInputNum=connection.getDigiIndexFromFlag(connection.data.targetInput);
                        connection.data.targetID=object.data.id;

		                digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_DIGI,System.currentTimeMillis(),0L);
                        digiThread.setFirstOutValue(values[i].digi,object);
		                MainWin.g_flowDispatcher.appendNewThread(digiThread);
                     }
			      }
			      else if ((ios & bitmask & OAPCIServer.OAPC_NUM_IO_MASK)!=0)
                  {
  	                 FlowWalkerThread numThread;
			         FlowConnection   connection;
			      		
                     connection=new FlowConnection();
                     assert(connection!=null);
                     if (connection!=null)
                     {
                        connection.isTempConnection=true;
			         	connection.data.targetInput=ios & bitmask & OAPCIServer.OAPC_NUM_IO_MASK;
			            connection.targetInputNum=connection.getNumIndexFromFlag(connection.data.targetInput);
			            connection.data.targetID=object.data.id;

			            numThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_NUM,System.currentTimeMillis(),0);
  	                    numThread.setFirstOutValue(values[i].num,object);
			            MainWin.g_flowDispatcher.appendNewThread(numThread);
			         }
			      }
			      else if ((ios & bitmask & OAPCIServer.OAPC_CHAR_IO_MASK)!=0)
   	      	      {
			         FlowWalkerThread charThread;
			         FlowConnection   connection;
			      		
  	      		     connection=new FlowConnection();
  	      		     assert(connection!=null);
	         		 if (connection!=null)
	         		 {
	         		    connection.isTempConnection=true;
			            connection.data.targetInput=ios & bitmask & OAPCIServer.OAPC_CHAR_IO_MASK;
			            connection.targetInputNum=connection.getCharIndexFromFlag(connection.data.targetInput);
			            connection.data.targetID=object.data.id;
			 
	                    charThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_CHAR,System.currentTimeMillis(),0);
	                    charThread.setFirstOutValue(values[i].str,object);
			            MainWin.g_flowDispatcher.appendNewThread(charThread);
			      	 }
   	      	      }
   	         	  else if ((ios & bitmask & OAPCIServer.OAPC_BIN_IO_MASK)!=0)
   	         	  {
		             FlowWalkerThread binThread;
			         FlowConnection   connection;
			            
		      	   	 connection=new FlowConnection();
		      	     assert(connection!=null);
	         		 if (connection!=null)
	         		 {
               		    connection.isTempConnection=true;
			         	connection.data.targetInput=ios & bitmask & OAPCIServer.OAPC_BIN_IO_MASK;
			            connection.targetInputNum=connection.getBinIndexFromFlag(connection.data.targetInput);
			            connection.data.targetID=object.data.id;

  		                binThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_BIN,System.currentTimeMillis(),0);
			            binThread.setFirstOutValue(values[i].bin,object);
			            MainWin.g_flowDispatcher.appendNewThread(binThread);
			         }
			      }
			   }
               bitmask=bitmask<<1;
	        }
	        MainWin.g_listMutex.unlock();
	     }
         else if (nodeName.contentEquals("/system/exit"))
  	     {
        	if (values[0]!=null)
        	 MainWin.g_breakThreads=values[0].digi;
         }
         else if (nodeName.contentEquals("/system/pluggererr"))
 	     {
 	        int errorCode=0;

			if (((ios & OAPCIServer.OAPC_NUM_IO0)!=0) && (values[0]!=null)) errorCode=(int)values[0].num;
	        if ((errorCode==jliboapc.OAPC_ERROR_CUSTOM) && ((ios & OAPCIServer.OAPC_CHAR_IO1)!=0) && (values[1]!=null))
	        {
			   flowObject eobject=new flowObject(null);

	           eobject.data.id=0;
	           eobject.data.name=values[1].str;
/* TODO:	           if (((ios & liboapc.OAPC_CHAR_IO2)!=0) && (values[2]!=null))
	            ctx.g_flowLog.setLogInfo(values[2].str,0,eobject);
	           else ctx.g_flowLog.setLogInfo(ctx.g_OAPCMessage(errorCode),0,eobject);*/
			   }
			}		   
		   
		   
	   }
   }	

   
   
	/**
	 * @param[in] executeServer defines if the server has to be started for a (re)connection or
	 *            not; this is not allowed when this function is called out of a thread context
	 */
	boolean openISConnection(boolean executeServer)
	{
	   String host;
	   
	   if ((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)==0) return true;
	   else if (((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)==common.PROJECT_FLAG_ISPACE_SINGLE_REMOTE) ||
	       ((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)==common.PROJECT_FLAG_ISPACE_DOUBLE_REMOTE))
	    host=MainWin.g_objectList.m_projectData.m_remSingleIP;
	   else return false;
		
	   try
	   {
          oapcIServer=new OAPCIServer(host);
          oapcListener=new MyOAPCListener();
          oapcIServer.addOAPCIServerListener(oapcListener);
	   }
	   catch (IOException ioe)
	   {
	      return false;
	   }
	   return true;
	}



	boolean sendDigiValueToIS(long targetInput,byte digiValue,flowObject object,boolean isOutput)
	{
/* TODO:	   if ((!g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
	   if ((g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
	  	{
	  		wxInt32 i;
	  		char   *nodeName;
	  		
	  		if (isOutput) nodeName=object.nodeNameOut;
	  		else nodeName=object.nodeNameIn;
	  		for (i=0; i<2; i++)
	  		{
	     		// TODO: put in queue and own thread to avoid blocking of data flow
	  	   	struct oapc_digi_value_block digiValueBlock;
	   		
	  		   digiValueBlock.digiValue=digiValue;
	         wxASSERT(targetInput & OAPC_DIGI_IO_MASK);
	         wxASSERT(strlen(nodeName));
	     		if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&digiValueBlock,&g_auth)!=OAPC_OK)
	  	   	{
	  		   	if ((i==1) || (!openISConnection(false)))
	  		   	{
	  			      // TODO: implement break up/error reaction
	               g_ispaceHandle=NULL;
	               return false;
	  		   	}
	  	   	}
	         else return true;
	  		}   		
	  	}*/
	  	return true;
	}



	boolean sendNumValueToIS(long targetInput,double numValue,flowObject object,boolean isOutput)
	{
/* TODO:	   if ((!g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
	   if ((g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
	  	{
	  		wxInt32 i;
	  		char   *nodeName;
	  		
	  		if (isOutput) nodeName=object.nodeNameOut;
	  		else nodeName=object.nodeNameIn;
	  		for (i=0; i<2; i++)
	  		{
	    		// TODO: put in queue and own thread to avoid blocking of data flow
	   		struct oapc_num_value_block numValueBlock;
	   		
	         wxASSERT(targetInput & OAPC_NUM_IO_MASK);
	         wxASSERT(strlen(nodeName));
	  	   	oapc_util_dbl_to_block(numValue,&numValueBlock);
	         if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&numValueBlock,&g_auth)!=OAPC_OK)
	  		   {
	  		   	if ((i==1) || (!openISConnection(false)))
	  		   	{
	  			      // TODO: implement break up/error reaction
	               g_ispaceHandle=NULL;
	               return false;
	  		   	}
	  		   }
	         else return true;
	  		}   		
	  	}*/
	  	return true;
	}



	boolean sendCharValueToIS(long targetInput,String charValue,flowObject object,boolean isOutput)
	{
/* TODO:	   if ((!g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
	   if ((g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
	  	{
	  		wxInt32 i;
	  		char   *nodeName;
	  		
	  		if (isOutput) nodeName=object.nodeNameOut;
	  		else nodeName=object.nodeNameIn;
	  		for (i=0; i<2; i++)
	  		{
	     		// TODO: put in queue and own thread to avoid blocking of data flow
	  	   	struct oapc_char_value_block charValueBlock;
	         wxMBConvUTF8  conv;

	         wxASSERT(targetInput & OAPC_CHAR_IO_MASK);
	         wxASSERT(strlen(nodeName));
	         memset(&charValueBlock,0,sizeof(struct oapc_char_value_block));
	         conv.WC2MB(charValueBlock.charValue,charValue,MAX_TEXT_LEN);
	     		if (oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,&charValueBlock,&g_auth)!=OAPC_OK)
	  	   	{
	  		   	if ((i==1) || (!openISConnection(false)))
	  		   	{
	  			      // TODO: implement break up/error reaction
	               g_ispaceHandle=NULL;
	               return false;
	  		   	}
	  	   	}
	         else return true;
	  		}   		
	  	}*/
	  	return true;
	}



	boolean sendBinValueToIS(long targetInput,oapc_bin_head binValue,flowObject object,boolean isOutput)
	{
/* TODO:	   if ((!g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER)) openISConnection(false);
	   if ((g_ispaceHandle) && (object.m_flowFlags & FLAG_MAP_OUTPUT_TO_ISERVER))
	  	{
	  		wxInt32 i,ret;
	  		char   *nodeName;
	  		
	  		if (isOutput) nodeName=object.nodeNameOut;
	  		else nodeName=object.nodeNameIn;
	  		for (i=0; i<2; i++)
	  		{
	     		// TODO: put in queue and own thread to avoid blocking of data flow
	         wxASSERT(targetInput & OAPC_BIN_IO_MASK);
	         wxASSERT(strlen(nodeName));
	         ret=oapc_ispace_set_value(g_ispaceHandle,nodeName,targetInput,binValue,&g_auth);
	  	   	if (ret!=OAPC_OK)
	  		   {
	  		   	if ((i==1) || (!openISConnection(false)))
	  		   	{
	  			      // TODO: implement break up/error reaction
	               g_ispaceHandle=NULL;
	               return false;
	  		   	}
	  	   	}
	         else return true;
	  		}   		
	  	}*/   	
	  	return true;
	}



/*	void FlowWalkerPool::checkObjectForInput(flowObject *object,wxUint64 outputs,wxLongLong currentTime,bool force)
	{
	   FlowConnection connection;

	   if (!g_flowDispatcher) return; // is not available in case the license dialogue is still open
	   if (((object.data.type==FLOW_TYPE_EXTERNAL_LIB) || (object.data.type==HMI_TYPE_EXTERNAL_LIB)) &&
	            (outputs) &&
	            ((object.lastPollTime+(object.m_cycleMicros/1000)<currentTime) || (force)))
	   {
	      wxUint64              bit=0x01010101;
	      wxInt32               rcode,i;
	      wxByte                digiValue=0;
	      wxFloat64             numValue=0.0;
	      wxString              charValue=_T("");
	      struct oapc_bin_head *binValue=NULL;
	      bool                  binValueUnused=true;

	      for (i=0; i<MAX_NUM_IOS; i++)
	      {
	         if (outputs & bit)
	         {
	            rcode=-1;
	            connection.data.sourceOutput=bit;
	            if (bit & outputs & OAPC_DIGI_IO_MASK)
	            {
	               connection.sourceOutputNum=connection.getDigiIndexFromFlag(connection.data.sourceOutput);
	               digiValue=object.getDigitalOutput(&connection,&rcode,0,currentTime);
	               sendDigiValueToIS(bit & outputs & OAPC_DIGI_IO_MASK,digiValue,object,true);
	#ifdef ENV_DEBUGGER
	               setNewValue(connection.sourceOutputNum,digiValue,object);
	#endif
	            }
	            if (bit & outputs & OAPC_NUM_IO_MASK)
	            {
	               connection.sourceOutputNum=connection.getNumIndexFromFlag(connection.data.sourceOutput);
	               numValue=object.getNumOutput(&connection,&rcode,0,currentTime);
	               sendNumValueToIS(bit & outputs & OAPC_NUM_IO_MASK,numValue,object,true);
	#ifdef ENV_DEBUGGER
	               setNewValue(connection.sourceOutputNum,numValue,object);
	#endif
	            }
	            if (bit & outputs & OAPC_CHAR_IO_MASK)
	            {
	               connection.sourceOutputNum=connection.getCharIndexFromFlag(connection.data.sourceOutput);
	               charValue=object.getCharOutput(&connection,&rcode,0);
	               sendCharValueToIS(bit & outputs & OAPC_CHAR_IO_MASK,charValue,object,true);
	#ifdef ENV_DEBUGGER
	               setNewValue(connection.sourceOutputNum,charValue,object);
	#endif
	            }
	            if (bit & outputs & OAPC_BIN_IO_MASK)
	            {
	               struct oapc_bin_head *tmp;

	               connection.sourceOutputNum=connection.getBinIndexFromFlag(connection.data.sourceOutput);
	               tmp=object.getBinOutput(&connection,&rcode,0);
	               if ((rcode==OAPC_OK) && (tmp))
	               {
	                  binValue=(struct oapc_bin_head*)malloc(tmp.sizeHead+tmp.sizeData);
	                  if (binValue)
	                  {
	                     memcpy(binValue,tmp,tmp.sizeHead+tmp.sizeData);
	                     object.releaseBinData(&connection);
	                     sendBinValueToIS(bit & outputs & OAPC_BIN_IO_MASK,binValue,object,true);
	#ifdef ENV_DEBUGGER
	                     setNewValue(connection.sourceOutputNum,binValue,object);
	#endif
	                  }
	               }
	               else binValue=NULL;
	             }

	             if ((rcode & OAPC_ERROR_MASK)==OAPC_OK)
	             {
	                wxNode         *node;
	                FlowConnection *connection;

	                oapcMutexLocker locker(g_listMutex,__FILE__,__LINE__);
	                node=object.getFlowConnection(NULL);
	                while (node)
	                {
	                   connection=(FlowConnection*)node.GetData();

	                   if (connection.data.sourceOutput & bit) // we found the flow connection that belongs to the current output
	                   {
	                      FlowWalkerThread *dataThread=NULL;
	                      
	                      if (bit & outputs & OAPC_DIGI_IO_MASK)
	                      {
	                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_DIGI,currentTime,0);
	                         dataThread.setFirstOutValue(digiValue,object);
	                         g_flowDispatcher.appendNewThread(dataThread);
	                      }
	                      if (bit & outputs & OAPC_NUM_IO_MASK)
	                      {
	                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_NUM,currentTime,0);
	                         dataThread.setFirstOutValue(numValue,object);
	                         g_flowDispatcher.appendNewThread(dataThread);
	                      }
	                      if (bit & outputs & OAPC_CHAR_IO_MASK)
	                      {
	                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_CHAR,currentTime,0);
	                         dataThread.setFirstOutValue(charValue,object);
	                         g_flowDispatcher.appendNewThread(dataThread);
	                      }
	                      if (bit & outputs & OAPC_BIN_IO_MASK)
	                      {
	                         dataThread=new FlowWalkerThread(object,connection,FlowWalkerThread::THREAD_MODE_BIN,currentTime,0);
	                         dataThread.setFirstOutValue(binValue,object);
	                         if (rcode & OAPC_INT_BIN_NOT_UNLOCKEDi) g_dataCtrl.setBinDataUnused(binValue,false);
	                         rcode&=~OAPC_INT_BIN_NOT_UNLOCKEDi;
	                         g_flowDispatcher.appendNewThread(dataThread);
	                         binValueUnused=false;
	                      }
	                   }
	                   node=object.getFlowConnection(node);
	                }
	             }
	             if ((binValue) && (binValueUnused))
	             {
	                free((void*)binValue);
	             }
	          }
	          bit=bit<<1;
	       }
	       object.lastPollTime=wxGetLocalTimeMillis();
	    }
	}*/



   void startInitialFlows(ObjectList olist)
   {
      flowObject object;

      //TODO: g_objectList.setUserPrivileges(NULL);
      if (olist==null) return;
	  object=olist.getObject(null);
	  while (object!=null)
	  {
         if (object.data.type==flowObject.FLOW_TYPE_FLOW_START)
	      startFlows(object,null,object.data.stdOUT,new Int(0),System.currentTimeMillis(),0);
   	     object=olist.getObject(object);
	  }
   }



	void startTimerFlows(ObjectList olist)
	{
	   flowObject object;

	   // start the flows that use timers and run periodically
	   if (olist==null) return;
	   object=olist.getObject(null);
	   while (object!=null)
	   {
	      if (object.data.type==flowObject.FLOW_TYPE_FLOW_TIMER) object.applyData(true);
	      object=olist.getObject(object);
	   }
	}



	/**
	This method starts new control flows for an object
	@param[in] object the object that has to be checked for valid outgoing connections and where
	                  new flowWalkerThreads have to be created for
	@param[in] thread an optionally existing flow thread, this one will continue with the first
	                  flowConnection, therefore all new threads have to be created for all following
	                  flowConnections only
	@param[in] inheritThreadID ID of a thread that will die after this call but where its context has not to die
	                  to avoid endless loops: if inheritThreadID is not equal 0 the first creadted thread will
	                  get this ID
	*/
   void startFlows(flowObject object,FlowWalkerThread thread,long usedInput,Int inheritThreadID,long inheritCreationTime,long lastInput)
   {
      FlowConnection connection;
	   FlowConnection statConnection=new FlowConnection();
	   long           assignedOutputs=0;
	   Int[]          rcode=new Int[OAPCIServer.MAX_NUM_IOS+1];
	   int            i;
	   Int            statRcode=new Int(jliboapc.OAPC_ERROR);
	   int            bitmask=0x01010101;

	   for (i=0; i<OAPCIServer.MAX_NUM_IOS+1; i++) rcode[i]=new Int(jliboapc.OAPC_ERROR);
	   if (MainWin.g_breakThreads) return;
	   flowSection.lock();
	   if (thread==null) // if there is no running thread this flow is started externally, not by a running flow
	   {
	      assignedOutputs=usedInput;
	   }
	   else
	   {
	      assignedOutputs=object.getAssignedOutput(usedInput);
	      if (assignedOutputs==0) // if it is an external lib we have to exit here: their outputs are checked periodically or by callback
	      {
	         thread.setNextConnection(null);
	         flowSection.unlock();
	         return;
	      }
	   }

	   for (i=0; i<OAPCIServer.MAX_NUM_IOS+1; i++) object.dataValid[i]=false;
	   
	   connection=object.getFlowConnection(null);
	   while (connection!=null)
	   {
	      rcode[connection.sourceOutputNum].val=jliboapc.OAPC_OK;

	      // prefetch the output data if not already done ***************************
	      assert(connection!=null);
	      if ((connection.data.sourceOutput & assignedOutputs)!=0)
	      {
	         if ((connection.data.sourceOutput & (OAPCIServer.OAPC_DIGI_IO_MASK|flowObject.FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))!=0)
	         {
	            if (!object.dataValid[connection.sourceOutputNum])
	             object.dataDigi[connection.sourceOutputNum]=object.getDigitalOutput(connection,rcode[connection.sourceOutputNum],usedInput,inheritCreationTime);
	         }
	         else if ((connection.data.sourceOutput & (OAPCIServer.OAPC_NUM_IO_MASK|flowObject.FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT))!=0)
	         {
	            if (!object.dataValid[connection.sourceOutputNum])
	             object.dataNum[connection.sourceOutputNum]=object.getNumOutput(connection,rcode[connection.sourceOutputNum],usedInput,inheritCreationTime);
	         }
	         else if ((connection.data.sourceOutput & (OAPCIServer.OAPC_CHAR_IO_MASK|flowObject.FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT))!=0)
	         {
	            if (!object.dataValid[connection.sourceOutputNum])
	             object.dataTxt[connection.sourceOutputNum]=object.getCharOutput(connection,rcode[connection.sourceOutputNum],usedInput);
	         }
	         else if ((connection.data.sourceOutput & (OAPCIServer.OAPC_BIN_IO_MASK|flowObject.FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))!=0)
	         {
	            if (!object.dataValid[connection.sourceOutputNum])
	             object.dataBin[connection.sourceOutputNum]=object.getBinOutput(connection,rcode[connection.sourceOutputNum],usedInput);
	            //TODO: decrease oapc_bin_head usage ctr . really?
	         }
	         if ((rcode[connection.sourceOutputNum].val & jliboapc.OAPC_ERROR_MASK)!=jliboapc.OAPC_OK)
	         {
	            if ((rcode[connection.sourceOutputNum].val & jliboapc.OAPC_ERROR_MASK)==jliboapc.OAPC_ERROR_NO_DATA_AVAILABLE) // this message is sent in case of an overflow
	            {
//	            	   pthread_mutex_unlock(&flowSection);
//	               return 0;
	            }
	            else
	            {
	               assert(false);
//	            	   pthread_mutex_unlock(&flowSection);
//	               return 0;
	            }
	         }
	         else object.dataValid[connection.sourceOutputNum]=true;
	      }
	      // end of prefetch the output data if not already done ********************
          connection=object.getFlowConnection(connection);
	   }


	   for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	   {
	      statConnection.sourceOutputNum=i;
	      if ((assignedOutputs & bitmask & OAPCIServer.OAPC_DIGI_IO_MASK)!=0)
	      {
	         if (!object.dataValid[i])
	          object.dataDigi[i]=object.getDigitalOutput(statConnection,statRcode,usedInput,inheritCreationTime);
	         sendDigiValueToIS(assignedOutputs & bitmask & OAPCIServer.OAPC_DIGI_IO_MASK,object.dataDigi[i],object,true);
	      }
	      else if ((assignedOutputs & bitmask & OAPCIServer.OAPC_NUM_IO_MASK)!=0)
	      {
	         if (!object.dataValid[i])
	          object.dataNum[i]=object.getNumOutput(statConnection,statRcode,usedInput,inheritCreationTime);
	         sendNumValueToIS(assignedOutputs & bitmask & OAPCIServer.OAPC_NUM_IO_MASK,object.dataNum[i],object,true);
	      }
	      else if ((assignedOutputs & bitmask & OAPCIServer.OAPC_CHAR_IO_MASK)!=0)
	      {
	         if (!object.dataValid[i])
	          object.dataTxt[i]=object.getCharOutput(statConnection,statRcode,usedInput);
	         sendCharValueToIS(assignedOutputs & bitmask & OAPCIServer.OAPC_CHAR_IO_MASK,object.dataTxt[i],object,true);
	      }
	      else if ((assignedOutputs & bitmask & OAPCIServer.OAPC_BIN_IO_MASK)!=0)
	      {
	         if (!object.dataValid[i])
	          object.dataBin[i]=object.getBinOutput(statConnection,statRcode,usedInput);
	         sendBinValueToIS(assignedOutputs & bitmask & OAPCIServer.OAPC_BIN_IO_MASK,object.dataBin[i],object,true);
	      }
	      bitmask=bitmask<<1;
	   }


	   connection=object.getFlowConnection(null);
	   if ((connection==null) && (thread!=null)) thread.setNextConnection(null); // this flow ends here because there are no outgoing flow connections
	   while (connection!=null)
	   {
	      MainWin.g_listMutex.lock();
	      if (((connection.data.sourceOutput & assignedOutputs)!=0) && (object.dataValid[connection.sourceOutputNum]))
	      {
	         if (thread!=null)
	         {
	            thread.setNextConnection(connection);
	            if ((connection.data.sourceOutput & assignedOutputs & (OAPCIServer.OAPC_DIGI_IO_MASK|flowObject.FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT))==connection.data.sourceOutput)
	            {
	               thread.setMode(FlowWalkerThread.THREAD_MODE_DIGI);
	               thread.setFirstOutValue(object.dataDigi[connection.sourceOutputNum],object);
	               thread=null;
	            }
	            else if ((connection.data.sourceOutput & assignedOutputs & (OAPCIServer.OAPC_NUM_IO_MASK|flowObject.FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT))==connection.data.sourceOutput)
	            {
	               thread.setMode(FlowWalkerThread.THREAD_MODE_NUM);
	               thread.setFirstOutValue(object.dataNum[connection.sourceOutputNum],object);
	               thread=null;
	            }
	            else if ((connection.data.sourceOutput & assignedOutputs & (OAPCIServer.OAPC_CHAR_IO_MASK|flowObject.FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT))==connection.data.sourceOutput)
	            {
	               thread.setMode(FlowWalkerThread.THREAD_MODE_CHAR);
	               thread.setFirstOutValue(object.dataTxt[connection.sourceOutputNum],object);
	               thread=null;
	            }
	            else if ((connection.data.sourceOutput & assignedOutputs & (OAPCIServer.OAPC_BIN_IO_MASK|flowObject.FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT))==connection.data.sourceOutput)
	            {
	               thread.setMode(FlowWalkerThread.THREAD_MODE_BIN);
	               thread.setFirstOutValue(object.dataBin[connection.sourceOutputNum],object);
// TODO:	               if (rcode[connection.sourceOutputNum] & liboapc.OAPC_INT_BIN_NOT_UNLOCKEDi) ctx.g_dataCtrl.setBinDataUnused(object.dataBin[connection.sourceOutputNum],false);
	               rcode[connection.sourceOutputNum].val&=~jliboapc.OAPC_INT_BIN_NOT_UNLOCKEDi;
	               thread=null;
	            }
	         }
	         else if ((connection!=null) &&
	                  ((connection.data.sourceOutput & assignedOutputs & OAPCIServer.OAPC_DIGI_IO_MASK)==connection.data.sourceOutput))
	         {
	            FlowWalkerThread digiThread;
	             digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_DIGI,inheritCreationTime,lastInput);
	            if (digiThread!=null)
	            {
	               if (inheritThreadID.val>0) digiThread.setFlowThreadID(inheritThreadID);
	               digiThread.setFirstOutValue(object.dataDigi[connection.sourceOutputNum],object);
	               inheritThreadID.val=0;
	               MainWin.g_flowDispatcher.appendNewThread(digiThread);
	            }
	            else
	            {
	            }
	         }
	         else if ((connection!=null) &&
	                  ((connection.data.sourceOutput & assignedOutputs & OAPCIServer.OAPC_NUM_IO_MASK)==connection.data.sourceOutput))
	         {
	            FlowWalkerThread numThread;
	             numThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_NUM,inheritCreationTime,lastInput);
	            if (numThread!=null)
	            {
	               if (inheritThreadID.val>0) numThread.setFlowThreadID(inheritThreadID);
	               numThread.setFirstOutValue(object.dataNum[connection.sourceOutputNum],object);
	               inheritThreadID.val=0;
	               MainWin.g_flowDispatcher.appendNewThread(numThread);
	            }
	            else
	            {
	            }
	         }
	         else if ((connection!=null) &&
	                  ((connection.data.sourceOutput & assignedOutputs & OAPCIServer.OAPC_CHAR_IO_MASK)==connection.data.sourceOutput))
	         {
	            FlowWalkerThread charThread;
	             charThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_CHAR,inheritCreationTime,lastInput);
	            if (charThread!=null)
	            {
	               if (inheritThreadID.val>0) charThread.setFlowThreadID(inheritThreadID);
	               charThread.setFirstOutValue(object.dataTxt[connection.sourceOutputNum],object);
	               inheritThreadID.val=0;
	               MainWin.g_flowDispatcher.appendNewThread(charThread);
	            }
	            else
	            {
	            }
	         }
	         else if ((connection!=null) &&
	                  ((connection.data.sourceOutput & assignedOutputs & OAPCIServer.OAPC_BIN_IO_MASK)==connection.data.sourceOutput))
	         {
	            FlowWalkerThread binThread;
	             binThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_BIN,inheritCreationTime,lastInput);
	            if (binThread!=null)
	            {
	               if (inheritThreadID.val>0) binThread.setFlowThreadID(inheritThreadID);
	               binThread.setFirstOutValue(object.dataBin[connection.sourceOutputNum],object);
	               inheritThreadID.val=0;
	               MainWin.g_flowDispatcher.appendNewThread(binThread);
	            }
	            else
	            {
	           }
	        }
	     }
         MainWin.g_listMutex.unlock();
         connection=object.getFlowConnection(connection);
	  }
      if (thread!=null) thread.setNextConnection(null); // we had a thread but there could no output be found where we could continue it
      flowSection.unlock();
   }



   void startOverflowFlows(flowObject object,long assignedOutputs,byte firstDigiVal,double firstNumVal,String firstCharVal,oapc_bin_head firstBinVal,long inheritCreationTime)
   {
	   FlowConnection       connection;

	//   oapcMutexLocker lock(&flowSection,__FILE__,__LINE__);
	   MainWin.g_listMutex.lock();
	   connection=object.getFlowConnection(null);
	   while (connection!=null)
	   {
	       if (connection.data.sourceOutput!=0)
	       {
	          if ((connection!=null) && ((assignedOutputs & connection.data.sourceOutput & (flowObject.FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT|OAPCIServer.OAPC_DIGI_IO_MASK))!=0))
	          {
	             FlowWalkerThread digiThread;

	             digiThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_DIGI,inheritCreationTime,0L);
	             digiThread.setFirstOutValue(firstDigiVal,object);
	             MainWin.g_flowDispatcher.appendNewThread(digiThread);
	          }
	          else if ((connection!=null) && ((assignedOutputs & connection.data.sourceOutput & (flowObject.FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT|OAPCIServer.OAPC_NUM_IO_MASK))!=0))
	          {
	             FlowWalkerThread numThread;

	             numThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_NUM,inheritCreationTime,0);
	             numThread.setFirstOutValue(firstNumVal,object);
	             MainWin.g_flowDispatcher.appendNewThread(numThread);
	          }
	          else if ((connection!=null) && ((assignedOutputs & connection.data.sourceOutput & (flowObject.FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT|OAPCIServer.OAPC_CHAR_IO_MASK))!=0))
	          {
	             FlowWalkerThread charThread;

	             charThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_CHAR,inheritCreationTime,0);
	             charThread.setFirstOutValue(firstCharVal,object);
	             MainWin.g_flowDispatcher.appendNewThread(charThread);
	          }
	          else if ((connection!=null) && ((assignedOutputs & connection.data.sourceOutput & (flowObject.FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT|OAPCIServer.OAPC_BIN_IO_MASK))!=0))
	          {
	             FlowWalkerThread binThread;

	             binThread=new FlowWalkerThread(object,connection,FlowWalkerThread.THREAD_MODE_BIN,inheritCreationTime,0);
	             binThread.setFirstOutValue(firstBinVal,object);
	             MainWin.g_flowDispatcher.appendNewThread(binThread);
	          }
	       }
	       connection=object.getFlowConnection(connection);
	   }
	   MainWin.g_listMutex.unlock();
   }



	
	

}
