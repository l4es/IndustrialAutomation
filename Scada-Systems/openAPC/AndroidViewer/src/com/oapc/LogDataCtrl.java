package com.oapc;

public class LogDataCtrl 
{
   private hmiObjectLogData[] data=new hmiObjectLogData[common.MAX_LOG_TYPES];
   private boolean            prevDigiVal;
   private double             prevNumVal;
   private String             prevCharVal;
   private boolean            digiValid,numValid,charValid;

   
   
   LogDataCtrl(hmiObjectLogData[] logData)
   {
      int i;

      for (i=0; i<common.MAX_LOG_TYPES; i++)
       data[i]=logData[i];
      digiValid=false;
      numValid=false;
      charValid=false;
   }
   
   
   
   void close()
   {
	   
   }
   
   
   
   void logDigi(byte val,flowObject object)
   {
      int i;

/*      if (g_flowLog==null) return;
      for (i=0; i<common.MAX_LOG_TYPES; i++)
      {
         if ((!digiValid) || (prevDigiVal!=val))
         {
            if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
             g_flowLog->setLogInfo(_("Value changed"),i,object);
         }
         if ((val==1) && (data[i].flags & LOG_FLAG_ON_DIGI_VALUE_HI))
          g_flowLog->setLogInfo(_("Value set to HIGH"),i,object);
         if ((val==0) && (data[i].flags & LOG_FLAG_ON_DIGI_VALUE_LO))
          g_flowLog->setLogInfo(_("Value set to LOW"),i,object);
      }
      prevDigiVal=val;
      digiValid=true;*/
   }



   void logNum(double val,flowObject object)
   {
/*      wxInt32 i;
      bool    allowNormalRangeMessage,doNormalRangeMessage;

      if (!g_flowLog) return;
      for (i=0; i<MAX_LOG_TYPES; i++)
      {
         allowNormalRangeMessage=true;
         doNormalRangeMessage=false;
         if ((!numValid) || (prevNumVal!=val))
         {
            if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
             g_flowLog->setLogInfo(_("Value changed"),i,object);
         }

         if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_BELOW)
         {
            if ((val<data[i].mBelowVal/1000.0) && ((!numValid) || (prevNumVal>=data[i].mBelowVal/1000.0)))
            {
               g_flowLog->setLogInfo(_("Value smaller than limit"),i,object);
               allowNormalRangeMessage=false;
            }
            else if ((val>data[i].mBelowVal/1000.0) && (prevNumVal<=data[i].mBelowVal/1000.0))
             doNormalRangeMessage=true;
         }

         if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_BETWEEN)
         {
            if ((val>=data[i].mFromVal/1000.0) && (val<=data[i].mToVal/1000.0) && 
                ((!numValid) || (prevNumVal<data[i].mFromVal/1000.0) || (prevNumVal>data[i].mToVal/1000.0)))
            {
               g_flowLog->setLogInfo(_("Value in limited range"),i,object);
               allowNormalRangeMessage=false;
            }
            else if (((val<data[i].mFromVal/1000.0) || (val>data[i].mToVal/1000.0)) && 
                ((prevNumVal>=data[i].mFromVal/1000.0) && (prevNumVal<=data[i].mToVal/1000.0)))
             doNormalRangeMessage=true;
         }
         if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_ABOVE)
         {
            if ((val>data[i].mAboveVal/1000.0) && ((!numValid) || (prevNumVal<=data[i].mAboveVal/1000.0)))
            {
               allowNormalRangeMessage=false;
            }
            else if ((val<data[i].mAboveVal/1000.0) && (prevNumVal>=data[i].mAboveVal/1000.0))
             doNormalRangeMessage=true;
         }
         if ((doNormalRangeMessage) && (allowNormalRangeMessage)) g_flowLog->setLogInfo(_("Value in normal range"),i,object);

         if (data[i].flags & LOG_FLAG_ON_NUM_VALUE_ABOVE)
         {
            if ((val>data[i].mAboveVal/1000.0) && ((!numValid) || (prevNumVal<=data[i].mAboveVal/1000.0)))
            {
               g_flowLog->setLogInfo(_("Value greater than limit"),i,object);
               allowNormalRangeMessage=false;
            }
         }
      }
      prevNumVal=val;
      numValid=true;*/
   }



   void logChar(String val,flowObject object)
   {
/*      wxInt32 i;

      if (!g_flowLog) return;
      for (i=0; i<MAX_LOG_TYPES; i++)
      {
         if ((!charValid) || (prevCharVal!=val))
         {
            if (data[i].flags & LOG_FLAG_ON_VALUE_CHANGED)
             g_flowLog->setLogInfo(_("Value changed"),i,object);
         }
      }
      prevCharVal=val;
      charValid=true;*/
   }
   
}
