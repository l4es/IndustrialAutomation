/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* August 2011 */
/* ------------------------------------------------------ */
/* Preferences:                                           */
/* - Project to load per default at startup               */
/* - Others parameters (usefull on embedded or else)      */
/* - Windows positions / open-close states                */
/* - Network config                                       */
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
#include <stdlib.h>
#include <string.h>

#include "classicladder.h"
#include "global.h"
#include "files.h"
#include "vars_access.h"

#ifdef GTK_INTERFACE
#include <gtk/gtk.h>
#include "classicladder_gtk.h" // for macro MY_GTK_WIDGET_IS_VISIBLE()...
#endif

#include "preferences.h"

char PrefsFile[ 400 ];
StrPreferences Preferences;
StrNetworkConfigDatas NetworkConfigDatas;
char SetsVarsFile[ 400 ];

void InitPreferences( void )
{
	int ScanWin;
	PrefsFile[ 0 ] = '\0';
	Preferences.DefaultProjectFileNameToLoadAtStartup[ 0 ] = '\0';
	Preferences.LatestStateSaved = STATE_STOP;
	Preferences.MonitorSlaveOnSerialPort[ 0 ] = '\0';
	Preferences.MonitorSlaveSerialSpeed = 115200;
	Preferences.UseSysLog = FALSE;
	Preferences.Daemonize = FALSE;
	Preferences.UseRtcDevice = FALSE;
	strcpy( Preferences.ModemForMasterMonitor.StrInitSequence, "ATZ" );
	strcpy( Preferences.ModemForMasterMonitor.StrConfigSequence, "ATE0" );
	strcpy( Preferences.ModemForMasterMonitor.StrCallSequence, "ATDT" );
	Preferences.ModemForMasterMonitor.StrCodePIN[ 0 ] = '\0';
	for( ScanWin=0; ScanWin<NBR_WINDOWS_PREFS; ScanWin++ )
	{
		StrWindowPosisOpenPrefs * WinPrefs = &Preferences.WindowPosisOpenPrefs[ ScanWin ];
		WinPrefs->WindowName[ 0 ] ='\0';
	}
	Preferences.DisplaySymbolsInMainWindow = TRUE;
	Preferences.DisplaySymbolsInBoolsVarsWindows = FALSE;

	NetworkConfigDatas.IpAddr[0] = '\0';
	NetworkConfigDatas.NetMask[0] = '\0';
	NetworkConfigDatas.Route[0] = '\0';
	NetworkConfigDatas.ServerDNS1[0] = '\0';
	NetworkConfigDatas.ServerDNS2[0] = '\0';
	NetworkConfigDatas.HostName[0] = '\0';
	SetsVarsFile[ 0 ] = '\0';
};

void GetPrefsFilePathAndName( void )
{
	FILE * File;
	File = fopen( "classicladder_prefs", "rt" );
	if ( File )
	{
		fclose(File);
		strcpy( PrefsFile, "classicladder_prefs" );
	}
	else
	{
		File = fopen( "/etc/classicladder_prefs", "rt" );
		if ( File )
		{
			fclose(File);
			strcpy( PrefsFile, "/etc/classicladder_prefs" );
			strcpy( SetsVarsFile, "/etc/classicladder_set_vars_list.csv" );
			strcpy( LogEventsDataFile, "/etc/classicladder_log_events.bin" );
		}
		else
		{
			char * path = getenv( "HOME" );
			if ( path )
			{
				char EndWithSlash = FALSE;
				if ( strlen(path)>=1 )
				{
					if( path[strlen(path)-1]=='/' )
						EndWithSlash = TRUE;
				}
				if ( EndWithSlash )
				{
					sprintf( PrefsFile, "%s.classicladder_prefs", path );
					sprintf( SetsVarsFile, "%s.classicladder_set_vars_list.csv", path );
					sprintf( LogEventsDataFile, "%s.classicladder_log_events.bin", path );
				}
				else
				{
					sprintf( PrefsFile, "%s/.classicladder_prefs", path );
					sprintf( SetsVarsFile, "%s/.classicladder_set_vars_list.csv", path );
					sprintf( LogEventsDataFile, "%s/.classicladder_log_events.bin", path );
				}
			}
			else
			{
				printf("Environment variable HOME not found...\n");
				strcpy(PrefsFile, "classicladder_prefs" );
			}
		}
	}
	if ( SetsVarsFile[0]=='\0' )
		strcpy( SetsVarsFile, "classicladder_set_vars_list.csv" );
	if ( LogEventsDataFile[0]=='\0' )
		strcpy( LogEventsDataFile, "classicladder_log_events.bin" );
}

void LoadPreferences( void )
{
	GetPrefsFilePathAndName( );
	printf("Trying to load prefs file %s\n", PrefsFile );
	FILE * File;
	char Line[300];
	char * LineOk;
	int ScanWinPrefs = 0;
	int * PtrInts[ 5 ];
	char * PtrChars[ 5 ];
	File = fopen(PrefsFile,"rt");
	if (File)
	{
		printf("Prefs file %s found.\n", PrefsFile );
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "PROJECT_TO_LOAD_PER_DEFAULT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.DefaultProjectFileNameToLoadAtStartup, &Line[ strlen( pParameter) ] );
				pParameter = "LATEST_STATE_SAVED=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.LatestStateSaved = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MONITOR_SLAVE_ON_SERIAL_PORT=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.MonitorSlaveOnSerialPort, &Line[ strlen( pParameter) ] );
				pParameter = "MONITOR_SLAVE_SERIAL_SPEED=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.MonitorSlaveSerialSpeed = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "USE_SYSLOG=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.UseSysLog = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "DAEMONIZE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.Daemonize = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "USE_RTC_DEVICE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.UseRtcDevice = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "MODEM_MASTER_INIT_SEQUENCE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.ModemForMasterMonitor.StrInitSequence, &Line[ strlen( pParameter) ] );
				pParameter = "MODEM_MASTER_CONFIG_SEQUENCE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.ModemForMasterMonitor.StrConfigSequence, &Line[ strlen( pParameter) ] );
				pParameter = "MODEM_MASTER_CALL_SEQUENCE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.ModemForMasterMonitor.StrCallSequence, &Line[ strlen( pParameter) ] );
				pParameter = "MODEM_MASTER_PIN_CODE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( Preferences.ModemForMasterMonitor.StrCodePIN, &Line[ strlen( pParameter) ] );

				pParameter = "WINDOW_";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
				{
					char * Datas = strstr( Line, "=" );
					if ( Datas )
					{
						StrWindowPosisOpenPrefs * WinPrefs = &Preferences.WindowPosisOpenPrefs[ ScanWinPrefs++ ];
						*Datas = 0;
						strcpy( WinPrefs->WindowName, Line+strlen( pParameter ) );
						PtrInts[ 0 ] = &WinPrefs->PosX; PtrChars[ 0 ] = NULL;
						PtrInts[ 1 ] = &WinPrefs->PosY; PtrChars[ 1 ] = NULL;
						PtrInts[ 2 ] = &WinPrefs->SizeX; PtrChars[ 2 ] = NULL;
						PtrInts[ 3 ] = &WinPrefs->SizeY; PtrChars[ 3 ] = NULL;
						PtrInts[ 4 ] = NULL; PtrChars[ 4 ] = &WinPrefs->Opened;
						ConvRawLineOfStringsOrNumbers( Datas+1, 5, NULL, PtrInts, PtrChars );
					}
				}
				pParameter = "DISPLAY_SYMBOLS_MAIN_WINDOW=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.DisplaySymbolsInMainWindow = atoi( &Line[ strlen( pParameter) ] );
				pParameter = "DISPLAY_SYMBOLS_BOOLS_VARS_WINDOW=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					Preferences.DisplaySymbolsInBoolsVarsWindows = atoi( &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
	}
}
char SavePreferences( void )
{
	FILE * File = NULL;
	if ( PrefsFile[ 0 ]=='\0' )
		return FALSE;
	File = fopen(PrefsFile,"wt");
	if (File)
	{
		int ScanWin;
		fprintf( File,S_LINE "PROJECT_TO_LOAD_PER_DEFAULT=%s" E_LINE "\n", Preferences.DefaultProjectFileNameToLoadAtStartup );
		fprintf( File,S_LINE "LATEST_STATE_SAVED=%d" E_LINE "\n", Preferences.LatestStateSaved );
		fprintf( File,S_LINE "MONITOR_SLAVE_ON_SERIAL_PORT=%s" E_LINE "\n", Preferences.MonitorSlaveOnSerialPort );
		fprintf( File,S_LINE "MONITOR_SLAVE_SERIAL_SPEED=%d" E_LINE "\n", Preferences.MonitorSlaveSerialSpeed );
		fprintf( File,S_LINE "USE_SYSLOG=%d" E_LINE "\n", Preferences.UseSysLog?1:0 );
		fprintf( File,S_LINE "DAEMONIZE=%d" E_LINE "\n", Preferences.Daemonize?1:0 );
		fprintf( File,S_LINE "USE_RTC_DEVICE=%d" E_LINE "\n", Preferences.UseRtcDevice?1:0 );
		fprintf( File,S_LINE "MODEM_MASTER_INIT_SEQUENCE=%s" E_LINE "\n", Preferences.ModemForMasterMonitor.StrInitSequence );
		fprintf( File,S_LINE "MODEM_MASTER_CONFIG_SEQUENCE=%s" E_LINE "\n", Preferences.ModemForMasterMonitor.StrConfigSequence );
		fprintf( File,S_LINE "MODEM_MASTER_CALL_SEQUENCE=%s" E_LINE "\n", Preferences.ModemForMasterMonitor.StrCallSequence );
		fprintf( File,S_LINE "MODEM_MASTER_PIN_CODE=%s" E_LINE "\n", Preferences.ModemForMasterMonitor.StrCodePIN );
		for( ScanWin=0; ScanWin<NBR_WINDOWS_PREFS; ScanWin++ )
		{
			StrWindowPosisOpenPrefs * WinPrefs = &Preferences.WindowPosisOpenPrefs[ ScanWin ];
			if ( WinPrefs->WindowName[ 0 ]!='\0' )
			{
				fprintf( File,S_LINE "WINDOW_%s=%d,%d,%d,%d,%d" E_LINE "\n", WinPrefs->WindowName, WinPrefs->PosX, WinPrefs->PosY,
					WinPrefs->SizeX, WinPrefs->SizeY, WinPrefs->Opened );
			}
		}
		fprintf( File,S_LINE "DISPLAY_SYMBOLS_MAIN_WINDOW=%d" E_LINE "\n", Preferences.DisplaySymbolsInMainWindow?1:0 );
		fprintf( File,S_LINE "DISPLAY_SYMBOLS_BOOLS_VARS_WINDOW=%d" E_LINE "\n", Preferences.DisplaySymbolsInBoolsVarsWindows?1:0 );
		fclose(File);
		printf("Saved prefs file %s\n", PrefsFile );
	}
	return (File!=NULL);
}


#ifdef GTK_INTERFACE
StrWindowPosisOpenPrefs * GetPtrWindowPrefs( char * WindowName, char CanBeANewWin )
{
	StrWindowPosisOpenPrefs * pWinPrefs = NULL;
	StrWindowPosisOpenPrefs * pWinPrefsFree = NULL;
	StrWindowPosisOpenPrefs * CurrentWinPrefs;
	int ScanWin = 0;
	do
	{
		CurrentWinPrefs = &Preferences.WindowPosisOpenPrefs[ ScanWin ];
		if ( CurrentWinPrefs->WindowName[0]=='\0' && pWinPrefsFree==NULL )
			pWinPrefsFree = CurrentWinPrefs;
		if ( strcmp( CurrentWinPrefs->WindowName, WindowName )==0 )
			pWinPrefs = CurrentWinPrefs;
		else
			ScanWin++;
	}
	while( ScanWin<NBR_WINDOWS_PREFS && pWinPrefs==NULL );
//printf("GetPtrWindowPrefs, index=%d, %s\n", ScanWin, pWinPrefs!=NULL?"found":"not found");
	if( CanBeANewWin && pWinPrefs==NULL && pWinPrefsFree!=NULL )
	{
		pWinPrefs = pWinPrefsFree;
		strcpy( pWinPrefs->WindowName, WindowName );
		pWinPrefs->PosX = 0;
		pWinPrefs->PosY = 0;
		pWinPrefs->SizeX = -1;
		pWinPrefs->SizeY = -1;
	}
	return pWinPrefs;
}
void RestoreWindowPosiPrefs( char * WindowName, GtkWidget * TheGtkWindow )
{
//printf("RestoreWindowPosiPrefs: %s\n", WindowName);
	StrWindowPosisOpenPrefs * pWinPrefs = GetPtrWindowPrefs( WindowName, FALSE/*CanBeANewWin*/ );
	if ( pWinPrefs )
	{
		GdkScreen * pScreen = gtk_window_get_screen( GTK_WINDOW(TheGtkWindow) );
//printf("RestoreWindowPrefs: %s moved to %d,%d (size=%d/%d)\n", WindowName, pWinPrefs->PosX, pWinPrefs->PosY, pWinPrefs->SizeX, pWinPrefs->SizeY);
		if ( pWinPrefs->PosX>=gdk_screen_get_width(pScreen) )
			pWinPrefs->PosX = 0;
		if ( pWinPrefs->PosY>=gdk_screen_get_height(pScreen) )
			pWinPrefs->PosY = 0;
		gtk_window_move( GTK_WINDOW(TheGtkWindow), pWinPrefs->PosX, pWinPrefs->PosY );
		if ( pWinPrefs->SizeX!=-1 && pWinPrefs->SizeY!=-1 )
			gtk_window_resize( GTK_WINDOW(TheGtkWindow), pWinPrefs->SizeX, pWinPrefs->SizeY );
	}
}
void RememberWindowPosiPrefs( char * WindowName, GtkWidget * TheGtkWindow, char SaveWindowSize )
{
//printf("RememberWindowPosiPrefs: %s\n", WindowName);
//ForGTK3	if ( !MY_GTK_WIDGET_VISIBLE( GTK_WINDOW(TheGtkWindow) ) )
	if ( !MY_GTK_WIDGET_VISIBLE( TheGtkWindow ) )
	{
		printf("!!! RememberWindowPrefs CALLED WITH WINDOW %s NOT VISIBLE !!!\n",WindowName);
		return;
	}
	StrWindowPosisOpenPrefs * pWinPrefs = GetPtrWindowPrefs( WindowName, TRUE/*CanBeANewWin*/ );
	if( pWinPrefs==NULL )
	{
		printf("!!! Constant NBR_WINDOWS_PREFS in classicladder.h not enough...!?\n");
	}
	else
	{
		gint x,y;
		gtk_window_get_position( GTK_WINDOW(TheGtkWindow), &x, &y );
		if ( x<0 ) x=0;
		if ( y<0 ) y=0;
		if ( SaveWindowSize )
		{
			gint width,height;
			gtk_window_get_size( GTK_WINDOW (TheGtkWindow), &width, &height );
			pWinPrefs->SizeX = width;
			pWinPrefs->SizeY = height;
		}
		else
		{
			pWinPrefs->SizeX = -1;
			pWinPrefs->SizeY = -1;
		}
		pWinPrefs->PosX = x;
		pWinPrefs->PosY = y;
//printf("RememberWindowPrefs: %s seen at %d,%d (size=%d/%d)\n", WindowName, pWinPrefs->PosX, pWinPrefs->PosY,pWinPrefs->SizeX,pWinPrefs->SizeY);
	}
}
char GetWindowOpenPrefs( char * WindowName )
{
//printf("GetWindowOpenPrefs: %s\n", WindowName);
	StrWindowPosisOpenPrefs * pWinPrefs = GetPtrWindowPrefs( WindowName, FALSE/*CanBeANewWin*/ );
	if ( pWinPrefs )
	{
//printf("GetWindowOpenPrefs: %s opened per default %d\n", WindowName, pWinPrefs->Opened);
		return pWinPrefs->Opened;
	}
	return FALSE;
}
void RememberWindowOpenPrefs( char * WindowName, char WindowOpened )
{
//printf("RememberWindowOpenPrefs: %s opened=%d\n", WindowName,WindowOpened);
	StrWindowPosisOpenPrefs * pWinPrefs = GetPtrWindowPrefs( WindowName, TRUE/*CanBeANewWin*/ );
	if( pWinPrefs==NULL )
	{
		printf("!!! Constant NBR_WINDOWS_PREFS in classicladder.h not enough...!?\n");
	}
	else
	{
		pWinPrefs->Opened = WindowOpened;
	}
}
#endif

#define NETWORK_CONFIG_FILE "/etc/sysconfig/network"
char LoadNetworkConfigDatas( )
{
	FILE * File;
	char Line[300];
	char * LineOk;
	File = fopen(NETWORK_CONFIG_FILE,"rt");
	if (File)
	{
		printf("Network config datas %s found.\n", NETWORK_CONFIG_FILE );
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				char * pParameter;
				pParameter = "IPADDR=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( NetworkConfigDatas.IpAddr, &Line[ strlen( pParameter) ] );
				pParameter = "NETMASK=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( NetworkConfigDatas.NetMask, &Line[ strlen( pParameter) ] );
				pParameter = "ROUTE=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( NetworkConfigDatas.Route, &Line[ strlen( pParameter) ] );
				pParameter = "SERVER_DNS1=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( NetworkConfigDatas.ServerDNS1, &Line[ strlen( pParameter) ] );
				pParameter = "SERVER_DNS2=";
				if ( strncmp( Line, pParameter, strlen( pParameter) )==0 )
					strcpy( NetworkConfigDatas.ServerDNS2, &Line[ strlen( pParameter) ] );
			}
		}
		while(LineOk);
		fclose(File);
		return TRUE;
	}
	return FALSE;
}
char SaveNetworkConfigDatas( )
{
	FILE * File;
	File = fopen(NETWORK_CONFIG_FILE,"wt");
	if (File)
	{
		fprintf( File,S_LINE "IPADDR=%s" E_LINE "\n", NetworkConfigDatas.IpAddr );
		fprintf( File,S_LINE "NETMASK=%s" E_LINE "\n", NetworkConfigDatas.NetMask );
		fprintf( File,S_LINE "ROUTE=%s" E_LINE "\n", NetworkConfigDatas.Route );
		fprintf( File,S_LINE "SERVER_DNS1=%s" E_LINE "\n", NetworkConfigDatas.ServerDNS1 );
		fprintf( File,S_LINE "SERVER_DNS2=%s" E_LINE "\n", NetworkConfigDatas.ServerDNS2 );
		fclose(File);
		printf("Saved Network config datas file %s\n", NETWORK_CONFIG_FILE );
		return TRUE;
	}
	return FALSE;
}
#define HOSTNAME_FILE "/etc/hostname"
char LoadHostNameFile( )
{
	FILE * File = fopen(HOSTNAME_FILE,"rt");
	if (File)
	{
		if ( cl_fgets(NetworkConfigDatas.HostName,80,File) )
		{
printf("%s: hostname=%s\n", __FUNCTION__, NetworkConfigDatas.HostName );
			return TRUE;
		}
		fclose(File);
		return TRUE;
	}
	return FALSE;
}
char SaveHostNameFile( )
{
	FILE * File = fopen(HOSTNAME_FILE,"wt");
	if (File)
	{
		fprintf( File, "%s", NetworkConfigDatas.HostName );
		fclose(File);
		return TRUE;
	}
	return FALSE;
}

char LoadSetsVarsList( void )
{
	FILE * File;
	char Okay = FALSE;
	char Line[300];
	char * LineOk;
	int Params[3];
	File = fopen(SetsVarsFile,"rt");
	if (File)
	{
printf("Loading set vars file '%s' opened...\n", SetsVarsFile);
		InfosGene->DoNotSaveDuringLoadingSetVarsList = TRUE; // else SetVar below will save list !
		do
		{
			LineOk = cl_fgets(Line,300,File);
			if (LineOk)
			{
				if (Line[0]!=';' && Line[0]!='#')
				{
					ConvRawLineOfNumbers(Line,3,Params,-1/*DefaultValueToComplete*/);
					if ( Params[ 0 ]!=-1 && Params[ 1 ]!=-1 && Params[ 2 ]!=-1 )
					{
						int NbrVarsMax = GetNbrVarsForType( Params[ 0 ] );
						if ( Params[ 1 ]<NbrVarsMax )
							SetVar( Params[ 0 ], Params[ 1 ], Params[ 2 ] );
						else
							printf("In %s(), not set var(%d/%d) with offset too high (max=%d)...\n", __FUNCTION__, Params[ 0 ], Params[ 1 ], NbrVarsMax );
					}
				}
			}
		}
		while(LineOk);
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

char SaveSetsVarsList( void )
{
	FILE * File;
	char Okay = FALSE;
	int ScanVarType;
	int ScanVarNum;
	File = fopen(SetsVarsFile,"wt");
	if (File)
	{
		fprintf(File,S_LINE "#VER=1.0" E_LINE "\n");
		for( ScanVarType=0; ScanVarType<2; ScanVarType++ )
		{
			int VarType = (ScanVarType==0)?VAR_PHYS_INPUT:VAR_PHYS_OUTPUT;
			for( ScanVarNum=0; ScanVarNum<((ScanVarType==0)?NBR_PHYS_INPUTS:NBR_PHYS_OUTPUTS); ScanVarNum++ )
			{
				if ( IsVarSet( VarType, ScanVarNum ) )
					fprintf(File,S_LINE "%d,%d,%d" E_LINE "\n",VarType, ScanVarNum, ReadVar( VarType, ScanVarNum ));
			}
		}
		fclose(File);
		Okay = TRUE;
	}
	return (Okay);
}

