package com.oapc;

import java.io.*;
import java.util.*;

import android.os.Message;

import com.oapc.common;
import com.oapc.liboapc.*;
import com.oapc.iserver.*;
import com.oapc.jcommon.*;



class hmiObjectHead
{
   int version,size,reserved1,reserved2;
   
   int load(DataInputStream in)
   {
      int loaded=0;

      try
      {
         version=IO.readInt(in);   loaded+=4;
         size=IO.readInt(in);      loaded+=4;
         reserved1=IO.readInt(in); loaded+=4;
         reserved2=IO.readInt(in); loaded+=4;
      }
      catch (IOException ioe)
      {
    	  
      }
      return loaded;
   }
};



// flags:      
class hmiObjectLogData
{
   int mBelowVal,mAboveVal,mFromVal,mToVal;
   int flags,reserved1;
   
   int load(DataInputStream in)
   {
       int loaded=0;
       
       try
       {
    	  mBelowVal=IO.readInt(in); loaded+=4;
    	  mAboveVal=IO.readInt(in); loaded+=4;
    	  mFromVal=IO.readInt(in);  loaded+=4;
    	  mToVal=IO.readInt(in);    loaded+=4;
    	  flags=IO.readInt(in);     loaded+=4;
    	  reserved1=IO.readInt(in); loaded+=4;
       }
       catch (IOException ioe)
       {
    	   
       }
       return loaded;    	   
   }
   
   void set(hmiObjectLogData data)
   {
      mBelowVal=data.mBelowVal;
      mAboveVal=data.mAboveVal;
      mFromVal=data.mFromVal;
      mToVal=data.mToVal;
      flags=data.flags;
      reserved1=data.reserved1;
   }
};



class hmiObjectData
{
	int                    type,hmiFlags,logFlags,id; // align to 64 bit
    long                   stdIN,stdOUT,userPriviFlags,reserved;
    int                    dataFlowFlags,isChildOf;
    int                    mposx,mposy,msizex,msizey;
//    int                    flowposx,flowposy;
    int[]                  bgcolor={0,0},fgcolor={0,0},state={0,0};
    byte                   custom1;
    byte                   custom2;
    short                  custom3;
//    int                    mMaxValue_off;
    String                 name;        //store_name[MAX_NAME_LENGTH*2];
    String[]               text={"",""};// [2][MAX_TEXT_LENGTH*2];
    hmiObjectLogData[]     log=new hmiObjectLogData[common.MAX_LOG_TYPES];
    hmiObjectFontData[]    font=new hmiObjectFontData[2];
    oapc_num_value_block   MinValue=new oapc_num_value_block(),MaxValue=new oapc_num_value_block();
    
   int load(DataInputStream in,int loadSize)
   {
      int loaded=0,i;
       
      font[0]=new hmiObjectFontData();
      font[1]=new hmiObjectFontData();
      try
      {         
         type=IO.readInt(in);            loaded+=4;
         hmiFlags=IO.readInt(in);        loaded+=4;
         logFlags=IO.readInt(in);        loaded+=4;
         id=IO.readInt(in);              loaded+=4;
//         do not load these data, they depend on the code and therefore are static
//         data.stdIN         =oapc_util_ntohll(convData.stdIN);
//         data.stdOUT        =oapc_util_ntohll(convData.stdOUT);
         in.skip(16);                    loaded+=16;
         userPriviFlags=IO.readLong(in); loaded+=8;
         reserved=IO.readLong(in);       loaded+=8;
         dataFlowFlags=IO.readInt(in); loaded+=4;
         isChildOf=IO.readInt(in);     loaded+=4;
         mposx=IO.readInt(in);         loaded+=4;
         mposy=IO.readInt(in);         loaded+=4;
         msizex=IO.readInt(in);        loaded+=4;
         msizey=IO.readInt(in);        loaded+=4;
         in.skip(8);                   loaded+=8; // flowposX,flowposy
         bgcolor[0]=IO.readInt(in);    loaded+=4;
         bgcolor[1]=IO.readInt(in);    loaded+=4;
         fgcolor[0]=IO.readInt(in);    loaded+=4;
         fgcolor[1]=IO.readInt(in);    loaded+=4;
         state[0]=IO.readInt(in);      loaded+=4;
         state[1]=IO.readInt(in);      loaded+=4;
         custom1=in.readByte();        loaded+=1;
         custom2=in.readByte();        loaded+=1;
         custom3=in.readShort();       loaded+=2;
         in.skip(4);                   loaded+=4; //m_maxValueOff
         name=IO.readUTF16BE(in,common.MAX_NAME_LENGTH*2);    loaded+=common.MAX_NAME_LENGTH*2;
         text[0]=IO.readUTF16BE(in,OAPCIServer.MAX_TEXT_LENGTH*2); loaded+=OAPCIServer.MAX_TEXT_LENGTH*2;
         text[1]=IO.readUTF16BE(in,OAPCIServer.MAX_TEXT_LENGTH*2); loaded+=OAPCIServer.MAX_TEXT_LENGTH*2;
         for (i=0; i<common.MAX_LOG_TYPES; i++)
         {
            log[0]=new hmiObjectLogData();
            loaded+=log[i].load(in);
         }
         loaded+=font[0].load(in);
         loaded+=font[1].load(in);
         loaded+=MinValue.load(in);
         loaded+=MaxValue.load(in);             	   
      }
      catch (IOException ioe)
      { 	   
      }
      return loaded;
   }
    
    void set(hmiObjectData data)
    {
       int i;
       
       type=data.type;
       hmiFlags=data.hmiFlags;
       logFlags=data.logFlags;
       id=data.id;
       stdIN=data.stdIN;
       stdOUT=data.stdOUT;
       userPriviFlags=data.userPriviFlags;
       reserved=data.reserved;
       dataFlowFlags=data.dataFlowFlags;
       isChildOf=data.isChildOf;
       mposx=data.mposx;
       mposy=data.mposy;
       msizex=data.msizex;
       msizey=data.msizey;
       custom1=data.custom1;
       custom2=data.custom2;
       custom3=data.custom3;
       name=data.name;
       for (i=0; i<2; i++)
       {
    	  text[i]=data.text[i];
    	  font[i].set(data.font[i]);
       }
       for (i=0; i<common.MAX_LOG_TYPES; i++)
       {
          log[i].set(data.log[i]);
       }
       MinValue.set(data.MinValue);
       MaxValue.set(data.MaxValue);
    }
};



class flowObjectData
{
   hmiFlowData flowData;
   int         flowX,flowY;
   String      name; //  store_name[MAX_NAME_LENGTH*2];
};



public class flowObject 
{
   public static final int FLOWOBJECT_ACT_VERSION=2;
   
   public static final int MAX_LOG_TYPES=4;

   public static final int MAX_NUM_VALUE= 2100000000;
   public static final int MIN_NUM_VALUE=-2100000000;
	
   public static final int FLOW_TYPE_GROUP              =0x00000200;
   public static final int FLOW_TYPE_GROUP_IO_DEFINITION=0x00000300;
   public static final int FLOW_TYPE_CONV_DIGI2NUM      =0x00000400;
   public static final int FLOW_TYPE_CONV_DIGI2CHAR     =0x00000500;
   public static final int FLOW_TYPE_CONV_NUM2DIGI      =0x00000600;
   public static final int FLOW_TYPE_CONV_NUM2CHAR      =0x00000700;
   public static final int FLOW_TYPE_CONV_CHAR2DIGI     =0x00000800;
   public static final int FLOW_TYPE_CONV_CHAR2NUM      =0x00000900;
   public static final int FLOW_TYPE_CONV_MIXED2CHAR    =0x00000A00;
   public static final int FLOW_TYPE_CONV_NUM2BITS      =0x00000B00;
   public static final int FLOW_TYPE_LOGI_DIGINOT       =0x00000C00;
   public static final int FLOW_TYPE_LOGI_NUMNOT        =0x00000D00;
   public static final int FLOW_TYPE_LOGI_DIGIOR        =0x00000E00;
   public static final int FLOW_TYPE_LOGI_NUMOR         =0x00000F00;
   public static final int FLOW_TYPE_LOGI_DIGIAND       =0x00001000;
   public static final int FLOW_TYPE_LOGI_NUMAND        =0x00001100;
   public static final int FLOW_TYPE_LOGI_DIGIXOR       =0x00001200;
   public static final int FLOW_TYPE_LOGI_NUMXOR        =0x00001300;
   public static final int FLOW_TYPE_LOGI_NUMADD        =0x00001400;
   public static final int FLOW_TYPE_LOGI_NUMSUB        =0x00001500;
   public static final int FLOW_TYPE_LOGI_NUMMUL        =0x00001600;
   public static final int FLOW_TYPE_LOGI_NUMDIV        =0x00001700;
   public static final int FLOW_TYPE_FLOW_START         =0x00001800;
   public static final int FLOW_TYPE_FLOW_STOP          =0x00001900;
   public static final int FLOW_TYPE_FLOW_TIMER         =0x00001A00;
   public static final int FLOW_TYPE_FLOW_DELAY         =0x00001B00;
   public static final int FLOW_TYPE_MISC_LOG           =0x00001C00;
   public static final int FLOW_TYPE_MISC_LOG_REC       =0x00001D00;
   public static final int FLOW_TYPE_CONV_DIGI2PAIR     =0x00001E00;
   public static final int FLOW_TYPE_CONV_NUM2PAIR      =0x00001F00;
   public static final int FLOW_TYPE_CONV_CHAR2PAIR     =0x00002000;
   public static final int FLOW_TYPE_CONV_BIN2PAIR      =0x00002100;
   public static final int FLOW_TYPE_FLOW_NUMTGATE      =0x00002200;
   public static final int FLOW_TYPE_FLOW_CHARTGATE     =0x00002300;
   public static final int FLOW_TYPE_FLOW_BINTGATE      =0x00002400;
   public static final int FLOW_TYPE_FLOW_DIGITGATE     =0x00002500;
   public static final int FLOW_TYPE_CONV_PAIR2CHAR     =0x00002600;
   public static final int FLOW_TYPE_CONV_PAIR2DIGI     =0x00002700;
   public static final int FLOW_TYPE_CONV_PAIR2NUM      =0x00002800;
   public static final int FLOW_TYPE_CONV_PAIR2BIN      =0x00002900;
   public static final int FLOW_TYPE_LOGI_DIGINOP       =0x00002A00;
   public static final int FLOW_TYPE_LOGI_NUMNOP        =0x00002B00;
   public static final int FLOW_TYPE_LOGI_CHARNOP       =0x00002C00;
   public static final int FLOW_TYPE_LOGI_BINNOP        =0x00002D00;
   public static final int FLOW_TYPE_LOGI_DIGISHIFT     =0x00002E00;
   public static final int FLOW_TYPE_LOGI_NUMSHIFT      =0x00002F00;
   public static final int FLOW_TYPE_LOGI_CHARSHIFT     =0x00003000;
   public static final int FLOW_TYPE_LOGI_BINSHIFT      =0x00003100;
   public static final int FLOW_TYPE_FLOW_NUMGATE       =0x00003200;
   public static final int FLOW_TYPE_FLOW_CHARGATE      =0x00003300;
   public static final int FLOW_TYPE_FLOW_BINGATE       =0x00003400;
   public static final int FLOW_TYPE_FLOW_DIGIGATE      =0x00003500;
   public static final int FLOW_TYPE_LOGI_DIGIRSFF      =0x00003600;
   public static final int FLOW_TYPE_MATH_NUMCMP        =0x00003700;
   public static final int FLOW_TYPE_MATH_DIGICMP       =0x00003800;
   public static final int FLOW_TYPE_MATH_CHARCMP       =0x00003900;
   public static final int FLOW_TYPE_MATH_DIGICTR       =0x00003A00;
   public static final int FLOW_TYPE_MATH_NUMCTR        =0x00003B00;
   public static final int FLOW_TYPE_MATH_CHARCTR       =0x00003C00;
   public static final int FLOW_TYPE_MATH_BINCTR        =0x00003D00;
   public static final int FLOW_TYPE_LOGI_DIGITFF       =0x00003E00;
   public static final int FLOW_TYPE_MISC_LOGIN         =0x00003F00;
   public static final int FLOW_TYPE_MISC_ISCONNECT     =0x00004000;

   public static final long FLOW_TYPE_FLAG_DIGI_OVERFLOW_OUT=0x0000000100000000L;
   public static final long FLOW_TYPE_FLAG_NUM_OVERFLOW_OUT =0x0000000200000000L;
   public static final long FLOW_TYPE_FLAG_CHAR_OVERFLOW_OUT=0x0000000400000000L;
   public static final long FLOW_TYPE_FLAG_BIN_OVERFLOW_OUT =0x0000000800000000L;

   public static final int FLAG_MAP_OUTPUT_TO_ISERVER   =0x00000001; // send changed output values to iserver
   public static final int FLAG_ALLOW_MOD_FROM_ISERVER  =0x00000002; // accept new values from iserver for modifying in/outputs of this object
   
   
   public static final int OBJECT_MODE_FLOWELEMENT=1;
   public static final int OBJECT_MODE_DEVICE     =2;

   public static final int FLOW_TYPE_MASK=0x0000FF00;

   
   
   long                             lastPollTime;
   boolean[]                        dataValid=new boolean[OAPCIServer.MAX_NUM_IOS+1]; // used during flow processing to read an output only
   String[]                         dataTxt=new String[OAPCIServer.MAX_NUM_IOS];   // once also if there are several connections
   double[]                         dataNum=new double[OAPCIServer.MAX_NUM_IOS];   // outgoing from this output
   byte[]                           dataDigi=new byte[OAPCIServer.MAX_NUM_IOS];
   oapc_bin_head[]                  dataBin=new oapc_bin_head[OAPCIServer.MAX_NUM_IOS];
   boolean                          m_doDestroy;

   hmiObjectData                    data=new hmiObjectData();
   double                           MinValue,MaxValue;
   int                              m_cycleMicros,m_flowFlags;    // used by external IO lib objects only
   String                           nodeNameIn,nodeNameOut;

   byte                    digiBits;
   byte[]                  digi=new byte[OAPCIServer.MAX_NUM_IOS];
   Vector<FlowConnection>  flowList=new Vector<FlowConnection>();
   int                     m_mode;

   private flowObjectData  convData;
   private int             lastFlowThreadID;
   
   
   
   flowObject(flowObject obj)
   {
      int i;

      m_doDestroy=true;
      for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++) digi[i]=0;
      lastPollTime=0;
      lastFlowThreadID=0;
      digiBits=0;
      for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++) dataValid[i]=false;
      m_flowFlags=0;
      if (obj!=null)
      {
         data.set(obj.data);
         data.id=0; // do not use the ID of the copied object because they have to be unique (ID is set by ObjectList::Append())
//         fontFaceName[0]=obj.fontFaceName[0];
//         fontFaceName[1]=obj.fontFaceName[1];
         data.text[0]=obj.data.text[0];
         data.text[1]=obj.data.text[1];
      }
      else
      {
         data.type=0;
         m_flowFlags=0; //FLAG_MAP_OUTPUT_TO_ISERVER|FLAG_ALLOW_MOD_FROM_ISERVER;
         data.logFlags=0;
         data.isChildOf=0;
         data.stdIN=OAPCIServer.OAPC_DIGI_IO0;
         data.stdOUT=OAPCIServer.OAPC_DIGI_IO0;
//TODO:         data.userPriviFlags=(0xFFFFFFFFFFFFFFFF & ~hmiUserMgmntPanel::priviFlagMask[0]) | hmiUserMgmntPanel::priviFlagEnabled[0];
         data.dataFlowFlags=hmiObject.HMI_FLOWFLAG_ON_CHANGED_OUT7;
         data.id=0;
//         data.flowposx=-1;         data.flowposy=-1;
         data.bgcolor[0]=0xDDDDDD; data.bgcolor[1]=0xBBBBBB;
         data.fgcolor[0]=0x000000; data.fgcolor[1]=0x222222;
         data.state[0]=0;          data.state[1]=0;

/*         wxWindow win;
         wxFont   font;

         font=win.GetFont();
         for (i=0; i<2; i++)
         {
            this->data.font[i].pointSize=font.GetPointSize();
            this->data.font[i].style=font.GetStyle();
            this->data.font[i].weight=font.GetWeight();
            fontFaceName[i]=_T("");
         }*/
         for (i=0; i<MAX_LOG_TYPES; i++)
         {
            data.log[i]=new hmiObjectLogData();
            data.log[i].mBelowVal=0;
            data.log[i].mAboveVal=0;
            data.log[i].mFromVal=0;
            data.log[i].mToVal=0;
            data.log[i].flags=0;
         }
         MinValue=0;
         MaxValue=100;
      }
   }



   void close()
   {
/* TODO:      wxNode *node;

      node=flowList.GetFirst();
      while (node)
      {
         if (node->GetData()) delete node->GetData();
         node=node->GetNext();
      }
   
      if (nodeNameIn) free(nodeNameIn);
      if (nodeNameOut) free(nodeNameOut);
   */
   }

   
   
   int load(DataInputStream FHandle,int chunkSize,int IDOffset)
   {
      hmiObjectHead convHead=new hmiObjectHead();
      int           i;

      if (FHandle==null) return 0;
      chunkSize=convHead.load(FHandle);

      if (convHead.version!=FLOWOBJECT_ACT_VERSION)
      {
    	 MainWin.g_fileVersionError();
         return chunkSize;
      }

      chunkSize+=data.load(FHandle,convHead.size);
      
      if (IDOffset!=0) data.id=data.id-IDOffset+MainWin.g_objectList.currentUniqueID()+1;

      MinValue=data.MinValue.toDouble();
      MaxValue=data.MaxValue.toDouble();
//      createNodeNames();
      return chunkSize;
   }

   
   int loadFlow(DataInputStream FHandle,hmiFlowData flowData,int IDOffset,boolean usesOwnHead)
   {
      int            length=0;
      FlowConnection flowCon;
      int            i;
      Bool           successful=new Bool(false);
      hmiObjectHead  convHead=new hmiObjectHead();

      assert(FHandle!=null);
      if (FHandle==null) return 0;

      if (usesOwnHead)
      {
    	   length=convHead.load(FHandle);
      }

      for (i=0; i<flowData.usedFlows; i++)
      {
         flowCon=new FlowConnection();
         length+=flowCon.loadFlow(FHandle,flowData,IDOffset,successful);
         if (successful.val) addFlowConnection(flowCon);
         flowCon.close();
      }
      return length;
   }

   
   
   void applyData(boolean all)
   {
   }
   
   
   
   long getAssignedOutput(long input)
   {
      return 0;
   }



   /**
   Sets an digital input and checks if the same thread has set that input before
   @param[in] input the flag that specifies the input
   @param[in] value the value that has to be set for that input (0 or 1)
   @param[in] flowThreadID the Id of the thread that wants to set this value
   @return 0 if the same thread already has set a value; the thread has to finish
           in this case to avoid endless loops; 1 if the input value was set correctly
   */
   boolean setDigitalInput(FlowConnection connection,byte value,Int flowThreadID,flowObject object)
   {
      int   bit=0;

      if (!threadIDOK(flowThreadID,false)) return false;
      switch (connection.targetInputNum)
      {
         case 0:
            bit=1;
            break;
         case 1:
            bit=2;
            break;
         case 2:
            bit=4;
            break;
         case 3:
            bit=8;
            break;
         case 4:
            bit=16;
            break;
         case 5:
            bit=32;
            break;
         case 6:
            bit=64;
            break;
         case 7:
            bit=128;
            break;
         default:
            assert(false);
            return false;
      }
      if (value!=0)
      {
         digi[connection.targetInputNum]=1;
         digiBits|=bit;
      }
      else
      {
         digi[connection.targetInputNum]=0;
         digiBits&=~bit;
      }
      applyData(false);
      return true;
   }



   byte getDigitalOutput(FlowConnection connection,Int rcode,long lastInput,long origCreationTime)
   {
      rcode.val=jliboapc.OAPC_OK;
      return digi[connection.sourceOutputNum];
   }



   boolean valueHasChanged()
   {
   	  return false;
   }


   /**
   Sets an numerical input
   @param[in] input the flag that specifies the input
   @param[in] value the value that has to be set for that input
   @param[in] flowThreadID the Id of the thread that wants to set this value
   @return 0; this method has to be overriden
   */
   boolean setNumInput(FlowConnection connection,double value,Int flowThreadID,flowObject object)
   {
      assert(false);
      return false;
   }



   double getNumOutput(FlowConnection connection,Int rcode,long lastInput,long origCreationTime)
   {
      assert(false);
      rcode.val=jliboapc.OAPC_ERROR_NO_SUCH_IO;
      return 0.0;
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
      assert(false);
      return false;
   }



   String getCharOutput(FlowConnection connection,Int rcode,long lastInput)
   {
      assert(false);
      rcode.val=jliboapc.OAPC_ERROR_NO_SUCH_IO;
      return "X-(";
   }



   boolean setBinInput(FlowConnection connection,oapc_bin_head value,Int flowThreadID,flowObject object)
   {
      assert(false);
      return false;
   }



   void releaseBinData(FlowConnection connection)
   {
      assert(false);
   }



   oapc_bin_head getBinOutput(FlowConnection connection,Int rcode,long lastInput)
   {
      assert(false);
      return null;
   }   
   
   
   
   boolean threadIDOK(Int flowThreadID,boolean allowSameID)
   {
      if (lastFlowThreadID==flowThreadID.val)
      {
         if (allowSameID)
         {
            flowThreadID.val=MainWin.g_flowPool.getFlowThreadID();
            lastFlowThreadID=flowThreadID.val;
            return true;
         }
         return false;
      }
      lastFlowThreadID=flowThreadID.val;
      return true;
   }

   
   
   void deleteFlowConnection(FlowConnection connection)
   {
       flowList.removeElement(connection);
       connection.close();
   }
   
   
   
   private static int getFlowConnection_ctr;
   
   FlowConnection getFlowConnection(FlowConnection connection)
   {
	  try
	  {
         if (connection==null) getFlowConnection_ctr=0;
         else getFlowConnection_ctr++;
         return flowList.elementAt(getFlowConnection_ctr);
	  }
	  catch (IndexOutOfBoundsException ioobe)
	  {		  
	  }
	  return null;
   }

   
   
   void addFlowConnection(FlowConnection connection)
   {
       flowList.add(connection);
   }

   
   
   int getMode()
   {
      return m_mode;
   }

   
   void sendDigitalInput(FlowConnection connection,byte value,Int flowThreadID,flowObject object)
   {
      Message          msg=new Message();
      hmiObjectMessage objMsg=new hmiObjectMessage(this,flowThreadID,connection,object);

      objMsg.digiValue=value;
      msg.what=MainWin.UIHandler.MSG_TOGGLE_FLOW_IO;
      msg.arg1=hmiObjectMessage.DIGI_MESSAGE;
      msg.obj=objMsg;
      MainWin.g_uiHandler.sendMessage(msg);
   }
   
      
}


