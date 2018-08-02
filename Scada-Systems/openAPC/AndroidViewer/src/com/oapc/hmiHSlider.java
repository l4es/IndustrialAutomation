package com.oapc;

import com.oapc.iserver.*;
import com.oapc.jcommon.*;

import android.view.*;
import android.widget.*;



public class hmiHSlider extends hmiObject
{
   double num=0;

   hmiHSlider(BasePanel parent,flowObject obj)
   {
      super(parent,obj);
      data.type=hmiObject.HMI_TYPE_HSLIDER;
      data.stdIN =OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1|OAPCIServer.OAPC_NUM_IO2|OAPCIServer.OAPC_NUM_IO3|             OAPCIServer.OAPC_NUM_IO7;
      data.stdOUT=OAPCIServer.OAPC_DIGI_IO0|                                                                OAPCIServer.OAPC_NUM_IO6|OAPCIServer.OAPC_NUM_IO7;
      num=0;
   }

   
   
   void close()
   {
	   super.close();
   }

   

   View createUIElement()
   {
      if (uiElement==null)
      {
         uiElement=new SeekBar(m_parent.getContext());
         if (uiElement==null) return null;
      }
      if (uiElement instanceof SeekBar)
      {
         ((SeekBar)uiElement).setMax((int)(MaxValue-MinValue));
         ((SeekBar)uiElement).setOnSeekBarChangeListener(MainWin.g_hmiCanvas);
      }
      return super.createUIElement();
   }



   String getDefaultName()
   {
      return "Horizontal Slider";
   }



   void applyData(boolean all)
   {
      super.applyData(all);
/*if (all)
{
setSize(wxRealPoint(data.msizex,data.msizey));
setPos(wxRealPoint(data.mposx,data.mposy));
}*/
      uiElement.setBackgroundColor(wxConvert.fromWxColour(data.bgcolor[digi[0]]));
// TODO:      ((SeekBar)uiElement).SetForegroundColour(wxColour(data.fgcolor[digi[0]]));
      uiElement.setEnabled((data.state[digi[0]] & hmiObject.HMI_STATE_FLAG_DISABLED)==0);
      if (uiElement instanceof SeekBar)
      {
         ((SeekBar)uiElement).setMax((int)(MaxValue-MinValue));
         ((SeekBar)uiElement).setProgress((int)(num-MinValue));
      }
   }  



   long getAssignedOutput(long input)
   {
      if (input==OAPCIServer.OAPC_NUM_IO7) return OAPCIServer.OAPC_NUM_IO7;
      return super.getAssignedOutput(input);
   }



	/**
	Sets an digital input, checks if the same thread has set that input before and uses the
	input as trigger for sending the own value (if configured)
	@param[in] input the flag that specifies the input
	@param[in] value the value that has to be set for that input (0 or 1)
	@param[in] flowThreadID the Id of the thread that wants to set this value
	@return 0 if the same thread already has set a value; the thread has to finish
	 in this case to avoid endless loops; 1 if the input value was set correctly
	*/
   boolean setDigitalInput(FlowConnection connection,byte value,Int flowThreadID,flowObject object)
   {
      if (!threadIDOK(flowThreadID,false)) return false;
	  if (connection.targetInputNum==1)
	  {
	     if (((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6)!=0) && (value!=0))
	      ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO6,flowThreadID,System.currentTimeMillis());
	     else if (((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6)!=0) && (value==0))
	      ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO6,flowThreadID,System.currentTimeMillis());
	     if (((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7)!=0) && (value!=0))
	      ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO7,flowThreadID,System.currentTimeMillis());
         else if (((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7)!=0) && (value==0))
	      ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO7,flowThreadID,System.currentTimeMillis());
         return false; // return with 0 because afterwards no digi thread has to run but a numerical thread
	   }
	   return super.setDigitalInput(connection,value,flowThreadID,object);
	}
	
	
	
	/**
	Sets an numerical input and checks if the same thread has set that input before
	@param[in] input the flag that specifies the input
	@param[in] value the value that has to be set for that input
	@param[in] flowThreadID the Id of the thread that wants to set this value
	@return 0 if the same thread already has set a value; the thread has to finish
	 in this case to avoid endless loops; 1 if the input value was set correctly
	*/
   boolean setNumInput(FlowConnection connection,double value,Int flowThreadID,flowObject object)
   {
	  boolean changed=false;
	   
	  if (!threadIDOK(flowThreadID,false)) return false;
	  if (connection.targetInputNum==2) this.MinValue=value;
	  else if (connection.targetInputNum==3) this.MaxValue=value;
	  else if (connection.targetInputNum==7)
	  {
	     num=value;
	     changed=true;
      }
	  else return false; 
	  if (num>MaxValue)
	  {
	     num=MaxValue;
	     changed=true;
	  }
	  else if (num<MinValue)
	  {
	     num=MinValue;
	     changed=true;
	  }
	  applyData(false);
	
	  if (changed)
	  {
		 if ((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGED_OUT6)!=0)
		  ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO6,flowThreadID,System.currentTimeMillis());
         if ((data.dataFlowFlags & hmiObject.HMI_FLOWFLAG_ON_CHANGED_OUT7)!=0)
	      ((HMICanvas)m_parent).DoOnSliderScrolled(this,OAPCIServer.OAPC_NUM_IO7,flowThreadID,System.currentTimeMillis());
	  }
      return true;
   }
	
	
	
   double getNumOutput(FlowConnection connection,Int rcode,long lastInput,long origCreationTime)
   {
	  if ((connection.sourceOutputNum==6) || (connection.sourceOutputNum==7))
	  {
	     int l;
	
	     l=(int)(((SeekBar)uiElement).getProgress()+MinValue);
	     if (l<MinValue) l=(int)MinValue;
	     else if (l>MaxValue) l=(int)MaxValue;
	     ((SeekBar)uiElement).setProgress((int)(l-MinValue));
	
	     logDataCtrl.logNum(l,this);
	
	     rcode.val=jliboapc.OAPC_OK;
	     return l;
	  }
	  else
	  {
         rcode.val=jliboapc.OAPC_ERROR_NO_SUCH_IO;
	     return 0.0;
	  }
   }	
	
}
