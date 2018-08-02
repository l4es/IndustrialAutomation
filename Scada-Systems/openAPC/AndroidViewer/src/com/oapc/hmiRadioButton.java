package com.oapc;

import android.widget.*;
import android.view.*;

import com.oapc.iserver.OAPCIServer;



public class hmiRadioButton extends hmiToggleButton
{

   
   hmiRadioButton(BasePanel parent,flowObject obj,int type)
   {
      super(parent, obj);
      data.type=type;
      data.stdIN= OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1|OAPCIServer.OAPC_CHAR_IO3;
      data.stdOUT=OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1;
   }



   View createUIElement()
   {
      if (data.type==hmiObject.HMI_TYPE_RADIOBUTTON) uiElement=new RadioButton(m_parent.getContext());
      else if (data.type==HMI_TYPE_CHECKBOX) uiElement=new CheckBox(m_parent.getContext());
      super.createUIElement();
/*      #ifdef ENV_PLAYER
      if (data.type==HMI_TYPE_RADIOBUTTON) g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_RADIOBUTTON_SELECTED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
      else if (data.type==HMI_TYPE_CHECKBOX) g_hmiCanvas->Connect(uiElement->GetId(),wxEVT_COMMAND_CHECKBOX_CLICKED,wxCommandEventHandler(HMICanvas::OnButtonClicked));
      else wxASSERT(0);
      #endif*/
      return uiElement;
   }

   
   void applyData(boolean all)
   {
      if (data.type==hmiObject.HMI_TYPE_RADIOBUTTON) ((RadioButton)uiElement).setText(data.text[digi[0]]);
      else if (data.type==HMI_TYPE_CHECKBOX) ((CheckBox)uiElement).setText(data.text[digi[0]]);
      super.applyData(all);
   }
   
   
}
