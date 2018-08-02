package eu.halaser.beamctrl;

import java.io.*;

import android.media.*;
import android.net.*;
import android.os.*;
import android.view.*;
import android.widget.*;
import android.content.*;


class ConnectionData implements View.OnClickListener
{
   private static final int NOTIFY_FLAG_OFFLINE         =0x00000001;
   private static final int NOTIFY_FLAG_READY_TO_IDLE   =0x00000002;
   private static final int NOTIFY_FLAG_MARKING_TO_READY=0x00000004;
   private static final int NOTIFY_FLAG_ERROR           =0x00000008;
   private static final int NOTIFY_FLAG_PAUSED          =0x00000010;

   private int    vibrationFlags,ringtoneFlags;
   String host;

   private String ID="",Time="",Text="",Project="";
   private int    Parts=0,MaxSlices=0,CurrSlice=0;

   private ImageView    stateImage,detStateImage;
   private ImageButton  deleteServerButton;
   private GridLayout   baseLayout;
   private TextView     stateText,connectionText,detStateText,detConnectionText,detTimeText,detErrorText,
                        detProjectText,detProjectParts,detProjectProgress;
   private ProgressBar  detProgressBar;
   private CheckBox     vibrateOfflineCB,soundOfflineCB,
                        vibrateIdleCB,soundIdleCB,
                        vibrateReadyCB,soundReadyCB,
                        vibrateErrorCB,soundErrorCB,
                        vibratePausedCB,soundPausedCB;
   private TextView     textSound,textVibrate,textDisconnected,textReadyToIdle,textMarkingToReady,textError;
   private BeamCtrl     ctx;
   private boolean      detailViewEnabled=false;
   private final int    STATE_DISCONNECTED=0;
   private final int    STATE_IDLE=1;
   private final int    STATE_READY=2;
   private final int    STATE_MARKING=3;
   private final int    STATE_ERROR=4;
   private final int    STATE_PAUSED=5;
   private final int    STATE_UNKNOWN=6;
   private int          state=STATE_DISCONNECTED;
   private Vibrator     v;
   private Ringtone     r;
   private boolean      notify=false;
   private ScanService  service;


   ConnectionData(BeamCtrl in_ctx)
   {
      ctx=in_ctx;
      initUI();
   }

   ConnectionData(BeamCtrl in_ctx,String in_host)
   {
      ctx=in_ctx;
      host=in_host;
      vibrationFlags=NOTIFY_FLAG_OFFLINE|NOTIFY_FLAG_MARKING_TO_READY|NOTIFY_FLAG_ERROR;
      ringtoneFlags=NOTIFY_FLAG_ERROR;
      initUI();
   }

   protected void setContext(BeamCtrl in_ctx)
   {
      ctx=in_ctx;
   }

   protected void setState(String in_state,ScanService in_service)
   {
      int     prevState=state;

      if (in_state==null) state=STATE_DISCONNECTED;
      else if (in_state.compareTo("idle")==0) state=STATE_IDLE;
      else if (in_state.compareTo("ready")==0) state=STATE_READY;
      else if (in_state.compareTo("marking")==0) state=STATE_MARKING;
      else if (in_state.compareTo("error")==0) state=STATE_ERROR;
      else if (in_state.compareTo("paused")==0) state=STATE_PAUSED;
      else state=STATE_UNKNOWN;

      if (prevState!=state)
      {
         service=in_service;

         if (((vibrationFlags & NOTIFY_FLAG_OFFLINE) == NOTIFY_FLAG_OFFLINE) &&
                 (state == STATE_DISCONNECTED))
         {
            v.vibrate(500);
            notify=true;
         }
         else if (((vibrationFlags & NOTIFY_FLAG_ERROR)==NOTIFY_FLAG_ERROR) &&
                  (state==STATE_ERROR))
         {
            v.vibrate(500);
            notify=true;
         }
         else if (((vibrationFlags & NOTIFY_FLAG_PAUSED)==NOTIFY_FLAG_PAUSED) &&
                  (state==STATE_PAUSED))
         {
            v.vibrate(500);
            notify=true;
         }
         else if (((vibrationFlags & NOTIFY_FLAG_READY_TO_IDLE) == NOTIFY_FLAG_READY_TO_IDLE) &&
                 (prevState == STATE_READY) &&
                 (state == STATE_IDLE))
         {
            v.vibrate(500);
            notify=true;
         }
         else if (((vibrationFlags & NOTIFY_FLAG_MARKING_TO_READY) == NOTIFY_FLAG_MARKING_TO_READY) &&
                 (prevState == STATE_MARKING) &&
                 (state == STATE_READY))
         {
            v.vibrate(500);
            notify=true;
         }

         if (((ringtoneFlags & NOTIFY_FLAG_OFFLINE) == NOTIFY_FLAG_OFFLINE) &&
                 (state == STATE_DISCONNECTED))
         {
            r.play();
            notify=true;
         }
         else if (((ringtoneFlags & NOTIFY_FLAG_ERROR)==NOTIFY_FLAG_ERROR) &&
                  (state==STATE_ERROR))
         {
            r.play();
            notify=true;
         }
         else if (((ringtoneFlags & NOTIFY_FLAG_PAUSED)==NOTIFY_FLAG_PAUSED) &&
                  (state==STATE_PAUSED))
         {
            r.play();
            notify=true;
         }
         else if (((ringtoneFlags & NOTIFY_FLAG_READY_TO_IDLE) == NOTIFY_FLAG_READY_TO_IDLE) &&
                 (prevState == STATE_READY) &&
                 (state == STATE_IDLE))
         {
            r.play();
            notify=true;
         }
         else if (((ringtoneFlags & NOTIFY_FLAG_MARKING_TO_READY) == NOTIFY_FLAG_MARKING_TO_READY) &&
                 (prevState == STATE_MARKING) &&
                 (state == STATE_READY))
         {
            r.play();
            notify=true;
         }
      }
   }


   void setID(String in_ID)
   {
      if (in_ID.compareTo(ID)!=0)
      {
         ID=in_ID;
      }
   }


   void setTime(String in_Time)
   {
      if (in_Time.compareTo(Time)!=0)
      {
         Time=in_Time;
      }
   }


   void setText(String in_Text)
   {
      if (in_Text.compareTo(Text)!=0)
      {
         Text=in_Text;
      }
   }


   void setProject(String in_Project)
   {
      if (in_Project.compareTo(Project)!=0)
      {
         Project=in_Project;
      }
   }


   void setParts(String in_Parts)
   {
      try
      {
         Parts = Integer.parseInt(in_Parts);
      }
      catch (Exception e)
      {
      }
   }


   void setProgress(String in_maxSlices,String in_currSlice)
   {
      try
      {
         int value;

         value=Integer.parseInt(in_maxSlices);
         if (value>0) MaxSlices=value;
         CurrSlice=Integer.parseInt(in_currSlice);
      }
      catch (Exception e)
      {
      }
   }


   void updateUI()
   {
      ctx.sendMessage(BeamCtrl.MsgHandler.MSG_UPDATE_CONNECTION_UI,0,0,this);
   }


   void doUpdateUI()
   {
      int          res;
      CharSequence text;

      connectionText.setText(ID+" ("+host+")");
      switch (state)
      {
         case STATE_DISCONNECTED:
            res=R.drawable.disconnected;
            text=ctx.getResources().getText(R.string.stateDisonnected);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.disconnected);
               detStateText.setText(ctx.getResources().getText(R.string.stateDisonnected));
            }
            break;
         case STATE_IDLE:
            res=R.drawable.idle;
            text=ctx.getResources().getText(R.string.stateIdle);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.idle);
               detStateText.setText(ctx.getResources().getText(R.string.stateIdle));
            }
            break;
         case STATE_READY:
            res=R.drawable.ready;
            text=ctx.getResources().getText(R.string.stateReady);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.ready);
               detStateText.setText(ctx.getResources().getText(R.string.stateReady));
            }
            break;
         case STATE_MARKING:
            res=R.drawable.marking;
            text=ctx.getResources().getText(R.string.stateMarking);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.marking);
               detStateText.setText(ctx.getResources().getText(R.string.stateMarking));
            }
            break;
         case STATE_ERROR:
            res=R.drawable.error;
            text=ctx.getResources().getText(R.string.stateError);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.error);
               detStateText.setText(ctx.getResources().getText(R.string.stateError));
            }
            break;
         case STATE_PAUSED:
            res=R.drawable.paused;
            text=ctx.getResources().getText(R.string.statePaused);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.paused);
               detStateText.setText(ctx.getResources().getText(R.string.statePaused));
            }
            break;
         default:
            res=R.drawable.unknown;
            text=ctx.getResources().getText(R.string.stateUnknown);
            if (detailViewEnabled)
            {
               detStateImage.setImageResource(R.drawable.unknown);
               detStateText.setText(ctx.getResources().getText(R.string.stateUnknown));
            }
            break;
      }

      stateImage.setImageResource(res);
      stateText.setText(text);

      if (notify)
      {
         service.setNotification(text.toString(),ID,host);
         notify=false;
      }

      if (detailViewEnabled)
      {
         detConnectionText.setText(ID+" ("+host+")");
         detTimeText.setText(ctx.getResources().getText(R.string.detTimeText)+" "+Time);
         if (state==STATE_ERROR)
         {
            detErrorText.setText(ctx.getResources().getText(R.string.detProjectError)+" "+Text);
            detErrorText.setTextColor(0xFFFF0000);
         }
         else
         {
            detErrorText.setText(ctx.getResources().getText(R.string.ok));
            detErrorText.setTextColor(0xFF00FF00);
         }
         detProjectText.setText(ctx.getResources().getText(R.string.detProjectText)+" "+Project);
         detProjectParts.setText(ctx.getResources().getText(R.string.detProjectParts)+" "+Parts);

         if ((MaxSlices>=CurrSlice) && (MaxSlices>0))
         {
            double percentage=(100.0*CurrSlice)/MaxSlices;

            if ((percentage>=0.0) && (percentage<=100.0))
            {
               detProjectProgress.setText(ctx.getResources().getText(R.string.detProjectProgress)+" "+percentage+"%");
               detProgressBar.setProgress((int)percentage);
            }
            else
            {
               detProjectProgress.setText(ctx.getResources().getText(R.string.detProjectProgress)+" ---");
               detProgressBar.setProgress(0);
            }
         }
      }
   }

   void initUI()
   {
      v = (Vibrator) ctx.getSystemService(Context.VIBRATOR_SERVICE);
      Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
      r = RingtoneManager.getRingtone(ctx.getApplicationContext(), notification);

      View view = LayoutInflater.from(ctx).inflate(R.layout.listitem,null);

      baseLayout=(GridLayout)view.findViewById(R.id.baseLayout);
      baseLayout.setClickable(true);
      baseLayout.setOnClickListener(this);

      stateImage=(ImageView)view.findViewById(R.id.stateImage);
      stateImage.setClickable(true);
      stateImage.setOnClickListener(this);

      connectionText=(TextView)view.findViewById(R.id.connectionText);
      connectionText.setClickable(true);
      connectionText.setOnClickListener(this);
      setTextStyle(connectionText);

      stateText=(TextView)view.findViewById(R.id.stateText);
      stateText.setClickable(true);
      stateText.setOnClickListener(this);
      setTextStyle(stateText);
      updateUI();
   }


   private void setTextStyle(TextView text)
   {
      if (BeamCtrl.textSizeVal==1) text.setTextAppearance(ctx,android.R.style.TextAppearance_Small);
      else if (BeamCtrl.textSizeVal==2) text.setTextAppearance(ctx,android.R.style.TextAppearance_Medium);
      else text.setTextAppearance(ctx,android.R.style.TextAppearance_Large);
   }

   void setUI(LinearLayout parent)
   {
      connectionText.setText(host);
      parent.addView(baseLayout);
   }


   void detailView(boolean enabled)
   {
      detailViewEnabled=enabled;
      if (enabled)
      {
         deleteServerButton=(ImageButton)ctx.findViewById(R.id.deleteServerButton);
         deleteServerButton.setOnClickListener(this);

         detStateImage=(ImageView)ctx.findViewById(R.id.detStateImage);
         detStateText=(TextView)ctx.findViewById(R.id.detStateText);
         setTextStyle(detStateText);

         detConnectionText=(TextView)ctx.findViewById(R.id.detConnectionText);
         setTextStyle(detConnectionText);

         detTimeText=(TextView)ctx.findViewById(R.id.detTimeText);
         setTextStyle(detTimeText);

         detErrorText=(TextView)ctx.findViewById(R.id.detProjectError);
         setTextStyle(detErrorText);

         detProjectText=(TextView)ctx.findViewById(R.id.detProjectText);
         setTextStyle(detProjectText);

         detProjectParts=(TextView)ctx.findViewById(R.id.detProjectParts);
         setTextStyle(detProjectParts);

         detProjectProgress=(TextView)ctx.findViewById(R.id.detProjectProgress);
         setTextStyle(detProjectProgress);

         detProgressBar=(ProgressBar)ctx.findViewById(R.id.detProgressBar);
         detProgressBar.setMax(100);

         vibrateOfflineCB=(CheckBox)ctx.findViewById(R.id.vibrateOfflineCB);
         if ((vibrationFlags & NOTIFY_FLAG_OFFLINE)==NOTIFY_FLAG_OFFLINE) vibrateOfflineCB.setChecked(true);
         else vibrateOfflineCB.setChecked(false);
         vibrateOfflineCB.setOnClickListener(this);

         soundOfflineCB=(CheckBox)ctx.findViewById(R.id.soundOfflineCB);
         if ((ringtoneFlags & NOTIFY_FLAG_OFFLINE)==NOTIFY_FLAG_OFFLINE) soundOfflineCB.setChecked(true);
         else soundOfflineCB.setChecked(false);
         soundOfflineCB.setOnClickListener(this);

         vibrateIdleCB=(CheckBox)ctx.findViewById(R.id.vibrateIdleCB);
         if ((vibrationFlags & NOTIFY_FLAG_READY_TO_IDLE)==NOTIFY_FLAG_READY_TO_IDLE) vibrateIdleCB.setChecked(true);
         else vibrateIdleCB.setChecked(false);
         vibrateIdleCB.setOnClickListener(this);

         soundIdleCB=(CheckBox)ctx.findViewById(R.id.soundIdleCB);
         if ((ringtoneFlags & NOTIFY_FLAG_READY_TO_IDLE)==NOTIFY_FLAG_READY_TO_IDLE) soundIdleCB.setChecked(true);
         else soundIdleCB.setChecked(false);
         soundIdleCB.setOnClickListener(this);

         vibrateReadyCB=(CheckBox)ctx.findViewById(R.id.vibrateReadyCB);
         if ((vibrationFlags & NOTIFY_FLAG_MARKING_TO_READY)==NOTIFY_FLAG_MARKING_TO_READY) vibrateReadyCB.setChecked(true);
         else vibrateReadyCB.setChecked(false);
         vibrateReadyCB.setOnClickListener(this);

         soundReadyCB=(CheckBox)ctx.findViewById(R.id.soundReadyCB);
         if ((ringtoneFlags & NOTIFY_FLAG_MARKING_TO_READY)==NOTIFY_FLAG_MARKING_TO_READY) soundReadyCB.setChecked(true);
         else soundReadyCB.setChecked(false);
         soundReadyCB.setOnClickListener(this);

         vibrateErrorCB=(CheckBox)ctx.findViewById(R.id.vibrateErrorCB);
         if ((vibrationFlags & NOTIFY_FLAG_ERROR)==NOTIFY_FLAG_ERROR) vibrateErrorCB.setChecked(true);
         else vibrateErrorCB.setChecked(false);
         vibrateErrorCB.setOnClickListener(this);

         soundErrorCB=(CheckBox)ctx.findViewById(R.id.soundErrorCB);
         if ((ringtoneFlags & NOTIFY_FLAG_ERROR)==NOTIFY_FLAG_ERROR) soundErrorCB.setChecked(true);
         else soundErrorCB.setChecked(false);
         soundErrorCB.setOnClickListener(this);

         vibratePausedCB=(CheckBox)ctx.findViewById(R.id.vibratePausedCB);
         if ((vibrationFlags & NOTIFY_FLAG_PAUSED)==NOTIFY_FLAG_PAUSED) vibratePausedCB.setChecked(true);
         else vibratePausedCB.setChecked(false);
         vibratePausedCB.setOnClickListener(this);

         soundPausedCB=(CheckBox)ctx.findViewById(R.id.soundPausedCB);
         if ((ringtoneFlags & NOTIFY_FLAG_PAUSED)==NOTIFY_FLAG_PAUSED) soundPausedCB.setChecked(true);
         else soundPausedCB.setChecked(false);
         soundPausedCB.setOnClickListener(this);

         textVibrate=(TextView)ctx.findViewById(R.id.textVibrate);
         setTextStyle(textVibrate);

         textSound=(TextView)ctx.findViewById(R.id.textSound);
         setTextStyle(textSound);

         textReadyToIdle=(TextView)ctx.findViewById(R.id.textReadyToIdle);
         setTextStyle(textReadyToIdle);

         textDisconnected=(TextView)ctx.findViewById(R.id.textDisconnected);
         setTextStyle(textDisconnected);

         textReadyToIdle=(TextView)ctx.findViewById(R.id.textReadyToIdle);
         setTextStyle(textReadyToIdle);

         textMarkingToReady=(TextView)ctx.findViewById(R.id.textMarkingToReady);
         setTextStyle(textMarkingToReady);

         textError=(TextView)ctx.findViewById(R.id.textError);
         setTextStyle(textError);

      }
      updateUI();
   }


   public void onClick(View v)
   {
      if ((v==vibrateOfflineCB) ||
          (v==soundOfflineCB) ||
          (v==vibrateIdleCB) ||
          (v==soundIdleCB) ||
          (v==vibrateReadyCB) ||
          (v==soundReadyCB) ||
          (v==vibrateErrorCB) ||
          (v==soundErrorCB) ||
          (v==vibratePausedCB) ||
          (v==soundPausedCB))
      {
         if (vibrateOfflineCB.isChecked()) vibrationFlags|=NOTIFY_FLAG_OFFLINE;
         else vibrationFlags&=~NOTIFY_FLAG_OFFLINE;

         if (soundOfflineCB.isChecked()) ringtoneFlags|=NOTIFY_FLAG_OFFLINE;
         else ringtoneFlags&=~NOTIFY_FLAG_OFFLINE;

         if (vibrateIdleCB.isChecked()) vibrationFlags|=NOTIFY_FLAG_READY_TO_IDLE;
         else vibrationFlags&=~NOTIFY_FLAG_READY_TO_IDLE;

         if (soundIdleCB.isChecked()) ringtoneFlags|=NOTIFY_FLAG_READY_TO_IDLE;
         else ringtoneFlags&=~NOTIFY_FLAG_READY_TO_IDLE;

         if (vibrateReadyCB.isChecked()) vibrationFlags|=NOTIFY_FLAG_MARKING_TO_READY;
         else vibrationFlags&=~NOTIFY_FLAG_MARKING_TO_READY;

         if (soundReadyCB.isChecked()) ringtoneFlags|=NOTIFY_FLAG_MARKING_TO_READY;
         else ringtoneFlags&=~NOTIFY_FLAG_MARKING_TO_READY;

         if (vibrateErrorCB.isChecked()) vibrationFlags|=NOTIFY_FLAG_ERROR;
         else vibrationFlags&=~NOTIFY_FLAG_ERROR;

         if (soundErrorCB.isChecked()) ringtoneFlags|=NOTIFY_FLAG_ERROR;
         else ringtoneFlags&=~NOTIFY_FLAG_ERROR;

         if (vibratePausedCB.isChecked()) vibrationFlags|=NOTIFY_FLAG_PAUSED;
         else vibrationFlags&=~NOTIFY_FLAG_PAUSED;

         if (soundPausedCB.isChecked()) ringtoneFlags|=NOTIFY_FLAG_PAUSED;
         else ringtoneFlags&=~NOTIFY_FLAG_PAUSED;

         ctx.saveConnections();
      }
      else if (v==deleteServerButton) ctx.deleteConnection();
      else ctx.showDetailView(this);
   }

      void writeData(DataOutputStream writer)
   {
      try
      {
         int    len,i;
         char[] cArray;

         len=host.length();
         writer.writeInt(len);
         cArray=host.toCharArray();
         for (i=0; i<len; i++) writer.writeChar(cArray[i]);

         writer.writeInt(vibrationFlags);
         writer.writeInt(ringtoneFlags);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
         writer.writeInt(0);
      }
      catch (IOException ioe) {}
   }


   boolean readData(DataInputStream reader)
   {
      int    len,i;
      char[] cArray;

      try
      {
         len=reader.readInt();
         if (len>100) return false;
         cArray=new char[len];
         for (i=0; i<len; i++) cArray[i]=reader.readChar();
         host=new String(cArray);
         vibrationFlags=reader.readInt();
         ringtoneFlags=reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();
         reader.readInt();

         return true;
      }
      catch (IOException ioe) {}
      return false;
   }


}
