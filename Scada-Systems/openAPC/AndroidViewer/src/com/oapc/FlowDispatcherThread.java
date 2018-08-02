package com.oapc;

import java.util.*;



public class FlowDispatcherThread extends Thread
{
   private MainWin ctx;
   private Vector<FlowWalkerThread> m_threadList=new Vector<FlowWalkerThread>();

   FlowDispatcherThread(MainWin ctx)
   {
	   this.ctx=ctx;
      if ((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)!=0)
      {
         if (((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)==common.PROJECT_FLAG_ISPACE_SINGLE_REMOTE) ||
             ((MainWin.g_objectList.m_projectData.flags & common.PROJECT_FLAG_ISPACE_MASK)==common.PROJECT_FLAG_ISPACE_DOUBLE_REMOTE))
         {
            MainWin.g_flowPool.openISConnection(false); // do not try to start the server locally in case it is located somewhere else
         }
         else
         {
        	 // TODO: error message, no local IServwers possible
         }
      }
      start();
   }
        	 


   void close()
   {
   }
   
   
   void appendNewThread(FlowWalkerThread newThread)
   {
      m_threadList.add(newThread);
      this.interrupt();
   }

   
   
   public void run()
   {
      long             sTime,eTime;
      FlowWalkerThread eraseThread=null,currThread;
      int              listCtr=0;

//      std::list<FlowWalkerThread*>::iterator it,eraseIt;

      while (!MainWin.g_breakThreads)
      {
         MainWin.g_listMutex.lock();
	      if (m_threadList.size()>0) for (listCtr=0; listCtr<m_threadList.size(); listCtr++)
         {
            if (eraseThread!=null)
            {
               eraseThread.close();
               m_threadList.remove(eraseThread);
               eraseThread=null;
               listCtr--; // jump back one step to pick up the thread that follows this one
            }
	         eraseThread=null;
	         try
	         {
   	         currThread=m_threadList.elementAt(listCtr);
               MainWin.g_listMutex.unlock();
	   /*            if (eraseIt!=(std::list<FlowWalkerThread*>::iterator)NULL)
	               {
	                  delete (*eraseIt);
	                  m_threadList.erase(eraseIt); 
	                  eraseIt=(std::list<FlowWalkerThread*>::iterator)NULL;
	                  m_threadNum--;
	               }*/
               if (currThread.execute()==0) eraseThread=currThread;
/*            if (checkExecTime)
            {
               eTime=System.currentTimeMillis();
	           if (eTime-sTime>MAX_RUNTIME_MILLIS)
	           {
	           }
            }*/
               if ((currThread.connection!=null) && (currThread.connection.isTempConnection))
               {
                  currThread.connection.close();
                  currThread.connection=null;
               }
            }
            catch (ArrayIndexOutOfBoundsException aioobe)
            {
               //tolerate this exception because it may be caused by a removed thread 
            }
            MainWin.g_listMutex.lock();
         }
         if (eraseThread!=null)
         {
            eraseThread.close();
            m_threadList.remove(eraseThread);
            eraseThread=null;
         }
         MainWin.g_listMutex.unlock();
      }
      m_threadList.clear();

/*	      if (g_objectList.m_projectData->flags & PROJECT_FLAG_ISPACE_MASK)
	      {
	      	if (g_ispaceHandle) 
	      	{
	            struct oapc_digi_value_block digiValue;
	      	
	         	digiValue.digiValue=1;
	      	   oapc_ispace_set_value(g_ispaceHandle,"/system/exit",OAPC_DIGI_IO0,&digiValue,&g_auth); // change the value again
	         	oapc_ispace_disconnect(g_ispaceHandle);
	      	}
	      	g_ispaceHandle=NULL;
	      }*/
      MainWin.g_flowPool.exitApplication();
      ctx.finish();
      System.exit(0);
   }
	
}
