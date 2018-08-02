package com.oapc;

import android.graphics.*;

import com.oapc.iserver.*;
import com.oapc.jcommon.*;

import java.io.*;



class flowConnectionData
{
   public static final int MAX_CONNECTION_EDGES=32;
   
//   int   usedEdges; // align to 64 bit with next line, unused in compiled structure
   int   targetID;  // aligned to 64 bit
   long  sourceOutput,targetInput;

   int load(DataInputStream in)
   {
      int loaded=0;
      
      try
      {
         in.skip(4);                 loaded+=4; // usedEdges
         targetID=in.readInt();      loaded+=4;
         sourceOutput=in.readLong(); loaded+=8;
         targetInput=in.readLong();  loaded+=8;
      }
      catch (IOException ioe)
      {
    	  
      }
      return loaded;
   }
};

public class FlowConnection 
{
   flowConnectionData data=new flowConnectionData();
   int                sourceOutputNum,targetInputNum;
   boolean            isTempConnection;



   FlowConnection()
   {
      init();
      sourceOutputNum=-2000000;
      targetInputNum=-2000000;
   }



	private void init()
	{
	   isTempConnection=false;
	   data.sourceOutput=0;
	   data.targetID=0;
	   data.targetInput=0;
	}


	void close()
	{

	}


	int getDigiIndexFromFlag(long flag)
	{
	   int i;

	   if ((flag & OAPCIServer.OAPC_DIGI_IO_MASK)!=0) flag=(flag & OAPCIServer.OAPC_DIGI_IO_MASK);
	   for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	   {
	       if (flag==1) return i;
	       flag=flag>>1;
	   }
	   assert(false);
	   return 0;
	}



	int getNumIndexFromFlag(long flag)
	{
	   int i;

	   flag=(flag & OAPCIServer.OAPC_NUM_IO_MASK)>>8;
	   for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	   {
	       if (flag==1) return i;
	       flag=flag>>1;
	   }
	   assert(false);
	   return 0;
	}



	int getCharIndexFromFlag(long flag)
	{
	   int i;

	   flag=(flag & OAPCIServer.OAPC_CHAR_IO_MASK)>>16;
	   for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	   {
	       if (flag==1) return i;
	       flag=flag>>1;
	   }
	   assert(false);
	   return 0;
	}



	int getBinIndexFromFlag(long flag)
	{
	   int i;

	   flag=(flag & OAPCIServer.OAPC_BIN_IO_MASK)>>24;
	   for (i=0; i<OAPCIServer.MAX_NUM_IOS; i++)
	   {
	       if (flag==1) return i;
	       flag=flag>>1;
	   }
	   assert(false);
	   return 0;
	}



	void setFlowIONumbers()
	{
	   if ((data.sourceOutput & OAPCIServer.OAPC_DIGI_IO_MASK)!=0)      sourceOutputNum=getDigiIndexFromFlag(data.sourceOutput);
	   else if ((data.sourceOutput & OAPCIServer.OAPC_NUM_IO_MASK)!=0)  sourceOutputNum=getNumIndexFromFlag(data.sourceOutput);
	   else if ((data.sourceOutput & OAPCIServer.OAPC_CHAR_IO_MASK)!=0) sourceOutputNum=getCharIndexFromFlag(data.sourceOutput);
	   else if ((data.sourceOutput & OAPCIServer.OAPC_BIN_IO_MASK)!=0)  sourceOutputNum=getBinIndexFromFlag(data.sourceOutput);
	   else sourceOutputNum=OAPCIServer.MAX_NUM_IOS; // overflow outputs
	   assert(sourceOutputNum<OAPCIServer.MAX_NUM_IOS+1);

	   if ((data.targetInput & OAPCIServer.OAPC_DIGI_IO_MASK)!=0)      targetInputNum=getDigiIndexFromFlag(data.targetInput);
	   else if ((data.targetInput & OAPCIServer.OAPC_NUM_IO_MASK)!=0)  targetInputNum=getNumIndexFromFlag(data.targetInput);
	   else if ((data.targetInput & OAPCIServer.OAPC_CHAR_IO_MASK)!=0) targetInputNum=getCharIndexFromFlag(data.targetInput);
	   else if ((data.targetInput & OAPCIServer.OAPC_BIN_IO_MASK)!=0)  targetInputNum=getBinIndexFromFlag(data.targetInput);
	   else targetInputNum=OAPCIServer.MAX_NUM_IOS; // overflow outputs
	   assert(targetInputNum<OAPCIServer.MAX_NUM_IOS+1);
	}



   int loadFlow(DataInputStream FHandle,hmiFlowData flowData,int IDOffset,Bool successful) // later used with other versions of flowdata
	{
	   int length;

	   successful.val=false;
	   assert(FHandle!=null);
	   if (FHandle==null) return 0;

	   length=data.load(FHandle);
	   if (IDOffset!=0) data.targetID=data.targetID-IDOffset+MainWin.g_objectList.currentUniqueID()+1;
	   assert(data.targetInput!=0);
      if (data.targetID<=0) successful.val=false;
      else successful.val=true;
      setFlowIONumbers();
      return length;
   }

}
