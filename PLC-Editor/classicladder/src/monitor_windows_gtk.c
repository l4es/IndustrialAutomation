/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* October 2011 */
/* ------------------------------------- */
/* Monitor connect window (IP or serial) */
/* + frames logs/monitor windows         */
/* ------------------------------------- */
/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <zlib.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "menu_and_toolbar_gtk.h"
#include "preferences.h"
#include "classicladder_gtk.h"
//#include "tasks.h"
#include "time_and_rtc.h"
#include "monitor_windows_gtk.h"
// for FillComboBoxConfigSlavesList()
#include "config_gtk.h"
#include "socket_modbus_master.h"

/* for connect window */
static GtkWidget* pDialogBox;
static GtkWidget * pRadioModeChoice[ 3 ];
static GtkWidget * pEntryIP;
static GtkWidget * pEntrySerial,*pEntrySerialSpeed;
static GtkWidget * pEntryWaitTimeout;
static GtkWidget * pEntryTelephoneNumber;

/* for monitor window */
typedef struct StrFramesLogWindow
{
	GtkWidget *MonitorWindow;
	GtkWidget *MonitorFramesView;
	GtkWidget *MonitorAutoScroll;
	GtkWidget *MonitorManualFrameEntry;
}StrFramesLogWindow;
StrFramesLogWindow FramesLogWindow[ NBR_FRAMES_LOG_WINDOWS ];
GtkWidget *ComboSlaveStatsSelect;
GtkWidget * LabelStatsResults;

void SetSensitivesDependingMode( void )
{
	char bIpMode = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[0]) );
	char bModem = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[2]) );
	gtk_widget_set_sensitive( pEntryIP, bIpMode );
	gtk_widget_set_sensitive( pEntrySerial, !bIpMode );
	gtk_widget_set_sensitive( pEntrySerialSpeed, !bIpMode );
	gtk_widget_set_sensitive( pEntryTelephoneNumber, bModem );
}

void RadioModeChoiceToggledClick (GtkToggleButton *togglebutton, gpointer user_data)
{
	printf("radio click...\n");
	SetSensitivesDependingMode( );
}

/* to simulate "OK" click when return done on IP address / Serial port/speed */
static void DialogConnectDoResponseOk( void )
{
	gtk_dialog_response( GTK_DIALOG(pDialogBox), GTK_RESPONSE_OK );
}

char OpenDialogMonitorConnect( void )
{
	GtkWidget * hbox[ 5 ];
	GtkWidget * pLabel;
	char Buff[ 30 ];
	GtkWidget *DialogContentArea;

	char OkDone = FALSE;

	if ( InfosGUI->TargetMonitor.RemoteConnected || InfosGUI->TargetMonitor.RemoteFileTransfer )
	{
		ShowMessageBoxError( _("Already in communication with the remote target (monitor or file transfer)...") );
		return OkDone;
	}

	pDialogBox = gtk_dialog_new_with_buttons( InfosGUI->TargetMonitor.TransferFileNum==-1?(_("Target to connect")):(_("File transfer")),
		GTK_WINDOW( MainSectionWindow ),
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	
	//ForGTK3
	DialogContentArea = gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox));

	hbox[0] = gtk_hbox_new (FALSE, 0);
//ForGTK3	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[0]);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[0]);
	pRadioModeChoice[0] = gtk_radio_button_new_with_label( NULL, _("IP network") );
	gtk_signal_connect(GTK_OBJECT (pRadioModeChoice[0]), "toggled",
					GTK_SIGNAL_FUNC(RadioModeChoiceToggledClick), (void*)NULL);
	gtk_box_pack_start(GTK_BOX(hbox[0]), pRadioModeChoice[0], TRUE, TRUE, 0);
	pRadioModeChoice[1] = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON (pRadioModeChoice[0]), _("Serial link") );
	gtk_signal_connect(GTK_OBJECT (pRadioModeChoice[1]), "toggled",
					GTK_SIGNAL_FUNC(RadioModeChoiceToggledClick), (void*)NULL);
	gtk_box_pack_start(GTK_BOX(hbox[0]), pRadioModeChoice[1], TRUE, TRUE, 0);
	pRadioModeChoice[2] = gtk_radio_button_new_with_label_from_widget ( GTK_RADIO_BUTTON (pRadioModeChoice[0]), _("Modem") );
	gtk_signal_connect(GTK_OBJECT (pRadioModeChoice[2]), "toggled",
					GTK_SIGNAL_FUNC(RadioModeChoiceToggledClick), (void*)NULL);
	gtk_box_pack_start(GTK_BOX(hbox[0]), pRadioModeChoice[2], TRUE, TRUE, 0);

	hbox[1] = gtk_hbox_new (FALSE, 0);
//ForGTK3	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[1]);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[1]);
	pLabel = gtk_label_new( _("IP address or hostname") );
	gtk_box_pack_start(GTK_BOX(hbox[1]), pLabel, FALSE, FALSE, 0);
	pEntryIP = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(pEntryIP), InfosGUI->TargetMonitor.RemoteAdrIP );
	gtk_box_pack_start(GTK_BOX(hbox[1]), pEntryIP, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(pEntryIP), "activate",
				GTK_SIGNAL_FUNC(DialogConnectDoResponseOk), 0);

	hbox[2] = gtk_hbox_new (FALSE, 0);
//ForGTK3	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[2]);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[2]);
	pLabel = gtk_label_new( _("Serial port") );
	gtk_box_pack_start(GTK_BOX(hbox[2]), pLabel, FALSE, FALSE, 0);
	pEntrySerial = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(pEntrySerial), InfosGUI->TargetMonitor.RemoteWithSerialPort );
	gtk_box_pack_start(GTK_BOX(hbox[2]), pEntrySerial, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(pEntrySerial), "activate",
				GTK_SIGNAL_FUNC(DialogConnectDoResponseOk), 0);
	pLabel = gtk_label_new( _("Speed") );
	gtk_box_pack_start(GTK_BOX(hbox[2]), pLabel, FALSE, FALSE, 0);
	pEntrySerialSpeed = gtk_entry_new();
	sprintf( Buff, "%d", InfosGUI->TargetMonitor.RemoteWithSerialSpeed );
	gtk_entry_set_text( GTK_ENTRY(pEntrySerialSpeed), Buff );
	gtk_box_pack_start(GTK_BOX(hbox[2]), pEntrySerialSpeed, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(pEntrySerialSpeed), "activate",
				GTK_SIGNAL_FUNC(DialogConnectDoResponseOk), 0);

	hbox[3] = gtk_hbox_new (FALSE, 0);
//ForGTK3	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[3]);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[3]);
	pLabel = gtk_label_new( _("Telephone number") );
	gtk_box_pack_start(GTK_BOX(hbox[3]), pLabel, FALSE, FALSE, 0);
	pEntryTelephoneNumber = gtk_entry_new( );
	gtk_entry_set_text( GTK_ENTRY(pEntryTelephoneNumber), InfosGUI->TargetMonitor.RemoteTelephoneNumber );
	gtk_box_pack_start(GTK_BOX(hbox[3]), pEntryTelephoneNumber, FALSE, FALSE, 0);

	hbox[4] = gtk_hbox_new (FALSE, 0);
//ForGTK3	gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[4]);
	gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[4]);
	pLabel = gtk_label_new( _("Reply timeout (ms)") );
	gtk_box_pack_start(GTK_BOX(hbox[4]), pLabel, FALSE, FALSE, 0);
	pEntryWaitTimeout = gtk_entry_new();
	sprintf( Buff, "%d", InfosGUI->TargetMonitor.TimeOutWaitReply );
	gtk_entry_set_text( GTK_ENTRY(pEntryWaitTimeout), Buff );
	gtk_box_pack_start(GTK_BOX(hbox[4]), pEntryWaitTimeout, FALSE, FALSE, 0);

	if ( InfosGUI->TargetMonitor.RemoteWithSerialPort[0]!='\0' )
	{
		if ( InfosGUI->TargetMonitor.RemoteWithSerialModem )
		{
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[2]), TRUE );
			gtk_widget_grab_focus( pEntryTelephoneNumber );
		}
		else
		{
			gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[1]), TRUE );
			gtk_widget_grab_focus( pEntrySerial );
		}
	}
	else
	{
		gtk_widget_grab_focus( pEntryIP );
	}
	SetSensitivesDependingMode( );
//ForGTK3	gtk_widget_show_all(GTK_DIALOG(pDialogBox)->vbox);
	gtk_widget_show_all(DialogContentArea);

	switch (gtk_dialog_run(GTK_DIALOG(pDialogBox)))
	{
		case GTK_RESPONSE_OK:
		{
			char bIpMode = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[0]) );
			if ( bIpMode )
			{
				strcpy( InfosGUI->TargetMonitor.RemoteAdrIP, gtk_entry_get_text(GTK_ENTRY(pEntryIP) ) );
				InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ] = '\0';
			}
			else
			{
				InfosGUI->TargetMonitor.RemoteAdrIP[ 0 ] = '\0';
				strcpy( InfosGUI->TargetMonitor.RemoteWithSerialPort, gtk_entry_get_text(GTK_ENTRY(pEntrySerial) ) );
				InfosGUI->TargetMonitor.RemoteWithSerialSpeed = atoi( gtk_entry_get_text(GTK_ENTRY(pEntrySerialSpeed) ) );
				InfosGUI->TargetMonitor.RemoteWithSerialModem = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(pRadioModeChoice[2]) );
				if ( InfosGUI->TargetMonitor.RemoteWithSerialModem )
					strcpy( InfosGUI->TargetMonitor.RemoteTelephoneNumber, gtk_entry_get_text(GTK_ENTRY(pEntryTelephoneNumber) ) );
			}
			InfosGUI->TargetMonitor.TimeOutWaitReply = atoi( gtk_entry_get_text(GTK_ENTRY(pEntryWaitTimeout) ) );
			OkDone = TRUE;
			break;
		}
	}

	gtk_widget_destroy(pDialogBox);
	return OkDone;
}

/* ---- Frames monitor Window ---- */

void ButtonCleanUpSignal( GtkWidget *widget, gpointer data )
{
	int NumFrameLogBuff = (int)data;
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFrameLogBuff ];
printf(">>> cleanup for monitor window %d\n", NumFrameLogBuff);
	GtkTextBuffer *TextBufferForGtk;
        /* get the text buffer */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( pFramesLogWindow->MonitorFramesView ) );
	gtk_text_buffer_set_text( TextBufferForGtk, "", -1 );
	// buffered one ?
	if ( NumFrameLogBuff>=0 && NumFrameLogBuff<NBR_FRAMES_LOG_BUFFERS )
	{
		if ( InfosGUI->TargetMonitor.RemoteConnected )
			InfosGUI->TargetMonitor.AskTargetToCleanUp = 20+NumFrameLogBuff; /*FramesLog*/
		else
			FrameLogCleanupAsked( NumFrameLogBuff );
	}
}
void ButtonCopyToClipboardSignal( GtkWidget *widget, gpointer data )
{
	int NumFrameLogBuff = (int)data;
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFrameLogBuff ];
printf(">>> copy clipboard for monitor window %d\n", NumFrameLogBuff);
	GtkTextBuffer *TextBufferForGtk;
	gchar*         clipboard_str;
	GtkTextIter    start, end;
        /* get the text buffer */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( pFramesLogWindow->MonitorFramesView ) );

	gtk_text_buffer_get_start_iter(TextBufferForGtk, &start);
	gtk_text_buffer_get_end_iter(TextBufferForGtk, &end);
	clipboard_str = gtk_text_buffer_get_text( TextBufferForGtk, &start, &end, TRUE);
printf("---------------------------\n");
printf("contenu clipboard = %s\n", clipboard_str );
	gtk_clipboard_set_text(
		gtk_clipboard_get(/*GDK_SELECTION_PRIMARY*/GDK_SELECTION_CLIPBOARD),
		clipboard_str, -1 );//g_utf8_strlen( clipboard_str, -1) );
	gtk_clipboard_store(gtk_clipboard_get(/*GDK_SELECTION_PRIMARY*/GDK_SELECTION_CLIPBOARD));
}
// only for monitor master window
static gint MonitorManualFrameEntrySignal(GtkWidget *widget, int NumVarSpy)
{
	int StrLgt = strlen( gtk_entry_get_text( GTK_ENTRY(FramesLogWindow[FRAMES_LOG_MONITOR_MASTER].MonitorManualFrameEntry) ) );
	if ( StrLgt>0 )
	{
		char * StrAllocated = malloc( StrLgt+1 );
		if ( StrAllocated!=NULL )
		{
			strcpy( StrAllocated, gtk_entry_get_text( GTK_ENTRY( FramesLogWindow[FRAMES_LOG_MONITOR_MASTER].MonitorManualFrameEntry) ) );
			InfosGUI->TargetMonitor.AskTargetForThisManualFrame = StrAllocated;
		}
	}
	return TRUE;
}
gint MonitorWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
	int NumFrameLogBuff = (int)data;
	printf("MonitorWindowDeleteEvent, window=%d\n", NumFrameLogBuff);
// Here, we must only toggle the menu check that will call itself the function below to close the window ...
//	gtk_widget_hide( MonitorWindow );
	SetToggleMenuForMonitorWindow( NumFrameLogBuff, FALSE/*OpenedWin*/ );
	// we do not want that the window be destroyed.
	return TRUE;
}

void SignalStatisticsModbusSlaveSelect( GtkComboBox * pCombo, gpointer user_data )
{
	char BuffValue[ 100 ];
	int NumSlave = gtk_combo_box_get_active( GTK_COMBO_BOX(ComboSlaveStatsSelect) );
	if ( NumSlave>=0 )
		GetSocketModbusMasterStats( NumSlave, BuffValue );
	else
		strcpy(BuffValue, "---" );
	gtk_label_set_text( GTK_LABEL(LabelStatsResults), BuffValue );
}
gboolean StatisticsModbusSlaveSelectedRefreshInfosGtk( void )
{
	SignalStatisticsModbusSlaveSelect( NULL, NULL );
	return FALSE; //usefull when called with g_idle_add (just one time)
}

// called when window opened or when refresh button clicked
// usefull for a target connected,
// and for now also for Gtk application... TODO: perhaps frames should be directly displayed permanently in the window !?
void RefreshMonitorWindowSignal( GtkWidget *widget, gpointer data )
{
	int NumFrameLogBuff = (int)data;
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFrameLogBuff ];
printf(">>> refresh for window %d\n", NumFrameLogBuff);
	GtkTextBuffer *TextBufferForGtk;
        /* get the text buffer */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( pFramesLogWindow->MonitorFramesView ) );
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		gtk_text_buffer_set_text( TextBufferForGtk, _("Asked to read frames log buffer file of the target..."), -1 );
		InfosGUI->TargetMonitor.TransferFileNum = 20+NumFrameLogBuff;
		InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
printf("*** HERE SET REMOTE_FILE_TRANSFER ---FRAMES_LOG(%d)--- TO RECEIVE!\n",InfosGUI->TargetMonitor.TransferFileNum);
		TransferMasterStart( );
	}
	// here case of Gtk application frames (using same file as for embedded...) !
	else
	{
		char * TmpFramesLogFileName = "cl_tmp_frames_log.txt.gz";
		SaveFramesLogTextCompressed( NumFrameLogBuff, TmpFramesLogFileName );
		DisplayFrameLogFromFileTextCompressed( NumFrameLogBuff, TmpFramesLogFileName );
		remove( TmpFramesLogFileName );
	}
	if ( NumFrameLogBuff==FRAMES_LOG_MODBUS_MASTER )
		SignalStatisticsModbusSlaveSelect( NULL, NULL );
}

void OpenMonitorWindow( int NumFramesLogWindow, GtkAction * ActionOpen, gboolean OpenIt )
{
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFramesLogWindow ];
	char Buff[ 30 ];
	sprintf( Buff, "FramesLogWindow%d", NumFramesLogWindow );
	if ( ActionOpen!=NULL )
		OpenIt = gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(ActionOpen) );
printf("*** HERE %s MONITOR WINDOW : %s !\n",Buff,OpenIt?"OPEN":"CLOSE");
	if ( OpenIt )
	{
		RestoreWindowPosiPrefs( Buff, pFramesLogWindow->MonitorWindow );
		gtk_widget_show( pFramesLogWindow->MonitorWindow );
		gtk_window_present( GTK_WINDOW(pFramesLogWindow->MonitorWindow) );
		if ( NumFramesLogWindow!=FRAMES_LOG_MONITOR_MASTER )
		{
			RefreshMonitorWindowSignal( NULL, (gpointer)NumFramesLogWindow );
		}
	}
	else
	{
		RememberWindowPosiPrefs( Buff, pFramesLogWindow->MonitorWindow, TRUE/*SaveWindowSize*/ );
		gtk_widget_hide( pFramesLogWindow->MonitorWindow );
	}
}
// called per each toggle action menu, or at startup (if window saved open or not)...
void OpenMonitorWindow0( GtkAction * ActionOpen, gboolean OpenIt )
{
	OpenMonitorWindow( 0, ActionOpen, OpenIt );
}
void OpenMonitorWindow1( GtkAction * ActionOpen, gboolean OpenIt )
{
	OpenMonitorWindow( 1, ActionOpen, OpenIt );
}
void OpenMonitorWindow2( GtkAction * ActionOpen, gboolean OpenIt )
{
	OpenMonitorWindow( 2, ActionOpen, OpenIt );
}
void OpenMonitorWindow3( GtkAction * ActionOpen, gboolean OpenIt )
{
	OpenMonitorWindow( 3, ActionOpen, OpenIt );
}
void OpenMonitorWindow4( GtkAction * ActionOpen, gboolean OpenIt )
{
	OpenMonitorWindow( 4, ActionOpen, OpenIt );
}

void RememberMonitorWindowPrefs( int NumFramesLogWindow )
{
	char Buff[ 30 ];
//ForGTK3	char WindowIsOpened = GTK_WIDGET_VISIBLE( GTK_WINDOW(FramesLogWindow[ NumFramesLogWindow ].MonitorWindow) );
	char WindowIsOpened = MY_GTK_WIDGET_VISIBLE( FramesLogWindow[ NumFramesLogWindow ].MonitorWindow );
	sprintf( Buff, "FramesLogWindow%d", NumFramesLogWindow );
	RememberWindowOpenPrefs( Buff, WindowIsOpened );
	if ( WindowIsOpened )
		RememberWindowPosiPrefs( Buff, FramesLogWindow[ NumFramesLogWindow ].MonitorWindow, TRUE/*SaveWindowSize*/ );
}
void MonitorWindowInitGtk( int NumFramesLogWindow )
{
	GtkWidget *vbox,*hbox;
	GtkWidget *ScrollWin,*ButtonCleanUp,*ButtonCopyToClipboard,*ButtonRefresh;
	if ( NumFramesLogWindow>=0 && NumFramesLogWindow<NBR_FRAMES_LOG_WINDOWS )
	{
		StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFramesLogWindow ];
		pFramesLogWindow->MonitorWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		const char * Title = "---";
		switch( NumFramesLogWindow )
		{
			case FRAMES_LOG_MODBUS_MASTER: Title = _("Modbus master frames (buffered)"); break;
			case FRAMES_LOG_MONITOR_SLAVE_IP: Title = _("Target monitor slave (IP) frames (buffered)"); break;
			case FRAMES_LOG_MONITOR_SLAVE_SERIAL: Title = _("Target monitor slave (Serial) frames (buffered)"); break;
			case FRAMES_LOG_MODBUS_SLAVE_IP: Title = _("Modbus slave/server (IP) frames (buffered)"); break;
			case FRAMES_LOG_MONITOR_MASTER: Title = _("Monitor master frames with target (live)"); break;
		}
		gtk_window_set_title ((GtkWindow *)pFramesLogWindow->MonitorWindow, Title);
		gtk_window_set_default_size(GTK_WINDOW(pFramesLogWindow->MonitorWindow), 400, 200);
		gtk_signal_connect( GTK_OBJECT( pFramesLogWindow->MonitorWindow ), "delete_event",
			GTK_SIGNAL_FUNC(MonitorWindowDeleteEvent), (gpointer)NumFramesLogWindow );

		vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_add(GTK_CONTAINER(pFramesLogWindow->MonitorWindow),vbox);
		gtk_widget_show (vbox);

		/* Create a Scrolled Window that will contain the GtkTextView */
		ScrollWin = gtk_scrolled_window_new (NULL, NULL);
		gtk_box_pack_start(GTK_BOX(vbox), ScrollWin, TRUE, TRUE, 5);
		gtk_widget_show (ScrollWin);

		pFramesLogWindow->MonitorFramesView = gtk_text_view_new ();
		gtk_container_add(GTK_CONTAINER(ScrollWin),pFramesLogWindow->MonitorFramesView);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrollWin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_widget_set_sensitive( pFramesLogWindow->MonitorFramesView, FALSE );
		gtk_widget_show( pFramesLogWindow->MonitorFramesView );

		hbox = gtk_hbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
		gtk_widget_show (hbox);

		if ( NumFramesLogWindow!=FRAMES_LOG_MONITOR_MASTER )
		{
			ButtonRefresh = gtk_button_new_with_label( _("Refresh") );
			gtk_box_pack_start(GTK_BOX(hbox),ButtonRefresh,FALSE,FALSE,0);
			gtk_signal_connect(GTK_OBJECT (ButtonRefresh), "clicked", 
					GTK_SIGNAL_FUNC(RefreshMonitorWindowSignal), (gpointer)NumFramesLogWindow);
			gtk_widget_show( ButtonRefresh );
		}
		pFramesLogWindow->MonitorAutoScroll = gtk_check_button_new_with_label( _("Scroll") );
		gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( pFramesLogWindow->MonitorAutoScroll ), TRUE );
		gtk_box_pack_start (GTK_BOX(hbox), pFramesLogWindow->MonitorAutoScroll, FALSE, FALSE, 0);
		gtk_widget_show (pFramesLogWindow->MonitorAutoScroll);
		ButtonCleanUp = gtk_button_new_with_label( _("CleanUp") );
		gtk_box_pack_start(GTK_BOX(hbox),ButtonCleanUp,FALSE,FALSE,0);
		gtk_signal_connect(GTK_OBJECT (ButtonCleanUp), "clicked", 
				GTK_SIGNAL_FUNC(ButtonCleanUpSignal), (gpointer)NumFramesLogWindow);
		gtk_widget_show( ButtonCleanUp );
		if ( NumFramesLogWindow==FRAMES_LOG_MONITOR_MASTER )
		{
			pFramesLogWindow->MonitorManualFrameEntry = gtk_entry_new( );
			gtk_box_pack_start(GTK_BOX(hbox),pFramesLogWindow->MonitorManualFrameEntry,FALSE,FALSE,0);
			gtk_signal_connect(GTK_OBJECT (pFramesLogWindow->MonitorManualFrameEntry), "activate", 
					GTK_SIGNAL_FUNC(MonitorManualFrameEntrySignal), 0);
			gtk_widget_show( pFramesLogWindow->MonitorManualFrameEntry );
		}
		ButtonCopyToClipboard = gtk_button_new_with_label( _("Copy to clipboard") );
		gtk_box_pack_start(GTK_BOX(hbox),ButtonCopyToClipboard,FALSE,FALSE,0);
		gtk_signal_connect(GTK_OBJECT (ButtonCopyToClipboard), "clicked", 
				GTK_SIGNAL_FUNC(ButtonCopyToClipboardSignal), (gpointer)NumFramesLogWindow);
		gtk_widget_show( ButtonCopyToClipboard );
		
		if ( NumFramesLogWindow==FRAMES_LOG_MODBUS_MASTER )
		{
			// some statistics... to debug my AVR I/O module !
//////			char BuffValue[ 100 ];
			GtkWidget *hboxStats,*LabelStats;
			hboxStats =  gtk_hbox_new (FALSE/*homogeneous*/, 0/*spacing*/);
			gtk_container_add (GTK_CONTAINER (vbox), hboxStats);
//added to avoid that with the combobox too height of the hbox...
gtk_box_set_child_packing(GTK_BOX(vbox), hboxStats,
		/*expand*/ FALSE, /*fill*/ FALSE, /*pad*/ 0, GTK_PACK_START);

			gtk_widget_show (hboxStats);
			LabelStats = gtk_label_new( _("Stats for modbus slave:") );
		//	gtk_widget_set_usize( LabelComParam[NumLine],250,0 );
			gtk_box_pack_start( GTK_BOX(hboxStats), LabelStats, FALSE/*expand*/, FALSE/*fill*/, 0/*padding */ );
			gtk_widget_show( LabelStats );
			ComboSlaveStatsSelect = gtk_combo_box_new_text( );
//////			FillComboBoxConfigSlavesList( GTK_COMBO_BOX( ComboSlaveStatsSelect ), TRUE/*ListForStatsSelect*/, FALSE/*CleanUpBefore*/ );
//////			gtk_combo_box_set_active( GTK_COMBO_BOX( ComboSlaveStatsSelect ), 0 );
			gtk_box_pack_start( GTK_BOX(hboxStats), ComboSlaveStatsSelect, FALSE, FALSE, 0 );
			gtk_widget_show( ComboSlaveStatsSelect );
			gtk_signal_connect( GTK_OBJECT(ComboSlaveStatsSelect), "changed",
												GTK_SIGNAL_FUNC(SignalStatisticsModbusSlaveSelect), (void *)NULL );
//////			GetSocketModbusMasterStats( /*TODO, choose slave wanted!*/0, BuffValue );
			LabelStatsResults = gtk_label_new( "---" /*BuffValue*/ );
			gtk_box_pack_start( GTK_BOX(hboxStats), LabelStatsResults, FALSE, FALSE, 0 );
			gtk_widget_show( LabelStatsResults );
		}

	}
}

void UpdateMonitorModbusSlaveListStats( void )
{
	FillComboBoxConfigSlavesList( MY_GTK_COMBO_BOX( ComboSlaveStatsSelect ), TRUE/*ListForStatsSelect*/, TRUE/*CleanUpBefore*/ );
	gtk_combo_box_set_active( GTK_COMBO_BOX( ComboSlaveStatsSelect ), 0 );
}

void FramesLogWindowsInitGtk()
{
	int NumFramesLogWindow;
	for( NumFramesLogWindow=0; NumFramesLogWindow<NBR_FRAMES_LOG_WINDOWS; NumFramesLogWindow++ )
		MonitorWindowInitGtk( NumFramesLogWindow );
}

// do not call this function directly, use MonitorWindowAddText() ! */
// called with g_idle_add() which do not have the main GTK+ lock,
// so gdk_threads_enter() & gdk_threads_leave() functions required...
gboolean MonitorWindowAddTextFromMainGtk( gpointer text )
{
	GtkTextBuffer *TextBufferForGtk;
	GtkTextIter iter;
	GtkTextMark *mark;
	char * TextToFreeAfterUse = (char *)text;
	int NumFramesLogWindow = (int)TextToFreeAfterUse[0];
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFramesLogWindow ];

gdk_threads_enter();

//ForGTK3	if ( GTK_WIDGET_VISIBLE( GTK_WINDOW(pFramesLogWindow->MonitorWindow) ) )
	if ( MY_GTK_WIDGET_VISIBLE( pFramesLogWindow->MonitorWindow ) )
	{

#ifdef AAAAAAAAA
	/* Get the text buffer */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( MonitorFramesView ) );
	/* Get end iter */
	gtk_text_buffer_get_end_iter( TextBufferForGtk, &iter );
	/* Add the message to the text buffer */
	gtk_text_buffer_insert( TextBufferForGtk, &iter, text, -1 );
	/* Scroll to end iter */
	gtk_text_view_scroll_to_iter( GTK_TEXT_VIEW (MonitorFramesView),
									&iter, 0.0, FALSE, 0, 0 );
#endif
	/* Returns the GtkTextBuffer being displayed by this text view. */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW(pFramesLogWindow->MonitorFramesView) );
	/* Returns the mark that represents the cursor (insertion point). */
	mark = gtk_text_buffer_get_insert( TextBufferForGtk );
	/* Initializes iter with the current position of mark. */
	gtk_text_buffer_get_iter_at_mark( TextBufferForGtk, &iter, mark );
	/* Inserts buffer at position iter. */
	gtk_text_buffer_insert( TextBufferForGtk, &iter, &TextToFreeAfterUse[1], -1 );
	/* Scrolls text_view the minimum distance such that mark is contained within the visible area of the widget. */
	if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( pFramesLogWindow->MonitorAutoScroll ) ) )
		gtk_text_view_scroll_mark_onscreen( GTK_TEXT_VIEW(pFramesLogWindow->MonitorFramesView), mark ); 

	}

gdk_threads_leave();

	free( TextToFreeAfterUse );
	return FALSE; // add line one time.
}

// g_idle_add() technic call must be used to run on Win32 (called per gtk_main task...)
// gdk_threads_enter() & gdk_threads_leave() in another thread with gtk functions between only working on Linux.
void MonitorWindowAddText( int NumFramesLogWindow, char * text )
{
	char BuffCurrTime[ 30 ];
	char * AllocText = malloc( 1+strlen(text)+1 +9);
	if ( AllocText )
	{
		// first character = frames log window number !!!
		AllocText[0] = (char)NumFramesLogWindow;
		if ( text[0]!='\n' )
		{
			GetCurrentAscTime( BuffCurrTime );
			strcpy( &AllocText[1], &BuffCurrTime[9] );
			AllocText[9] = ' ';
			strcpy( &AllocText[10], text );
		}
		else
		{
			strcpy( &AllocText[1], text );
		}
		g_idle_add( MonitorWindowAddTextFromMainGtk, AllocText );
	}
	else
	{
		printf("Failed to alloc mem in %s()\n", __FUNCTION__);
	}

#ifdef BBBBBBBBBBBBBBBBBBB
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	GtkTextMark *mark;

gdk_threads_enter();

	if ( GTK_WIDGET_VISIBLE( GTK_WINDOW(MonitorWindow) ) )
	{
#ifdef AAAAAAAAAAAAAAAA
		/* Get the text buffer */
		buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW( MonitorFramesView ) );
		/* Get end iter */
		gtk_text_buffer_get_end_iter( buffer, &iter );
		/* Add the message to the text buffer */
		gtk_text_buffer_insert( buffer, &iter, text, -1 );
		/* Scroll to end iter */
		gtk_text_view_scroll_to_iter( GTK_TEXT_VIEW (MonitorFramesView),
									&iter, 0.0, FALSE, 0, 0 );
#endif
		/* How do I make a text view scroll to the end of the buffer automatically ?
		A good way to keep a text buffer scrolled to the end is to place a mark at the end of the buffer, and give it right gravity.
		The gravity has the effect that text inserted at the mark gets inserted before, keeping the mark at the end.
		To ensure that the end of the buffer remains visible, use gtk_text_view_scroll_to_mark() to scroll to the mark after
		inserting new text. */
		
		/* Returns the GtkTextBuffer being displayed by this text view. */
		buffer = gtk_text_view_get_buffer( GTK_TEXT_VIEW(MonitorFramesView) );
		/* Returns the mark that represents the cursor (insertion point). */
		mark = gtk_text_buffer_get_insert( buffer );
		/* Initializes iter with the current position of mark. */
		gtk_text_buffer_get_iter_at_mark( buffer, &iter, mark );
		/* Inserts buffer at position iter. */
		gtk_text_buffer_insert( buffer, &iter, text, -1 );
		/* Scrolls text_view the minimum distance such that mark is contained within the visible area of the widget. */
		if ( gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON( MonitorAutoScroll ) ) )
			gtk_text_view_scroll_mark_onscreen( GTK_TEXT_VIEW(MonitorFramesView), mark ); 
	}

gdk_threads_leave();
#endif
}


void DisplayFrameLogFromFileTextCompressed( int NumFramesLogWindow, char * FramesLogFile )
{
	gzFile pLogFile;
	GtkTextBuffer *TextBufferForGtk;
//	GtkTextIter iter;
	GtkTextMark *mark;
printf( ">>> %s(), for window %d\n", __FUNCTION__, NumFramesLogWindow );
	StrFramesLogWindow * pFramesLogWindow = &FramesLogWindow[ NumFramesLogWindow ];
    /* get the text buffer */
	TextBufferForGtk = gtk_text_view_get_buffer( GTK_TEXT_VIEW( pFramesLogWindow->MonitorFramesView ) );

	pLogFile = gzopen( FramesLogFile, "rt" );
	if ( pLogFile==NULL )
	{
		gtk_text_buffer_set_text( TextBufferForGtk, _("Failed to load frames log file."), -1 );
	}
	else
	{
		gtk_text_buffer_set_text( TextBufferForGtk, "", -1 );
		char Buff[ 400 ];
		// too much troubles with adding lines to gtk_text_buffer, so loading all in a big buffer, and then display in one time...
		while( gzgets( pLogFile, Buff, 400 ) )
		{
// should works, but crash sometimes...?
// finally found.. because called from a thread... of course, like with MonitorWindowAddText() before in fact !!!
//			/* Initializes iter with end. */
//			gtk_text_buffer_get_end_iter( TextBufferForGtk, &iter );
//			/* Inserts buffer at position iter. */
//			gtk_text_buffer_insert( TextBufferForGtk, &iter, Buff, -1 );
			gtk_text_buffer_insert_at_cursor( TextBufferForGtk, Buff, -1 );
		}
		gzclose( pLogFile ); 
printf( ">>> now scroll text to see end.\n" );
//		/* Initializes iter with end. */
//		gtk_text_buffer_get_end_iter( TextBufferForGtk, &iter );
//		/* Scroll to end iter */
//		gtk_text_view_scroll_to_iter( GTK_TEXT_VIEW(pFramesLogWindow->MonitorFramesView),
//									&iter, 0.0, FALSE, 0, 0 );
		/* Returns the mark that represents the cursor (insertion point). */
		mark = gtk_text_buffer_get_insert( TextBufferForGtk );
		gtk_text_view_scroll_mark_onscreen( GTK_TEXT_VIEW(pFramesLogWindow->MonitorFramesView), mark ); 
	}
}

// do not call this function directly, to be used with g_idle_add() (from another thread than Gtk Main) ! */
//text: first char = NumFramesLogWindow, then FramesLogFile to display
gboolean DisplayFrameLogFromFileTextCompressedFromMainGtk( gpointer text )
{
	char * TextToFreeAfterUse = (char *)text;
	int NumFramesLogWindow = (int)TextToFreeAfterUse[ 0 ];
	char * FramesLogFile = &TextToFreeAfterUse[ 1 ];
	DisplayFrameLogFromFileTextCompressed( NumFramesLogWindow, FramesLogFile );
	free( TextToFreeAfterUse );
	return FALSE; // do it only one time.
}

