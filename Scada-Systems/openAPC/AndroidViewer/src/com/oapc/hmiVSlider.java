package com.oapc;

import android.view.*;

import com.oapc.iserver.*;
import com.oapc.jcommon.*;



public class hmiVSlider extends hmiHSlider
{

   hmiVSlider(BasePanel parent,flowObject obj)
   {
      super(parent,obj);
      data.type=hmiObject.HMI_TYPE_VSLIDER;
      data.stdIN =OAPCIServer.OAPC_DIGI_IO0|OAPCIServer.OAPC_DIGI_IO1|OAPCIServer.OAPC_NUM_IO2|OAPCIServer.OAPC_NUM_IO3|             OAPCIServer.OAPC_NUM_IO7;
      data.stdOUT=OAPCIServer.OAPC_DIGI_IO0|                                                                OAPCIServer.OAPC_NUM_IO6|OAPCIServer.OAPC_NUM_IO7;
      num=0;
   }   
 
   
   View createUIElement()
   {
      if (uiElement==null)
       uiElement=new VerticalSeekBar(m_parent.getContext());
      return super.createUIElement();
   }
   
   
}
