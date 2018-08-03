/* Classic Ladder Project */
/* Copyright (C) 2001-2016 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* January 2014 */
/* ------------------------------------------------------ */
/* Monitor Serial config window (Serial port name, speed) */
/* ------------------------------------------------------ */
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
#include <libintl.h> // i18n
#include <locale.h> // i18n

#include "classicladder.h"
#include "global.h"
#include "classicladder_gtk.h"
#include "monitor_serial_config_window_gtk.h"

#define NBR_MON_SER_PARAMS 2
static GtkWidget* pDialogBox;
static GtkWidget * pEntryParams[ NBR_MON_SER_PARAMS ];

/* to simulate "OK" click when return done on parameter field */
static void DialogConnectDoResponseOk( void )
{
	gtk_dialog_response( GTK_DIALOG(pDialogBox), GTK_RESPONSE_OK );
}

char OpenMonitorSerialConfigDialog( void )
{
	char * NameParams[] = { N_("Serial port (blank=not used)"), N_("Serial Speed") };
	GtkWidget * hbox[ NBR_MON_SER_PARAMS ];
	GtkWidget * pLabel;
	GtkWidget *DialogContentArea;
	int ScanLine;

	char OkDone = FALSE;
	
	if ( !InfosGUI->TargetMonitor.RemoteConnected )
	{
		ShowMessageBoxError( _("You are not currently connected to a remote target...") );
		return OkDone;
	}
	InfosGUI->TargetMonitor.AskTargetToGetMonitorSerialConfig = 1;

	pDialogBox = gtk_dialog_new_with_buttons( _("Serial monitor config"),
		GTK_WINDOW( MainSectionWindow ),
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	
	//ForGTK3
	DialogContentArea = gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox));
	for( ScanLine=0; ScanLine<NBR_MON_SER_PARAMS; ScanLine++ )
	{
		hbox[ScanLine] = gtk_hbox_new (FALSE, 0);
//ForGTK3		gtk_container_add (GTK_CONTAINER(GTK_DIALOG(pDialogBox)->vbox), hbox[ScanLine]);
		gtk_container_add (GTK_CONTAINER(DialogContentArea), hbox[ScanLine]);
		pLabel = gtk_label_new( gettext(NameParams[ScanLine]) );
		gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pLabel, FALSE, FALSE, 0);
		pEntryParams[ ScanLine ] = gtk_entry_new();
		gtk_entry_set_text( GTK_ENTRY(pEntryParams[ ScanLine ]), _("reading...") );
		gtk_box_pack_start(GTK_BOX(hbox[ScanLine]), pEntryParams[ ScanLine ], FALSE, FALSE, 0);
		gtk_signal_connect(GTK_OBJECT(pEntryParams[ ScanLine ]), "activate",
				GTK_SIGNAL_FUNC(DialogConnectDoResponseOk), 0);
	}

//ForGTK3	gtk_widget_show_all(GTK_DIALOG(pDialogBox)->vbox);
	gtk_widget_show_all(DialogContentArea);
	switch (gtk_dialog_run(GTK_DIALOG(pDialogBox)))
	{
		case GTK_RESPONSE_OK:
		{
			for( ScanLine=0; ScanLine<NBR_MON_SER_PARAMS; ScanLine++ )
			{
				const char * ParamToSave = gtk_entry_get_text(GTK_ENTRY(pEntryParams[ ScanLine ]) );
				switch( ScanLine )
				{
					case 0: strcpy( InfosGUI->TargetMonitor.TargetSlaveOnSerialPort, ParamToSave ); break;
					case 1: InfosGUI->TargetMonitor.TargetSlaveSerialSpeed = atoi( ParamToSave ); break;
				}
			}
			InfosGUI->TargetMonitor.AskTargetToWriteMonitorSerialConfig = 1;
			OkDone = TRUE;
			break;
		}
	}

	gtk_widget_destroy(pDialogBox);
	return OkDone;
}

void UpdateMonitorSerialConfigValues( void )
{
	int ScanLine;
	char Buff[ 30 ];
	char * ParamToDisplay = NULL;
	for( ScanLine=0; ScanLine<NBR_MON_SER_PARAMS; ScanLine++ )
	{
		switch( ScanLine )
		{
			case 0: ParamToDisplay = InfosGUI->TargetMonitor.TargetSlaveOnSerialPort; break;
			case 1: ParamToDisplay = Buff; sprintf(Buff, "%d", InfosGUI->TargetMonitor.TargetSlaveSerialSpeed); break;
		}
		gtk_entry_set_text( GTK_ENTRY(pEntryParams[ ScanLine ]), ParamToDisplay );
	}
}
