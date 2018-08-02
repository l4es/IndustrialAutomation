package eu.halaser.beamctrl;

import java.util.*;

import android.app.*;
import android.content.*;
import android.os.*;
import android.preference.*;


public class ScanService extends Service implements Runnable
{
   static boolean running = true;
   static Vector<ConnectionData> sync_connectionList;

   private HashMap<String, BeamConnection> useConnectionList = new HashMap<String, BeamConnection>();
   private Thread scanThread;
   private PowerManager.WakeLock wl = null;
   private PowerManager pm;
   private NotificationManager      notificationManager;
   private static SharedPreferences SP;

   @Override
   public IBinder onBind(Intent arg)
   {
      return null;
   }


   public void onCreate()
   {
      int flags, screenLightVal = 1;

      pm = (PowerManager) getSystemService(POWER_SERVICE);
      SP = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
      try
      {
         screenLightVal = Integer.parseInt(SP.getString("screenLight", "2"));
      }
      catch (NumberFormatException nfe)
      {
      }
      if (screenLightVal == 1) flags = PowerManager.PARTIAL_WAKE_LOCK;
      else if (screenLightVal == 3) flags = PowerManager.FULL_WAKE_LOCK;
      else flags = PowerManager.SCREEN_DIM_WAKE_LOCK;

      wl = pm.newWakeLock(flags, "BeamCtrl");
      wl.acquire();

      notificationManager = (NotificationManager)getSystemService(NOTIFICATION_SERVICE);
   }


   public void onDestroy()
   {
      running = false;

      if (scanThread != null) scanThread.interrupt();
      try
      {
         if (wl != null) wl.release();
      }
      catch (RuntimeException re)
      {
      }
      wl = null;
      if (notificationManager != null) notificationManager.cancel(0);
      try
      {
         if (scanThread != null)
            scanThread.join(1000);
      }
      catch (InterruptedException ie)
      {

      }
      System.exit(0);
   }


   public int onStartCommand(Intent intent, int flags, int startId)
   {
      scanThread = new Thread(this);
      scanThread.start();
      return START_STICKY;
   }


   static synchronized void setConnectionData(Vector<ConnectionData> in_connectionList)
   {
      sync_connectionList=in_connectionList;
   }


   public void setNotification(String text,String ID,String host)
   {
      if (BeamCtrl.appVisible) return;

      Notification notification=new Notification(R.drawable.ic_launcher,"",System.currentTimeMillis());
      notification.tickerText = ID+ " ("+host+"): "+text;
      Intent intent = new Intent(this, BeamCtrl.class);
      intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
      PendingIntent pendIntent = PendingIntent.getActivity(this, 0, intent, 0);
      notification.setLatestEventInfo(getBaseContext(),getResources().getText(R.string.app_name).toString(),notification.tickerText,pendIntent);

      notificationManager.notify(1810, notification);
   }

   public void run()
   {
      while (running)
      {
         try
         {
            int i;
            HashMap<String, BeamConnection> newConnectionList = new HashMap<String, BeamConnection>();

            if (sync_connectionList!=null)
             for (i=0; i<sync_connectionList.size(); i++)
            {
               BeamConnection connection = useConnectionList.get(sync_connectionList.elementAt(i).host);

               if (connection==null)
                  connection=new BeamConnection(sync_connectionList.elementAt(i),this);
               else
                  useConnectionList.remove(connection.data.host);

               newConnectionList.put(connection.data.host, connection);
            }
            if (!useConnectionList.isEmpty())
            {
               for (String key: useConnectionList.keySet())
               {
                  useConnectionList.get(key).running=false;
               }
            }
            useConnectionList.clear();
            useConnectionList = newConnectionList;
            Thread.sleep(1000);
         }
         catch (InterruptedException ie)
         {
         }
      }
   }
}
