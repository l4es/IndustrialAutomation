package com.oapc;

import android.widget.*;
import android.view.*;

import com.oapc.iserver.*;
import com.oapc.jcommon.*;



public class hmiToggleButton extends hmiSimpleButton 
{	
	hmiToggleButton(BasePanel parent,flowObject obj)
	{
	   super(parent,obj);
	   data.type=HMI_TYPE_TOGGLEBUTTON;
	   data.stdIN= OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1|OAPCIServer.OAPC_CHAR_IO3;
	   data.stdOUT=OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1;
	   m_mutexID=-1;
	}

	
	
   View createUIElement()
	{
      if (uiElement==null)
	    uiElement=new ToggleButton(m_parent.getContext());
	   super.createUIElement();
       uiElement.setOnClickListener(MainWin.g_hmiCanvas);
	   return uiElement;
	}



	void close()
	{
		super.close();
	}



	void applyData(boolean all)
	{
	   super.applyData(all);
	   if (uiElement instanceof ToggleButton)
	   {
         ((ToggleButton)uiElement).setTextOff(data.text[digi[0]]);
         ((ToggleButton)uiElement).setTextOn(data.text[digi[0]]);
	   }
      uiElement.invalidate();
	}



	boolean setDigitalInput(FlowConnection connection,byte value,Int flowThreadID,flowObject object)
	{
	//   if (!threadIDOK(flowThreadID,false)) return 0;
/*	   if (connection.targetInputNum==0) this is done in super-method
	   {
	      ((ToggleButton)uiElement).setText(data.text[value]);
	   }
	   else*/
	   if (connection.targetInputNum==1) setSelected(value!=0);
	   return super.setDigitalInput(connection,value,flowThreadID,object);
	}



	void updateSelectionState()
	{
	   if (((CompoundButton)uiElement).isChecked()) setSelected(true);
	}



	void setSelected(boolean selected)
	{
      ((CompoundButton)uiElement).setChecked(selected);
	   if (!selected) MainWin.g_hmiCanvas.onClick(uiElement);
	   if ((selected) && (m_mutexID>0))
	   {
	      hmiMutex                 mutex;
	      hmiObject                object;

	      mutex=MainWin.g_objectList.getMutexByObjectID(data.id);
	      if (mutex!=null)
	      {
	         int i,id;
	         
	         for (i=0; i<mutex.m_idQ.size(); i++)
	         {
	            id=((Int)mutex.m_idQ.elementAt(i)).val;
	            if (id!=data.id)
	            {
	               object=(hmiObject)MainWin.g_objectList.getObject(id);
	               if (object!=null) object.setSelected(false);
	            }
	         }
	      }
	   }
	}



   byte getDigitalOutput(FlowConnection connection,Int rcode,long lastInput,long origCreationTime)
	{
	   byte val;

	   if (connection.sourceOutputNum==1)
	   {
	      rcode.val=jliboapc.OAPC_OK;
	      if (((CompoundButton)uiElement).isChecked()) val=1;
	      else val=0;
	      logDataCtrl.logDigi(val,this);
	      return val;
	   }
	   return super.getDigitalOutput(connection,rcode,lastInput,origCreationTime);
	}



	long getAssignedOutput(long input)
	{
	   if (input==OAPCIServer.OAPC_DIGI_IO1) return OAPCIServer.OAPC_DIGI_IO1;
	   else return super.getAssignedOutput(input);
	}
	

}
