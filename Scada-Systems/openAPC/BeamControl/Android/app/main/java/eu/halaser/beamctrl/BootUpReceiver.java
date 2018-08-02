package eu.halaser.beamctrl;

import android.content.*;

public class BootUpReceiver extends BroadcastReceiver
{

   @Override
   public void onReceive(Context context, Intent intent) 
   {            
      Intent i = new Intent(context, BeamCtrl.class);
      i.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
      i.putExtra("autostarted",true);
      context.startActivity(i);  
   }

}
