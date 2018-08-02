package com.oapc;

import android.view.*;
import android.widget.*;

import com.oapc.iserver.*;
import com.oapc.liboapc.*;
import android.os.*;



public class hmiObject extends flowObject 
{
   public static final int HMI_TYPE_MASK              =0x000000FF;
   public static final int HMI_TYPE_SIMPLEBUTTON      =0x00000001;
   public static final int HMI_TYPE_TOGGLEBUTTON      =0x00000002;
   public static final int HMI_TYPE_HSLIDER           =0x00000003;
   public static final int HMI_TYPE_VSLIDER           =0x00000004;
   public static final int HMI_TYPE_TEXTLABEL         =0x00000005;
   public static final int HMI_TYPE_LINE              =0x00000006;
   public static final int HMI_TYPE_IMAGEBUTTON       =0x00000007;
   public static final int HMI_TYPE_IMAGE             =0x00000008;
   public static final int HMI_TYPE_HGAUGE            =0x00000009;
   public static final int HMI_TYPE_VGAUGE            =0x0000000A;
   public static final int HMI_TYPE_NUMFIELD          =0x0000000B;
   public static final int HMI_TYPE_FLOATFIELD        =0x0000000C;
   public static final int HMI_TYPE_TEXTFIELD         =0x0000000D;
   public static final int HMI_TYPE_TABBEDPANE        =0x0000000E;
   public static final int HMI_TYPE_STACKEDPANE       =0x0000000F;
   public static final int HMI_TYPE_ADDITIONALPANE    =0x00000010;
   public static final int HMI_TYPE_TOGGLEIMAGEBUTTON =0x00000011;
   public static final int HMI_TYPE_ANGULARMETER      =0x00000012;
   public static final int HMI_TYPE_LCDNUMBER         =0x00000013;
   public static final int HMI_TYPE_ANGULARREGULATOR  =0x00000014;
   public static final int HMI_TYPE_EXTERNAL_LIB      =0x00000015;
   public static final int HMI_TYPE_SINGLEPANEL       =0x00000016;
   public static final int HMI_TYPE_RADIOBUTTON       =0x00000017;
   public static final int HMI_TYPE_CHECKBOX          =0x00000018;
   public static final int HMI_TYPE_SP_USERMANAGEMENT =0x00000019;
   public static final int HMI_TYPE_PASSWORDFIELD     =0x0000001A;

   public static final int HMI_STATE_FLAG_DISABLED =0x0001;
   public static final int HMI_STATE_FLAG_RO       =0x0004;

   public static final int HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT6 =0x0001; /** trigger data output on OUT 6 in case of a signal on digital input 0*/
   public static final int HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT6 =0x0002; /** trigger data output on OUT 6 in case of a signal on digital input 0*/
   public static final int HMI_FLOWFLAG_ON_DIGI_IN1_HI_OUT7 =0x0004; /** trigger data output on OUT 7 in case of a signal on digital input 0*/
   public static final int HMI_FLOWFLAG_ON_DIGI_IN1_LO_OUT7 =0x0008; /** trigger data output on OUT 7 in case of a signal on digital input 0*/
   public static final int HMI_FLOWFLAG_ON_CHANGING_OUT6    =0x0010; /** trigger data output on OUT 6 in case of value is changing actually (includes HMI_FLOWFLAG_ON_CHANGED); in case of sliders or permanently changing data sources this may cause massive data transmission*/
   public static final int HMI_FLOWFLAG_ON_CHANGED_OUT6     =0x0020; /** trigger data output on OUT 6 in case of value was changed */
   public static final int HMI_FLOWFLAG_ON_CHANGING_OUT7    =0x0040; /** trigger data output on OUT 7 in case of value is changing actually (includes HMI_FLOWFLAG_ON_CHANGED); in case of sliders or permanently changing data sources this may cause massive data transmission*/
   public static final int HMI_FLOWFLAG_ON_CHANGED_OUT7     =0x0080; /** trigger data output on OUT 7 in case of value was changed */

   public static final int HMI_NOFLOWFLAG_ON_DIGI_IN1 =0x0001; /** do not give option to react on digital input 1 */
   public static final int HMI_NOFLOWFLAG_ON_CHANGING =0x0002; /** do not give option to react on changing value */
   public static final int HMI_NOFLOWFLAG_ON_CHANGED  =0x0004; /** do not give option to react on changed value */
   public static final int HMI_NOFLOWFLAG_OUT6        =0x0008; /** do not give options for output 6*/

   View            uiElement;
   BasePanel       m_parent;
   boolean         returnOK;
   int             m_mutexID;
   
   LogDataCtrl     logDataCtrl;

   int             lastFont_pointSize,lastFont_style,lastFont_weight;
   String          lastFont_faceName;   
   
   hmiObject(BasePanel parent,flowObject obj)
   {
      super(obj);
	  returnOK=false;
	  m_parent=parent;
	  if (m_parent!=null) data.isChildOf=m_parent.m_myID; // no parent in case of silent load
	  uiElement=null;
	  lastFont_pointSize=-1;
      lastFont_style=-1;
	  lastFont_weight=-1;
	  lastFont_faceName="";
	  logDataCtrl=new LogDataCtrl(data.log);
	  returnOK=true;
	}
	
	
	
   void close()
   {
/* TODO:      hmiMutex *mutex;
	
	  mutex=g_objectList.getMutexByObjectID(this->data.id);
	  if (mutex) mutex->removeObjectID(this->data.id);*/
	  if (logDataCtrl!=null) logDataCtrl.close();
	  logDataCtrl=null;
/* TODO:     if ((uiElement) && (m_doDestroy))
	  {
         uiElement->Close();
         uiElement->Destroy();
      }*/
   }
	
	
	
   void setParent(BasePanel parent)
   {
      m_parent=parent;
      data.isChildOf=parent.m_myID;
   }

   
   
   void applyData(boolean all)
   {
/** custom background colour not supported in first versions -> damages button layout, no smooth possibility to set background
 *       uiElement.setBackgroundColor(wxConvert.fromWxColour(data.bgcolor[digi[0]]));
 */
	  super.applyData(all);
   }



   View createUIElement()
   {
      if (uiElement==null) return null;
      m_parent.absLayout.addView(uiElement,new AbsoluteLayout.LayoutParams(data.msizex/1000,data.msizey/1000,data.mposx/1000,data.mposy/1000));      
      return uiElement;
   }   
   

   
   void applyFont(View ui)
   {
/* custom fonts not supported in first versions
       lastFont_pointSize=data.font[digi[0]].pointSize;
      lastFont_style=data.font[digi[0]].style;
      lastFont_weight=data.font[digi[0]].weight;
      lastFont_faceName=data.font[digi[0]].fontFaceName;*/
   }
   
   

   long getAssignedOutput(long input)
   {
      if (input==OAPCIServer.OAPC_DIGI_IO0) return OAPCIServer.OAPC_DIGI_IO0;
      return 0;
   }

   
   
   void updateSelectionState()
   {
   }   

   
   
   void setSelected(boolean selected)
   {
   }
   
   
}
