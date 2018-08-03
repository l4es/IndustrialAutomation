/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* May 2013 */
/* --------------------------------------------- */
/* Network config window (IP address, mask, ...) */
/* --------------------------------------------- */
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
#include "network_config_window_gtk.h"

#define NBR_NET_PARAMS 6
static GtkWidget* pDialogBox;
static GtkWidget * pEntryParams[ NBR_NET_PARAMS ];

/* to simulate "OK" click when return done on IP address / Serial port/speed */
static void DialogConnectDoResponseOk( void )
{
	gtk_dialog_response( GTK_DIALOG(pDialogBox), GTK_RESPONSE_OK );
}

char OpenNetworkConfigDialog( void )
{
	char * NameParams[] = {N_("IP Ad."), N_("Mask"), N_("Route"), N_("Server DNS 1"), N_("Server DNS 2"), N_("HostName")};
	GtkWidget * hbox[ NBR_NET_PARAMS ];
	GtkWidget * pLabel;
	int ScanLine;
	GtkWidget *DialogContentArea;

	char OkDone = FALSE;
	
	if ( !InfosGUI->TargetMonitor.RemoteConnected )
	{
		ShowMessageBoxError( _("You are not currently connected to a remote target...") );
		return OkDone;
	}
	InfosGUI->TargetMonitor.AskTargetToGetNetworkConfig = 1;

	pDialogBox = gtk_dialog_new_with_buttons( _("Network config"),
		GTK_WINDOW( MainSectionWindow ),
		GTK_DIALOG_MODAL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);
	
	//ForGTK3
	DialogContentArea = gtk_dialog_get_content_area(GTK_DIALOG(pDialogBox));

	for( ScanLine=0; ScanLine<NBR_NET_PARAMS; ScanLine++ )
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
			for( ScanLine=0; ScanLine<NBR_NET_PARAMS; ScanLine++ )
			{
				char * ParamToSave = NULL;
				switch( ScanLine )
				{
					case 0: ParamToSave = NetworkConfigDatas.IpAddr; break;
					case 1: ParamToSave = NetworkConfigDatas.NetMask; break;
					case 2: ParamToSave = NetworkConfigDatas.Route; break;
					case 3: ParamToSave = NetworkConfigDatas.ServerDNS1; break;
					case 4: ParamToSave = NetworkConfigDatas.ServerDNS2; break;
					case 5: ParamToSave = NetworkConfigDatas.HostName; break;
				}
				strcpy( ParamToSave, gtk_entry_get_text(GTK_ENTRY(pEntryParams[ ScanLine ]) ) );
			}
			InfosGUI->TargetMonitor.AskTargetToWriteNetworkConfig = 1;
			OkDone = TRUE;
			break;
		}
	}

	gtk_widget_destroy(pDialogBox);
	return OkDone;
}

void UpdateNetworkConfigValues( void )
{
	int ScanLine;
	char * ParamToDisplay = NULL;
	for( ScanLine=0; ScanLine<NBR_NET_PARAMS; ScanLine++ )
	{
		switch( ScanLine )
		{
			case 0: ParamToDisplay = NetworkConfigDatas.IpAddr; break;
			case 1: ParamToDisplay = NetworkConfigDatas.NetMask; break;
			case 2: ParamToDisplay = NetworkConfigDatas.Route; break;
			case 3: ParamToDisplay = NetworkConfigDatas.ServerDNS1; break;
			case 4: ParamToDisplay = NetworkConfigDatas.ServerDNS2; break;
			case 5: ParamToDisplay = NetworkConfigDatas.HostName; break;
		}
		gtk_entry_set_text( GTK_ENTRY(pEntryParams[ ScanLine ]), ParamToDisplay );
	}
}
