package eu.halaser.beamctrl;

import android.content.*;

import java.io.*;
import java.util.Vector;

import android.app.*; 
import android.os.*;
import android.view.*;
import android.view.View.*;
import android.widget.*;
import android.preference.*;


public class BeamCtrl extends Activity implements OnClickListener
{
   public static final int ALERT_OK       =0x0001;
   public static final int ALERT_NO_EXIT  =0x0002;
   public static final int ALERT_NO_DELETE=0x0003;

   private PowerManager             pm=null;
   private PowerManager.WakeLock    wl=null;
   private ImageButton              addConnectionButton;
   private static SharedPreferences SP;
   private LinearLayout             rootLayout;
   private ScrollView               mainScrollView,detailScrollView;
   private GridLayout               detailLayout;
           MsgHandler               msgHandler=null;

   static boolean                   appVisible=false;
   static int                       textSizeVal=1;

   static private Vector<ConnectionData> connectionList=new Vector<ConnectionData>();
   static private ConnectionData         viewConnection=null;

   static class MsgHandler extends Handler
   {
      static final int MSG_UPDATE_CONNECTION_UI = 1;

      public void handleMessage(Message msg)
      {

         super.handleMessage(msg);

         switch (msg.what)
         {
            case MSG_UPDATE_CONNECTION_UI:
               ((ConnectionData)msg.obj).doUpdateUI();
               break;
         }
      }
   }


   void sendMessage(int what,int arg1,int arg2,Object obj)
   {
      Message msg=new Message();
      msg.what=what;
      msg.arg1=arg1;
      msg.arg2=arg2;
      msg.obj=obj;
      msgHandler.sendMessage(msg);
   }


   void simpleAlert(String text,String title,int mode)
   {
      AlertDialog ad=null;
      final BeamCtrl    ctx=this;
      
      try
      {
         ad = new AlertDialog.Builder(ctx).create();
         if (mode!=ALERT_OK)
         {
            ad.setCancelable(false);  
            ad.setCanceledOnTouchOutside(false);
         }
         if (text!=null) ad.setMessage(text);
         else ad.setMessage("missing ressource!");
         if (title!=null) ad.setTitle(title);
         if (mode==ALERT_OK)
         {
            ad.setButton(ctx.getResources().getText(R.string.ok), new DialogInterface.OnClickListener() {
                @Override  
                public void onClick(DialogInterface dialog, int which) {  
                    dialog.dismiss();
                }  
            });  
         }
         else if (mode==ALERT_NO_EXIT)
         {
            ad.setButton(ctx.getResources().getText(R.string.exit), new DialogInterface.OnClickListener() {  
                @Override  
                public void onClick(DialogInterface dialog, int which) 
                {                   
                   ScanService.running=false;
                   stopService(new Intent(ctx,ScanService.class));
                   dialog.dismiss();
                   finish();
//                   System.exit(0); this is done in service
                }  
            });  
            ad.setButton2(ctx.getResources().getText(R.string.no), new DialogInterface.OnClickListener() {  
                @Override  
                public void onClick(DialogInterface dialog, int which) {  
                    dialog.dismiss();
                }  
            });  
         }
         else if (mode==ALERT_NO_DELETE)
         {
            ad.setButton(ctx.getResources().getText(R.string.delete), new DialogInterface.OnClickListener() {
               @Override
               public void onClick(DialogInterface dialog, int which)
               {
                  connectionList.remove(viewConnection);
                  showDetailView(null);
                  saveConnections();
                  createUI();
                  ScanService.setConnectionData(connectionList);
               }
            });
            ad.setButton2(ctx.getResources().getText(R.string.no), new DialogInterface.OnClickListener() {
               @Override
               public void onClick(DialogInterface dialog, int which) {
                  dialog.dismiss();
               }
            });
         }
         ad.show();
      }
      catch (Exception e) // to avoid strange exceptions when app is in background
      {
         if (ad!=null) ad.dismiss();
      }
   }


   protected void saveConnections()
   {
      int i;

      if (connectionList.size()<=0) return;
      try
      {
         DataOutputStream outputStream = new DataOutputStream(openFileOutput("config.bin",Context.MODE_PRIVATE));
         for (i=0; i<connectionList.size(); i++)
            connectionList.elementAt(i).writeData(outputStream);
         outputStream.close();
      }
      catch (IOException e)
      {

      }
   }


   private Vector<ConnectionData> loadConnections(Vector<ConnectionData> old_connections)
   {
      Vector<ConnectionData> new_connections=new Vector<ConnectionData>();
      try
      {
         DataInputStream din = new DataInputStream(new BufferedInputStream(openFileInput("config.bin")));

         for (; ; )
         {
            ConnectionData data=new ConnectionData(this);

            if (data.readData(din))
            {
               int     i;
               boolean foundData=false;

               for (i=0; i<old_connections.size(); i++)
               {
                  if (old_connections.elementAt(i).host.equalsIgnoreCase(data.host))
                  {
                     new_connections.addElement(old_connections.elementAt(i));
                     foundData=true;
                     break;
                  }
               }
               if (!foundData)
                new_connections.add(data);
            }
            else break;
         }
         try
         {
            din.close();
         }
         catch (IOException ioe)
         {
         };
      }
      catch (FileNotFoundException fnfe)
      {

      };
      return new_connections;
   }


   public void onSaveInstanceState (Bundle outState)
   {
      outState.putBoolean("init",true);
   }



   private void createUI()
   {
      int i;

      mainScrollView=(ScrollView)findViewById(R.id.mainScrollView);
      detailScrollView=(ScrollView)findViewById(R.id.detailScrollView);
      rootLayout=(LinearLayout)findViewById(R.id.rootLayout);
      detailLayout=(GridLayout)findViewById(R.id.detailLayout);
      detailLayout.setVisibility(View.INVISIBLE);

      rootLayout.removeAllViews();
      connectionList=loadConnections(connectionList);
      for (i=0; i<connectionList.size(); i++)
      {
         connectionList.elementAt(i).initUI();
         connectionList.elementAt(i).setUI(rootLayout);
      }

      addConnectionButton= new ImageButton(this);
      addConnectionButton.setImageResource(android.R.drawable.ic_input_add);
      rootLayout.addView(addConnectionButton);
      addConnectionButton.setOnClickListener(this);
   }

   
   
   private void createService(Bundle savedInstanceState)
   {
      if ((savedInstanceState==null) || (!savedInstanceState.getBoolean("init")))
      {   
         startService(new Intent(this,ScanService.class));
      }
   }
   
   
   /** Called when the activity is first created. */
   @Override
   public void onCreate(Bundle savedInstanceState) 
   {
      int themeVal=1;

      super.onCreate(savedInstanceState);

      SP = PreferenceManager.getDefaultSharedPreferences(getBaseContext());
      try
      {
         themeVal=Integer.parseInt(SP.getString("theme","1"));
      }
      catch (NumberFormatException nfe)
      {
      }
      if (themeVal==2) setTheme(R.style.LightTheme);
      else if (themeVal==3) setTheme(R.style.DarkTheme);

      try
      {
         textSizeVal=Integer.parseInt(SP.getString("textSize","1"));
      }
      catch (NumberFormatException nfe)
      {
      }

      pm = (PowerManager) getSystemService(POWER_SERVICE);
      wl=pm.newWakeLock(PowerManager.FULL_WAKE_LOCK,"BeamCtrlMain");
      setContentView(R.layout.main);
      msgHandler=new MsgHandler();
      createUI();
      createService(savedInstanceState);
      invalidateOptionsMenu();
   }


   public boolean onCreateOptionsMenu(Menu pMenu)
   {
      pMenu.clear();

      MenuItem prefsMenuItem = pMenu.add(0,2, Menu.NONE,R.string.prefs);
      prefsMenuItem.setIcon(android.R.drawable.ic_menu_preferences);

      prefsMenuItem = pMenu.add(0,3, Menu.NONE,R.string.about);
      prefsMenuItem.setIcon(android.R.drawable.ic_menu_info_details);

      prefsMenuItem = pMenu.add(0, 1, Menu.NONE,R.string.exit);
      prefsMenuItem.setIcon(android.R.drawable.ic_menu_close_clear_cancel);

      return true;
   }

   public boolean onPrepareOptionsMenu(Menu pMenu)
   {
      return true;
   }

   
   
   public boolean onMenuItemSelected(int featureId, MenuItem item)
   {
      switch (item.getItemId())
      {
         case 1:
            simpleAlert(getResources().getText(R.string.really_exit_app).toString(),null,ALERT_NO_EXIT);
            break;
         case 2:
            Intent intent = new Intent(this,eu.halaser.beamctrl.GlobalPrefs.class);
            startActivity(intent);
            break;
         case 3:
            simpleAlert(getResources().getText(R.string.app_name).toString()+ "\nV 0.9BETA\n(c) 2017 by HALaser Systems\nhttps://halaser.eu",null,ALERT_OK);
            break;
         default:
            break;
      }
      return super.onMenuItemSelected(featureId, item);
   }


   protected void deleteConnection()
   {
      simpleAlert(getResources().getText(R.string.really_delete_conn).toString(),null,ALERT_NO_DELETE);
   }


   protected void showDetailView(ConnectionData data)
   {
      if (data!=null)
      {
         data.detailView(true);
         rootLayout.setVisibility(View.INVISIBLE);
         detailLayout.setVisibility(View.VISIBLE);
         detailScrollView.bringToFront();
      }
      else
      {
         viewConnection.detailView(false);
         detailLayout.setVisibility(View.INVISIBLE);
         rootLayout.setVisibility(View.VISIBLE);
         mainScrollView.bringToFront();
      }
      viewConnection=data;
   }


   public void onBackPressed()
   {
      if (viewConnection!=null)
      {
         showDetailView(null);
      }
      else simpleAlert(getResources().getText(R.string.really_exit_app).toString(),null,ALERT_NO_EXIT);
   }
 
   
   public void onClick(View v)
   {
      if (v==addConnectionButton)
      {
         final EditText txtUrl = new EditText(this);
         final BeamCtrl ctx=this;

         txtUrl.setHint("192.168.1.1");

         new AlertDialog.Builder(this)
                 .setTitle(ctx.getResources().getText(R.string.host))
                 .setMessage(ctx.getResources().getText(R.string.enter_ip_text))
                 .setView(txtUrl)
                 .setPositiveButton(ctx.getResources().getText(R.string.ok), new DialogInterface.OnClickListener()
                 {
                    public void onClick(DialogInterface dialog, int whichButton)
                    {
                       String host;

                       host=txtUrl.getText().toString();
                       if ((host!=null) && (host.length()>6))
                       {
                          ConnectionData data = new ConnectionData(ctx,host);
                          connectionList.add(data);
                          saveConnections();
                          createUI();
                       }
                    }
                 })
                 .setNegativeButton(ctx.getResources().getText(R.string.cancel), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                    }
                 })
                 .show();
      }
   }

         
   protected void onResume() 
   {
      int i;

      super.onResume();
      appVisible=true;

      try
      {
         textSizeVal=Integer.parseInt(SP.getString("textSize","1"));
      }
      catch (NumberFormatException nfe)
      {
      }
      if (wl!=null) wl.acquire();

      for (i=0; i<connectionList.size(); i++)
      {
         connectionList.elementAt(i).setContext(this);
      }

      if (viewConnection!=null)
      {
/*    TODO: switch back to detail view when it was opened
         showDetailView(viewConnection);
         viewConnection.updateUI();*/

         showDetailView(null);
      }
      ScanService.setConnectionData(connectionList);
   }

   
   
   protected void onPause() 
   {
/*      WMapEntry currEntry;
      int       j;*/
      
      if (wl!=null) wl.release();      
/*      ScanService.scanData.isActive=false; // try to stop the thread
      ScanService.scanData.appVisible=false;
      ScanService.scanData.lock.lock();
      if (ScanService.scanData.wmapList.size()>0) for (j=0; j<ScanService.scanData.wmapList.size(); j++)
      {         
         currEntry=ScanService.scanData.wmapList.elementAt(j);
         currEntry.flags&=~WMapEntry.FLAG_UI_USED;
         currEntry.flags&=~WMapEntry.FLAG_IS_VISIBLE;
         scannerHandler.parentTable.removeView(currEntry.row);
      }
      ScanService.scanData.lock.unlock();*/
      appVisible=false;
      super.onPause();
   }

   

}
