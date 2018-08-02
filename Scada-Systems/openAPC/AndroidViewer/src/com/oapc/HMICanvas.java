package com.oapc;

import android.content.*;
import android.os.Message;
import android.widget.*;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.view.*;
import android.view.View.OnClickListener;

import com.oapc.iserver.OAPCIServer;
import com.oapc.jcommon.*;



public class HMICanvas extends BasePanel implements OnClickListener, OnSeekBarChangeListener 
{
   private AbsoluteLayout m_owner;

   HMICanvas(AbsoluteLayout parent,Context context,int x,int y,int width,int height)
   {
      super(parent,context,x,y,width,height);
      m_owner=parent;
	   m_myID=0;
	   init();
   }
	
	
	
   HMICanvas(AbsoluteLayout parent,Context context,int myID)
   {
      super(parent,context,myID);
      m_owner=null;
      m_myID=myID;
	   init();
   }
	
	
	
   void init()
   {
      if (m_myID==0) MainWin.g_objectList.setParent(this);
	 /*#ifdef ENV_PLAYER
	   if (!g_externalIOLibs) g_externalIOLibs=new ExternalIOLibs();
	   if (!g_externalHMILibs) g_externalHMILibs=new ExternalIOLibs();*/
	 /*#ifdef ENV_WINDOWS
	   RegisterHotKey(1,wxMOD_ALT,WXK_TAB);
	   RegisterHotKey(2,wxMOD_ALT|wxMOD_CONTROL,WXK_DELETE);
	   RegisterHotKey(3,wxMOD_ALT|wxMOD_CONTROL,WXK_BACK);
	   RegisterHotKey(4,wxMOD_WIN,0x45); // explorer
	   RegisterHotKey(5,wxMOD_CONTROL|wxMOD_SHIFT,WXK_ESCAPE); // taskmanager
	   RegisterHotKey(6,wxMOD_WIN,0x4C); // log off
	  #endif*/
	   this.requestFocus();
   }
	
	
	
   void close()
   {
      // TODO:	   g_selectedList.deleteAll(0);
   }

   
   
   void refreshProject()
   {
      if (MainWin.g_objectList.m_projectData==null) return;      
//      SetSize(wxSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH));
      SetBackgroundColour(MainWin.g_objectList.m_projectData.bgCol);
//      GetParent()->SetSize(g_objectList.m_projectData->totalW,g_objectList.m_projectData->totalH);
      invalidate();
   }

   
   
   boolean loadProject(String projectPath)
   {
      String error=null;

      error=MainWin.g_objectList.loadProject(projectPath);
      if ((error==null) || (error.length()==0))
      {
         refreshProject();
//         g_flowPool.startTimer();
         return true;
      }
      else
   	  {
         Message msg=new Message();
         
         msg.what=MainWin.UIHandler.MSG_MESSAGEBOX_OK;
         msg.obj=projectPath+":\n"+error;
         MainWin.g_uiHandler.sendMessage(msg);
   	  }
      return false;
   }



   void newProject()
   {
/*      g_objectList.deleteAll(1);
      g_objectList.init();*/
      refreshProject();
   }
   
   
/*   void HMICanvas::OnButtonClicked(wxCommandEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
          g_flowPool.startFlows(object,NULL,OAPC_DIGI_IO1,0,wxGetLocalTimeMillis(),0);
          object->updateSelectionState();
      }
   }



   void HMICanvas::OnSliderScrolled(wxScrollEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
      }
   }*/



   void DoOnSliderScrolled(flowObject object,int outflag,Int inheritThreadID,long inheritCreationTime)
   {
      MainWin.g_flowPool.startFlows(object,null,outflag,inheritThreadID,inheritCreationTime,0);
   }



/*   void HMICanvas::OnSliderScrolling(wxScrollEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
         {
//            if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
            {
//               ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
               g_flowPool.startFlows(object,NULL,OAPC_NUM_IO6,0,wxGetLocalTimeMillis(),0);
            }
         }
         else if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
         {
//            if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
            {
//               ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
               g_flowPool.startFlows(object,NULL,OAPC_NUM_IO7,0,wxGetLocalTimeMillis(),0);
            }
         }
      }
   }



   void HMICanvas::OnNumberChanged(wxCommandEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
      }
   }



   void HMICanvas::OnNumberFocusChanged(wxFocusEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
         {
   #ifdef ENV_DEBUGGER
            if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
   #endif
   		 if (object->valueHasChanged())
             DoOnSliderScrolled(object,OAPC_NUM_IO6,0,wxGetLocalTimeMillis());
         }
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
         {
   #ifdef ENV_DEBUGGER
            if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
   #endif
   		 if (object->valueHasChanged())
             DoOnSliderScrolled(object,OAPC_NUM_IO7,0,wxGetLocalTimeMillis());
         }
      }
   }



   void HMICanvas::OnCharChanged(wxCommandEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6) DoOnSliderScrolled(object,OAPC_CHAR_IO6,0,wxGetLocalTimeMillis());
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7) DoOnSliderScrolled(object,OAPC_CHAR_IO7,0,wxGetLocalTimeMillis());
      }
   }



   void HMICanvas::OnCharFocusChanged(wxFocusEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT6)
         {
   #ifdef ENV_DEBUGGER
            if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
   #endif
   		 if (object->valueHasChanged())
             DoOnSliderScrolled(object,OAPC_CHAR_IO6,0,wxGetLocalTimeMillis());
         }
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGED_OUT7)
         {
   #ifdef ENV_DEBUGGER
            if (g_debugWin) g_debugWin->setDebugInformation(object,DEBUG_INFO_FOCUS_LOST,_T(""));
   #endif
   		 if (object->valueHasChanged())
             DoOnSliderScrolled(object,OAPC_CHAR_IO7,0,wxGetLocalTimeMillis());
         }
      }
   }



   void HMICanvas::OnNumberChanging(wxCommandEvent &event)
   {
      hmiObject *object;

      object=g_objectList.getObjectByUIElementID(event.GetId());
      if (object)
      {
         if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT6)
         {
//            if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
            {
//               ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
               g_flowPool.startFlows(object,NULL,OAPC_NUM_IO6,0,wxGetLocalTimeMillis(),0);
            }
         }
         else if (object->data.dataFlowFlags & HMI_FLOWFLAG_ON_CHANGING_OUT7)
         {
//            if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
            {
//               ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
               g_flowPool.startFlows(object,NULL,OAPC_NUM_IO7,0,wxGetLocalTimeMillis(),0);
            }
         }
      }
   }


   void HMICanvas::OnSpinNumberChanging(wxSpinEvent& event)
   {
      wxCommandEvent cmdEvent;

      cmdEvent.SetId(event.GetId());
      OnNumberChanging(cmdEvent);
   }*/

   
   
   public void onClick(View v)
   {
      hmiObject object;

      object=MainWin.g_objectList.getObjectByViewObject(v);
      if (object!=null)
      {
         MainWin.g_flowPool.startFlows(object,null,OAPCIServer.OAPC_DIGI_IO1,new Int(0),System.currentTimeMillis(),0);
         object.updateSelectionState();
   /*       if ((object->data.type & HMI_TYPE_SIMPLEBUTTON)==HMI_TYPE_SIMPLEBUTTON)
          {
              wxMilliSleep(200);
              g_flowPool.startFlows(object,NULL,OAPC_DIGI_IO1,0,wxGetLocalTimeMillis(),0);
          }*/
   /*       else if ((object->data.type & HMI_TYPE_TOGGLEBUTTON)==HMI_TYPE_TOGGLEBUTTON)
           ((hmiToggleButton*)object)->applyToState();*/
      }     
   }
   
   
   
   // that's not OnSliderScrolling but OnSliderScrolled?
   public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
   {
      hmiObject object;

     object=MainWin.g_objectList.getObjectByViewObject(seekBar);
     if (object!=null)
      {
        if ((object.data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGING_OUT6)!=0)
         {
//          if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
           {
//             ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
              MainWin.g_flowPool.startFlows(object,null,OAPCIServer.OAPC_NUM_IO6,new Int(0),System.currentTimeMillis(),0);
           }
        }
        else if ((object.data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGING_OUT7)!=0)
         {
//          if (((hmiHSlider*)object)->lastThreadTime+100<wxGetLocalTimeMillis())
           {
//             ((hmiHSlider*)object)->lastThreadTime=wxGetLocalTimeMillis();
              MainWin.g_flowPool.startFlows(object,null,OAPCIServer.OAPC_NUM_IO7,new Int(0),System.currentTimeMillis(),0);
           }
        }
     }
   }
   
   
   
   public void onStartTrackingTouch(SeekBar seekBar)
   {
   }
   
   
   // that's not OnSliderScrolled, isn't it?
   public void onStopTrackingTouch(SeekBar seekBar)
   {
      hmiObject object;

      object=MainWin.g_objectList.getObjectByViewObject(seekBar);
      if (object!=null)
      {
         if ((object.data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGED_OUT6)!=0) DoOnSliderScrolled(object,OAPCIServer.OAPC_NUM_IO6,new Int(0),System.currentTimeMillis());
         if ((object.data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGED_OUT7)!=0) DoOnSliderScrolled(object,OAPCIServer.OAPC_NUM_IO7,new Int(0),System.currentTimeMillis());
      }
   }   
   
}

