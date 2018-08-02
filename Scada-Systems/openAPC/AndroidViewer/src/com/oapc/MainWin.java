package com.oapc;

import java.util.concurrent.locks.ReentrantLock;

import android.app.*;
import android.content.*;
import android.os.*;
import android.widget.*;
import android.view.View.*;
import android.view.*;
import android.widget.SeekBar.*;

import com.oapc.jcommon.*;
import com.oapc.iserver.*;

public class MainWin extends Activity
{
    AbsoluteLayout       rootLayout;
   private Context              ctx;
   
   static  boolean              g_fileLoadWarned=false,g_breakThreads=false;
//           flowLog            g_flowLog=null;
   static  FlowWalkerPool       g_flowPool;
   static  FlowDispatcherThread g_flowDispatcher;
   static  ReentrantLock        g_listMutex=new ReentrantLock();
   static  oapcResourceBundle   g_res;
   static  ObjectList           g_objectList;
   static  HMICanvas            g_hmiCanvas;
   static  UIHandler            g_uiHandler;
           
   class UIHandler extends Handler
   {
      public static final int MSG_MESSAGEBOX_OK =1;
      public static final int MSG_TOGGLE_FLOW_IO=2;

      public void handleMessage(Message msg) 
	   {
    	   switch (msg.what)
    	   {
    	      case MSG_TOGGLE_FLOW_IO:
    	         hmiObjectMessage objMessage;
    	         
    	         objMessage=(hmiObjectMessage)msg.obj;
    	         switch (msg.arg1)
    	         {
    	            case hmiObjectMessage.DIGI_MESSAGE:
    	               objMessage.srcObject.setDigitalInput(objMessage.connection,objMessage.digiValue,objMessage.flowThreadID,objMessage.object);
    	               break;
    	            case hmiObjectMessage.NUM_MESSAGE:
                     objMessage.srcObject.setNumInput(objMessage.connection,objMessage.numValue,objMessage.flowThreadID,objMessage.object);
                     break;
    	            case hmiObjectMessage.CHAR_MESSAGE:
                     objMessage.srcObject.setCharInput(objMessage.connection,objMessage.charValue,objMessage.flowThreadID,objMessage.object);
                     break;
    	            case hmiObjectMessage.BIN_MESSAGE:
                     objMessage.srcObject.setBinInput(objMessage.connection,objMessage.binValue,objMessage.flowThreadID,objMessage.object);
                     break;
    	         }
    	         break;
            case MSG_MESSAGEBOX_OK:
            {
                 AlertDialog ad = new AlertDialog.Builder(ctx).create();  
                 ad.setCancelable(false);  
                 ad.setMessage((String)msg.obj);  
                 ad.setButton("OK", new DialogInterface.OnClickListener() 
                 {  
                     @Override  
                     public void onClick(DialogInterface dialog, int which) 
                     {  
                         dialog.dismiss();
                     }  
                 });  
                 ad.show();
                 break;
             }
             default:
                break;
    	  }			   
      }		
   }
   
   
   public void onSaveInstanceState (Bundle outState)
   {
      outState.putBoolean("init",true);      
   }

   
           
   public void onCreate(Bundle savedInstanceState) 
   {
      super.onCreate(savedInstanceState);
      ctx=this;
      setContentView(R.layout.main);
      rootLayout=(AbsoluteLayout)findViewById(R.id.rootLayout);
      
      if ((savedInstanceState!=null) && (savedInstanceState.getBoolean("init"))) return;
      
      g_uiHandler=new UIHandler();
      
      g_objectList=new ObjectList();
      g_res=new oapcResourceBundle(this,"openplayer");
      g_res.appendResource("common");
      
      g_hmiCanvas=new HMICanvas(rootLayout,this,0);

      if ((!g_hmiCanvas.loadProject("testfile")) || (g_objectList.m_projectData==null))
      {
         finish();
         System.exit(0);
      }
      g_flowPool=new FlowWalkerPool();
/*      g_flowPool.startTimer();*/
      g_flowDispatcher=new FlowDispatcherThread(this);

      g_flowPool.startInitialFlows(g_objectList);
      g_flowPool.startTimerFlows(g_objectList);
   }
   
   
   
   static void g_fileVersionError()
   {
      if (!g_fileLoadWarned)
      {
         Message msg=new Message();
         msg.what=UIHandler.MSG_MESSAGEBOX_OK;
         msg.obj=g_res.getString("Warning: This project file was created using the wrong version of the Application\nand may not work as expected.");
         g_uiHandler.sendMessage(msg);
      }
      assert(false);
      g_fileLoadWarned=true;
   }   
   
}


