package com.oapc;

import android.view.*;
import android.widget.*;

import com.oapc.iserver.*;
import com.oapc.jcommon.*;



public class hmiSimpleButton extends hmiObject 
{

   hmiSimpleButton(BasePanel parent,flowObject obj)
   {
      super(parent,obj);
	  data.type=HMI_TYPE_SIMPLEBUTTON;
	  data.stdIN=OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1|OAPCIServer.OAPC_CHAR_IO3;
	  data.stdOUT=OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1;
   }
		
		
   void close()
   {
      super.close();
   }
   
   
   View createUIElement()
   {
      if (uiElement==null)
      {
         uiElement=new Button(m_parent.getContext());
         uiElement.setOnClickListener(MainWin.g_hmiCanvas);         
      }
      return super.createUIElement();
   }



   void applyData(boolean all)
   {
	   super.applyData(all);
/*       if (all)
       {
          setSize(wxRealPoint(data.msizex,data.msizey));
          setPos(wxRealPoint(data.mposx,data.mposy));
       }*/
      ((Button)uiElement).setTextColor(wxConvert.fromWxColour(data.fgcolor[digi[0]]));
      ((Button)uiElement).setText(data.text[digi[0]]);
      applyFont(uiElement);
      uiElement.setEnabled((data.state[digi[0]] & HMI_STATE_FLAG_DISABLED)==0);
   }   

   
   void applyFont(View ui)
   {
      if ((lastFont_pointSize!=data.font[digi[0]].pointSize) && (data.font[digi[0]].pointSize>0))
       ((Button)ui).setTextSize(data.font[digi[0]].pointSize);
/* TODO: implement generic face flags and use them for wxWidgets application too  
      if ((lastFont_style    !=data.font[digi[0]].style) || (lastFont_weight   !=data.font[digi[0]].weight))
      {
         Typeface t;
         ((Button)ui).setTypeface(t);
      }*/
//          (lastFont_faceName.compareTo(data.font[digi[0]].fontFaceName)!=0))
      super.applyFont(ui);
   }

   
   
   boolean setDigitalInput(FlowConnection connection,byte value,Int flowThreadID,flowObject object)
   {
      if (super.setDigitalInput(connection,value,flowThreadID,object))
      {
         if (connection.targetInputNum==0) applyData(false);
         return true;
      }
      else return false;
   }



   byte getDigitalOutput(FlowConnection connection,Int rcode,long lastInput,long origCreationTime)
   {
      if (connection.sourceOutputNum==1)
      {
         logDataCtrl.logDigi((byte)1,this);
         logDataCtrl.logDigi((byte)0,this);
         return 1;
      }
      return super.getDigitalOutput(connection,rcode,lastInput,origCreationTime);
   }



   /**
   Sets an character input
   @param[in] input the flag that specifies the input
   @param[in] value the value that has to be set for that input
   @param[in] flowThreadID the Id of the thread that wants to set this value
   @return 0; this method has to be overridden
   */
   boolean setCharInput(FlowConnection connection,String value,Int flowThreadID,flowObject object)
   {
      if (!threadIDOK(flowThreadID,false)) return false;
      data.text[digi[0]]=value;
      applyData(false);
      return true;
   }



   long getAssignedOutput(long input)
   {
      if (input==OAPCIServer.OAPC_DIGI_IO1) return OAPCIServer.OAPC_DIGI_IO1;
      else return super.getAssignedOutput(input);
   }
   
	
}
