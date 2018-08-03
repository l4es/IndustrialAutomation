/* Classic Ladder Project */
/* Copyright (C) 2001-2016 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* July 2009 */
/* --------------------- */
/* Log book - GTK window */
/* --------------------- */
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

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>
#include <libintl.h> // i18n
#include <locale.h> // i18n
#include "classicladder.h"
#include "global.h"
#include "edit.h"
#include "classicladder_gtk.h"
#include "vars_names.h"
#include "symbols_gtk.h"
#include "menu_and_toolbar_gtk.h"
#include "log_events_gtk.h"
#include "preferences.h"
#include "log_events.h"

GtkWidget *LogBookWindow;
GtkWidget *CheckFilterActiveEvents;
static GtkListStore *ListStore;

enum
{
	ID_EVENT,
	TIME_START,
	TIME_END,
	SYMBOL_EVENT,
	PARAMETER_EVENT,
	TEXT_EVENT,
	TYPE_EVENT,
	NBR_INFOS
};

//TODO: to delete, and replace per ConvertIntTimeToAsc( ) function...
//void TimeFormat( TypeTime * TimeValue, char * pBuff )
void TimeFormat( TypeTime TimeValue, char * pBuff )
{
	struct tm * TimeFields;
//	TimeFields = localtime( (time_t *)TimeValue );
	TimeFields = localtime( (time_t *)&TimeValue );
	sprintf( pBuff, "%d/%d/%d %d:%02d:%02d", TimeFields->tm_year+1900, TimeFields->tm_mon+1, TimeFields->tm_mday,
		TimeFields->tm_hour, TimeFields->tm_min, TimeFields->tm_sec );
}

//can be called from periodic timer if content log evolved
void DisplayLogBookEvents( char OnLogContentModified )
{
	GtkTreeIter   iter;
	StrEventLog * pEvent;
	char BuffTimeStart[ 30 ];
	char BuffTimeEnd[ 30 ];
	int NbrEventsRemaining = Log.NbrEvents;
	int ScanEvent = Log.LastEvent;

	if ( OnLogContentModified )
	{
//ForGTK3		if ( !GTK_WIDGET_VISIBLE( LogBookWindow ) )
		if ( !MY_GTK_WIDGET_VISIBLE( LogBookWindow ) )
			return;
	}
	else
	{
//test SaveLogEventsTextFile( "/tmp/cl_log_test.csv.gz", TRUE/*Compressed*/ );
//test DisplayLogBookEventsFromCsvFile( "/tmp/cl_log_test.csv.gz" );
//test return;

		gtk_list_store_clear( ListStore );

		if ( InfosGUI->TargetMonitor.RemoteConnected )
		{
			// Acquire an iterator
			gtk_list_store_append( ListStore, &iter );

			// fill the element
			gtk_list_store_set( ListStore, &iter,
			TEXT_EVENT, _("Asked to read log events file of the target..."),
			-1);
			
printf("*** HERE SET REMOTE_FILE_TRANSFER ---LOGS--- TO RECEIVE!\n");
			InfosGUI->TargetMonitor.TransferFileNum = 10;
			InfosGUI->TargetMonitor.TransferFileIsSend = FALSE;
			TransferMasterStart( );

			return;
		}
	}

	gtk_list_store_clear( ListStore );
	
	if ( NbrEventsRemaining>0  )
	{
		char DisplayOnlyActiveEvents = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( CheckFilterActiveEvents ) );
		do
		{
		    pEvent = &Log.Event[ ScanEvent ];
		    // verify not a killed event (config modified, ...)
		    if ( pEvent->StartTime!=0 )
		    {
				if ( !DisplayOnlyActiveEvents || ( DisplayOnlyActiveEvents && pEvent->EndTime==0 ) )
				{
					TimeFormat( /*& */pEvent->StartTime, BuffTimeStart );
					if ( pEvent->EndTime!=0 )
						TimeFormat( /*& */pEvent->EndTime, BuffTimeEnd );
					else
						strcpy( BuffTimeEnd, _("***not finished***") );

					// Acquire an iterator
					gtk_list_store_append( ListStore, &iter );

					// fill the element
					gtk_list_store_set( ListStore, &iter,
							ID_EVENT, pEvent->iIdEvent,
						TIME_START, BuffTimeStart,
						TIME_END, BuffTimeEnd,
						SYMBOL_EVENT, ConfigEventLog[ pEvent->ConfigArrayNum ].Symbol,
						PARAMETER_EVENT, pEvent->Parameter,
						TEXT_EVENT, ConfigEventLog[ pEvent->ConfigArrayNum ].Text,
						TYPE_EVENT, ConfigEventLog[ pEvent->ConfigArrayNum ].EventLevel,
						-1);
				}
			}
			NbrEventsRemaining--;
			ScanEvent--;
			if (ScanEvent<0)
				ScanEvent = NBR_EVENTS_IN_LOG-1;
		}
		while( NbrEventsRemaining>0 );
	}
}

int SplitCsvStringInArray( char * String, char * Array[] )
{
	int Idx = 0;
printf("SplitCsv: %s\n", String );
	char * ScanChar = String;
	Array[ Idx++ ] = ScanChar;
	while( *ScanChar!='\0' )
	{
		if ( *ScanChar==',' )
		{
			*ScanChar = '\0';
			ScanChar++;
printf("SplitCsvArray%d: %s\n", Idx, ScanChar );
			Array[ Idx++ ] = ScanChar;
		}
		else
		{
			ScanChar++;
		}
	}
printf("SplitCsv returns %d strings.\n", Idx);
	return Idx;
}

gboolean DisplayLogBookEventsFromCsvFile( char * LogBookFileCsv )
{
	GtkTreeIter   iter;
	gzFile pLogFile;
	
	gtk_list_store_clear( ListStore );

	pLogFile = gzopen( LogBookFileCsv, "rt" );
	if ( pLogFile==NULL )
	{
		// Acquire an iterator
		gtk_list_store_append( ListStore, &iter );

		// fill the element
		gtk_list_store_set( ListStore, &iter,
		TEXT_EVENT, _("No file to load..."),
		-1);
		return FALSE; //usefull when called with g_idle_add (just one time)
	}
	else
	{
		char Buff[ 200 ];
		while( gzgets( pLogFile, Buff, 200 ) )
		{
			char * Array[ 10 ];
			if ( SplitCsvStringInArray( Buff, Array )>=NBR_INFOS )
			{
				// Acquire an iterator
				gtk_list_store_append( ListStore, &iter );

				// fill the element
				gtk_list_store_set( ListStore, &iter,
						ID_EVENT, atoi( Array[0] ),
					TIME_START, Array[1],
					TIME_END, Array[2],
					SYMBOL_EVENT, Array[3],
					PARAMETER_EVENT, atoi( Array[4] ),
					TEXT_EVENT, Array[5],
					TYPE_EVENT, atoi( Array[6] ),
					-1);
			}
		}
		gzclose( pLogFile ); 
	}
	return FALSE; //usefull when called with g_idle_add (just one time)
}


gint LogBookWindowDeleteEvent( GtkWidget * widget, GdkEvent * event, gpointer data )
{
// Here, we must only toggle the menu check that will call itself the function below to close the window ...
//	gtk_widget_hide( LogBookWindow );
	SetToggleMenuForLogWindow( FALSE/*OpenedWin*/ );
	// we do not want that the window be destroyed.
	return TRUE;
}

// called per toggle action menu, or at startup (if window saved open or not)...
void OpenLogBookWindow( GtkAction * ActionOpen, gboolean OpenIt )
{
	if ( ActionOpen!=NULL )
		OpenIt = gtk_toggle_action_get_active( GTK_TOGGLE_ACTION(ActionOpen) );
	if ( OpenIt )
	{
		DisplayLogBookEvents(FALSE/*OnLogContentModified*/);
		RestoreWindowPosiPrefs( "LogBook", LogBookWindow );
		gtk_widget_show( LogBookWindow );
		gtk_window_present( GTK_WINDOW(LogBookWindow) );
	}
	else
	{
		RememberWindowPosiPrefs( "LogBook", LogBookWindow, TRUE/*SaveWindowSize*/ );
		gtk_widget_hide( LogBookWindow );
	}
}
void RememberLogBookWindowPrefs( void )
{
//ForGTK3	char WindowIsOpened = GTK_WIDGET_VISIBLE( GTK_WINDOW(LogBookWindow) );
	char WindowIsOpened = MY_GTK_WIDGET_VISIBLE( LogBookWindow );
	RememberWindowOpenPrefs( "LogBook", WindowIsOpened );
	if ( WindowIsOpened )
		RememberWindowPosiPrefs( "LogBook", LogBookWindow, TRUE/*SaveWindowSize*/ );
}

void FunctionRefreshLog( void )
{
	DisplayLogBookEvents(FALSE/*OnLogContentModified*/);
}
void FunctionExportCsv( void )
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ( _("Save CSV File"),
										GTK_WINDOW(MainSectionWindow),
										GTK_FILE_CHOOSER_ACTION_SAVE,
										GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
										NULL);
	gtk_file_chooser_set_do_overwrite_confirmation( GTK_FILE_CHOOSER (dialog), TRUE );
	gtk_file_chooser_set_current_name( GTK_FILE_CHOOSER (dialog), "classicladder_log_events.csv" );
	if ( gtk_dialog_run( GTK_DIALOG (dialog) ) == GTK_RESPONSE_ACCEPT )
	{
		char *filename;
		char FileSaveOk = FALSE;
		filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );

		if ( !InfosGUI->TargetMonitor.RemoteConnected )
		{
			FileSaveOk = SaveLogEventsTextFile( filename, FALSE/*Compressed*/ );
		}
		else
		{
			if ( TmpLogEventsFileFromTarget[0]!='\0' );
				FileSaveOk = ConvertCompressedCsvToUncompressedCsv( TmpLogEventsFileFromTarget, filename );
		}
		if ( !FileSaveOk )
			ShowMessageBox( _("Save Error"), _("Failed to save the csv log file..."), _("Ok") );

		g_free( filename );
	}
	gtk_widget_destroy( dialog );
}

void FunctionCleanAllEvents( void )
{
	if ( InfosGUI->TargetMonitor.RemoteConnected )
	{
		InfosGUI->TargetMonitor.AskTargetToCleanUp = 10; /*EventsLog*/
	}
	else
	{
		InitLogDatas( );
		DisplayLogBookEvents(FALSE/*OnLogContentModified*/);
	}
}
void FunctionAskToCleanAllEvents( void )
{
	ShowConfirmationBox( _("Sure?"), _("Do you really want to delete all events in the log?"), FunctionCleanAllEvents );
}

void LogBookInitGtk()
{
	GtkWidget  *scrolled_win, *vbox, *hbox;
	GtkWidget *ListView;
	GtkWidget *ButtonRefresh,*ButtonExportCsv,*ButtonClearAll;
	GtkCellRenderer   *renderer;
	long ScanCol;
	char * ColName[] = { N_("Id"), N_("Start Time"), N_("End Time"), N_("Symbol"), N_("Value"), N_("Description"), N_("Level") };

	LogBookWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW( LogBookWindow ), _("Events Log") );
	gtk_signal_connect( GTK_OBJECT( LogBookWindow ), "delete_event",
		GTK_SIGNAL_FUNC(LogBookWindowDeleteEvent), 0 );

	vbox = gtk_vbox_new(FALSE,0);

	/* Create a list-model and the view. */
	ListStore = gtk_list_store_new( NBR_INFOS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT );
	ListView = gtk_tree_view_new_with_model ( GTK_TREE_MODEL(ListStore) );

	/* Add the columns to the view. */
	for (ScanCol=0; ScanCol<NBR_INFOS; ScanCol++)
	{
		GtkTreeViewColumn *column;
		renderer = gtk_cell_renderer_text_new();
//		g_object_set(renderer, "editable", TRUE, NULL);
//TODO? gtk_entry_set_max_length(GTK_ENTRY(  ),9);
//		g_signal_connect( G_OBJECT(renderer), "edited", G_CALLBACK(Callback_TextEdited), (gpointer)ScanCol );
		column = gtk_tree_view_column_new_with_attributes( gettext(ColName[ ScanCol ]), renderer, "text", ScanCol, NULL );
		gtk_tree_view_append_column( GTK_TREE_VIEW(ListView), column );
		gtk_tree_view_column_set_resizable( column, TRUE );
//////		gtk_tree_view_column_set_sort_column_id( column, ScanCol );
	}
//	avail since gtk v2.10...?
	gtk_tree_view_set_grid_lines( GTK_TREE_VIEW(ListView), GTK_TREE_VIEW_GRID_LINES_BOTH );

	scrolled_win = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_win),
                                    GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
	// here we add the view to the scrolled !
	gtk_container_add(GTK_CONTAINER(scrolled_win), ListView);
	gtk_box_pack_start(GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);

gtk_window_set_default_size (GTK_WINDOW (LogBookWindow), -1, 250);

	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

	ButtonRefresh = gtk_button_new_with_label( _("Refresh") );
	gtk_box_pack_start( GTK_BOX (hbox), ButtonRefresh, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonRefresh), "clicked", GTK_SIGNAL_FUNC(FunctionRefreshLog), 0 );
	ButtonExportCsv = gtk_button_new_with_label( _("Export log to csv") );
	gtk_box_pack_start( GTK_BOX (hbox), ButtonExportCsv, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonExportCsv), "clicked", GTK_SIGNAL_FUNC(FunctionExportCsv), 0 );
	ButtonClearAll = gtk_button_new_with_label( _("Clear All") );
	gtk_box_pack_start( GTK_BOX (hbox), ButtonClearAll, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (ButtonClearAll), "clicked", GTK_SIGNAL_FUNC(FunctionAskToCleanAllEvents), 0 );
	CheckFilterActiveEvents = gtk_check_button_new_with_label( _("Display only active events") );
	gtk_box_pack_start( GTK_BOX (hbox), CheckFilterActiveEvents, FALSE, FALSE, 0 );
	gtk_signal_connect( GTK_OBJECT (CheckFilterActiveEvents), "toggled", GTK_SIGNAL_FUNC(FunctionRefreshLog), 0 );

//	gtk_widget_show( scrolled_win );
//	gtk_widget_show( ListView );
	gtk_container_add( GTK_CONTAINER(LogBookWindow), vbox );
	gtk_widget_show_all( vbox );

	gtk_window_set_icon_name (GTK_WINDOW( LogBookWindow ), GTK_STOCK_DND);
}

