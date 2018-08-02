import com.oapc.iserver.*;

import java.io.*;

public class IServerExample1 
{
   private OAPCIServer    oapcIServer;
   private MyOAPCListener oapcListener;
   private boolean        go=false,doExit=false;
   
   class MyOAPCListener implements OAPCIServerListener
   {
	   public void dataReceived(String nodeName,int cmd,long ios,IServerData[] values)
	   {
	      if (nodeName.equals("/GOButton/out")) go=values[1].digi;
	      else if (nodeName.equals("/system/exit")) doExit=values[0].digi;

	      System.out.println(nodeName+" "+cmd);
	   }
   }


   
   IServerExample1()
   {
	  int         pos=0,delay2=70;
	  IServerData trueData=new IServerData(true),falseData=new IServerData(false);
	  
	  try
	  {
         oapcIServer=new OAPCIServer();
         oapcListener=new MyOAPCListener();
         oapcIServer.addOAPCIServerListener(oapcListener);
         while (!doExit)
         {
        	if (go)
        	{
               pos=pos+1;
               if (pos==1)
        	   {
        	      oapcIServer.setValue("/Line_1/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_10/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==2)
        	   {
        	      oapcIServer.setValue("/Line_2/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_1/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==3)
        	   {
        	      oapcIServer.setValue("/Line_3/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_2/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==4)
        	   {
        	      oapcIServer.setValue("/Line_4/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
                  oapcIServer.setValue("/Line_3/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==5)
        	   {
                  oapcIServer.setValue("/Line_5/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
                  oapcIServer.setValue("/Line_4/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==6)
        	   {
                  oapcIServer.setValue("/Line_6/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_5/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==7)
        	   {
        	      oapcIServer.setValue("/Line_7/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_6/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==8)
        	   {
        	      oapcIServer.setValue("/Line_8/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_7/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==9)
        	   {
        	      oapcIServer.setValue("/Line_9/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_8/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	   }
        	   else if (pos==10)
        	   {
                  oapcIServer.setValue("/Line_10/in",OAPCIServer.OAPC_DIGI_IO0,trueData);
        	      oapcIServer.setValue("/Line_9/in",OAPCIServer.OAPC_DIGI_IO0,falseData);
        	      pos=0;
        	   }
        	}        	 
            try
            {
               Thread.sleep(delay2);
            }
            catch (InterruptedException ie2)
            {
           	 
            }
         }
	  }
	  catch (IOException ioe)
	  {
	     ioe.printStackTrace();
	  }
	  oapcIServer.close();
	  System.out.println("done!");
   }
   
   public static void main(String[] args) 
   {
      new IServerExample1();
   }

}
