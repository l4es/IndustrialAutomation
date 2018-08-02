import com.oapc.iface.*;

import java.io.*;

public class InterfaceExample1 
{
   private OAPCInterface   oapcInterface;
   private MyOAPCListener  oapcListener;
   
   class MyOAPCListener implements OAPCListener
   {
	   
      public void commandReceived(String cmd,String value,SocketEntry socket)
      {
         System.out.println("String received: "+cmd+"/"+value);
         if (cmd.equals("ECHO_STR")) oapcInterface.sendCommand("REPLY_STR",value,socket);    	  
      }
      
      
      
	  public void commandReceived(String cmd,float value,SocketEntry socket)
	  {
	     System.out.println("Number received: "+cmd+"/"+value);
         if (cmd.equals("ECHO_NUM")) oapcInterface.sendCommand("REPLY_NUM",value,socket);    	  		  
	  }
	  
	  
	  
	  public void commandReceived(String cmd,boolean value,SocketEntry socket)
	  {
         System.out.println("Digi received:   "+cmd+"/"+value);
	
         if (cmd.equals("ECHO_DIGI")) oapcInterface.sendCommand("REPLY_DIGI",value,socket);    	  
         else if ((cmd.equals("EXIT")) && (value)) System.exit(0);
	  }
	  
	  
      public void stateChanged(int state,String message,SocketEntry socket)
      {
         System.out.println("State change for client: "+state+" "+message);
      }

   }

   
   
   InterfaceExample1()
   {
	  try
	  {
         oapcInterface=new OAPCInterface();
         oapcListener=new MyOAPCListener();
         oapcInterface.addOAPCListener(oapcListener);
	  }
	  catch (IOException ioe)
	  {
	     ioe.printStackTrace();
	  }
   }
   
   public static void main(String[] args) 
   {
      new InterfaceExample1();
   }

}
