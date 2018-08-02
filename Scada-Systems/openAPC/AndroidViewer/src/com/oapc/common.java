package com.oapc;

import java.io.*;

class common
{
   static final int MAX_NAME_LENGTH=128;
   static final int MAX_LIBNAME_LENGTH=150;
   static final int MAX_PATH_LENGTH=500;
   static final int MAX_LOG_TYPES=4;

   static final int PROJECT_FLAG_ISPACE_SINGLE_LOCAL =0x0002; // one local single ispace server for in- and output data
   static final int PROJECT_FLAG_ISPACE_DOUBLE_LOCAL =0x0003; // separate local ispace servers for in- and output data and the possibility to have an external interlock for them
   static final int PROJECT_FLAG_ISPACE_SINGLE_REMOTE=0x0004; // one remote single ispace server for in- and output data
   static final int PROJECT_FLAG_ISPACE_DOUBLE_REMOTE=0x0005; // separate remote ispace servers for in- and output data and the possibility to have an external interlock for them
   static final int PROJECT_FLAG_ISPACE_RESERVED1    =0x0006; // reserved for other ISpace-related mutual excluding options
   static final int PROJECT_FLAG_ISPACE_RESERVED2    =0x0007; // reserved for other ISpace-related mutual excluding options
   static final int PROJECT_FLAG_ISPACE_RESERVED3    =0x0008; // reserved for other ISpace-related mutual excluding options

   static final int PROJECT_FLAG_ISPACE_MASK         =(PROJECT_FLAG_ISPACE_SINGLE_LOCAL|PROJECT_FLAG_ISPACE_DOUBLE_LOCAL|PROJECT_FLAG_ISPACE_SINGLE_REMOTE|PROJECT_FLAG_ISPACE_DOUBLE_REMOTE);
}


class hmiFlowData
{
    int version,id,length,usedFlows,/*maxEdges,*/flowFlags; // id - the identifier the hmiObject belongs to or the identifier that has to be assigned to the external flowObject
    
    int load(DataInputStream in)    
    {
       int loaded=0;
       
       try
       {
          version=in.readInt();   loaded+=4;
          id=in.readInt();        loaded+=4;
          length=in.readInt();    loaded+=4;  
    	  usedFlows=in.readInt(); loaded+=4;
    	  in.skip(4);             loaded+=4; // maxEdges
    	  flowFlags=in.readInt(); loaded+=4;
       }
       catch (IOException ioe)
       {    		
       }
       return loaded;
    }
};



class hmiObjectFontData
{
   int    pointSize,style,weight,reserved1;
   //char[common.MAX_NAME_LENGTH*2] store_faceName;
   String fontFaceName;
   
   int load(DataInputStream in)
   {
      int loaded=0;

      try
      {
    	 pointSize=IO.readInt(in); loaded+=4;
    	 style=IO.readInt(in);     loaded+=4;
    	 weight=IO.readInt(in);    loaded+=4;
         reserved1=IO.readInt(in); loaded+=4;
         fontFaceName=IO.readUTF16BE(in,common.MAX_NAME_LENGTH*2); loaded+=common.MAX_NAME_LENGTH*2;
      }
      catch (IOException ioe)
      {
    	  
      }
      return loaded;
   }
   
   
   void set(hmiObjectFontData data)
   {
      pointSize=data.pointSize;
      style=data.style;
      weight=data.weight;
      reserved1=data.reserved1;
      fontFaceName=data.fontFaceName;
   }
};



class hmiProjectData 
{
   int  version,reserved1;
   int  totalW,totalH;
   int  bgCol;
   int  flags;
   int  flowTimeout;    // timeout value in milliseconds after that a control flow thread has to die
   int  timerResolution; // resolution the timer polls outputs with
	    // new since 1.1
   int  touchFactor;
   hmiObjectFontData font;
	    // new since 1.3
//   char                     m_remSingleIP[19]; // xxx.xxx.xxx.xxx + 4 chars
   String m_remSingleIP;
	   // new since 1.4
//   char                     m_remDoubleIP[19]; // xxx.xxx.xxx.xxx + 4 chars
   String m_remDoubleIP;
   
   
   hmiProjectData(boolean deleteUserList)
   {
/* TODO:      if (deleteUserList)
      {
         if (g_userList) delete g_userList;
         g_userList=NULL;
      }*/
      flags=0;
      totalW=800;
      totalH=600;
      bgCol=0xFFFFFF;
      flowTimeout=1000;
      timerResolution=25;

      //new since 1.1
      touchFactor=1250;
      font=new hmiObjectFontData();
      font.pointSize=12;
      font.style=0;
      font.weight=0;
      font.fontFaceName="";
      //new since 1.3
      m_remSingleIP="192.168.1.1";
      //new since 1.4
      m_remDoubleIP="192.168.1.1";
      //end of new since

/* TODO:      if (deleteUserList)
      {
         g_userPriviData.enabled=false;
         g_userPriviData.privisEnabled=hmiUserMgmntPanel::priviFlagEnabled[1]| // shifted by 1, 0 is the supervision privilege which is non-editable
                                       hmiUserMgmntPanel::priviFlagEnabled[2]|
                                       hmiUserMgmntPanel::priviFlagEnabled[3]|
                                       hmiUserMgmntPanel::priviFlagEnabled[4]|
                                       hmiUserMgmntPanel::priviFlagEnabled[5]|
                                       hmiUserMgmntPanel::priviFlagEnabled[6]|
                                       hmiUserMgmntPanel::priviFlagEnabled[7]|
                                       hmiUserMgmntPanel::priviFlagEnabled[8];
         g_userPriviData.priviName[0]=_("Manage Users");
         g_userPriviData.priviName[1]=_("Exit Application");
         g_userPriviData.priviName[2]=_("Manual Control");
         g_userPriviData.priviName[3]=_("Modify Parameters");
         g_userPriviData.priviName[4]=_("Edit Recipe");
         g_userPriviData.priviName[5]=_("Select Recipe");
         g_userPriviData.priviName[6]=_("Start Process");
         g_userPriviData.priviName[7]=_("Stop Process");
         g_userPriviData.priviName[8]=_("unused");
         g_userPriviData.priviName[9]=_("unused");
         g_userPriviData.priviName[10]=_("unused");
         g_userPriviData.priviName[11]=_("unused");
         g_userPriviData.priviName[12]=_("unused");
         g_userPriviData.priviName[13]=_("unused");
         g_userPriviData.priviName[14]=_("unused");
      }
      g_plugInPanel->setEnabled((projectData->flags & PROJECT_FLAG_ISPACE_MASK)!=0);
   #endif*/
   }
   
}
