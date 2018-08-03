/* Classic Ladder Project */
/* Copyright (C) 2001-2017 Marc Le Douarain */
/* http://www.sourceforge.net/projects/classicladder */
/* http://sites.google.com/site/classicladder */
/* May 2012 */
/* ------------------------------------------------------ */
/* Monitor protocol - Transfer files send & receive       */
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
#ifndef __WIN32__
#include <syslog.h>
//#include <unistd.h>
//#include <errno.h>
#endif
#ifdef GTK_INTERFACE
#include <gtk/gtk.h> // for the many g_idle_add() !
#include <libintl.h> // i18n
#include <locale.h> // i18n
#endif
#include "classicladder.h"
#include "global.h"
#include "monitor_transfer.h"
#include "monitor_protocol_adds_serial.h" // just for MONITOR_SERIAL_SLAVE & MONITOR_SERIAL_MASTER defines !
#include "monitor_windows_gtk.h" // for MonitorWindowAddText() function
#include "base64.h"
#ifdef GTK_INTERFACE
#include "classicladder_gtk.h"
#include "log_events_gtk.h"
#endif
#include "files_project.h"
#include "preferences.h"
#include "log_events.h"
#include "tasks.h"
#include "calc.h"


StrFileTransfer FileTransfer[ 2 ]; // first for slave, second for master

char CurrentDatasFile[ 600 ];
char CurrentDatasFileBase64[ 600*2 ];

int NbrErrorsMaster = 0;
char FlagMasterQuestionAckForSlaveReceive = FALSE;
int TimeSecurityAbortTransToSlave = -1; //sleeping...

char TmpLogEventsFileFromTarget[ 400 ] = "";

void InitMonitorTransferFile( void )
{
	int Scan;
	for( Scan=0; Scan<2; Scan++ )
	{
		StrFileTransfer * pFileTrans = &FileTransfer[ Scan ];
		pFileTrans->FileName[ 0 ] = '\0';
		pFileTrans->IsSendTransfer = FALSE;
		pFileTrans->pFileTransferDesc = NULL;
		pFileTrans->NumBlock = -1;
		pFileTrans->NbrTotalBlocks = 0;
		pFileTrans->SizeBlock = 10;
		pFileTrans->NumFile = -1;
		pFileTrans->StartTransferDone = FALSE;
		pFileTrans->AskToAbort = FALSE;
	}
	CurrentDatasFile[ 0 ] = '\0';
	CurrentDatasFileBase64[ 0 ] = '\0';
//	LaunchScriptParamList[ 0 ] = NULL; // no parameter
}

#ifdef GTK_INTERFACE
void TransferMasterStart( void )
{
	if ( InfosGUI->TargetMonitor.TransferFileIsSend )
		MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "***START FILE SEND TRANSFER!***\n" );
	else
		MonitorWindowAddText( FRAMES_LOG_MONITOR_MASTER, "***START FILE RECEIVE TRANSFER!***\n" );
	NbrErrorsMaster = 0;
	StrFileTransfer * pFileTransForMaster = &FileTransfer[ MONITOR_SERIAL_MASTER ];
	pFileTransForMaster->IsSendTransfer = InfosGUI->TargetMonitor.TransferFileIsSend; // transfer send/receive a file
	pFileTransForMaster->StartTransferDone = FALSE;
	pFileTransForMaster->AskToAbort = FALSE;
	// set block size (beware it is not size of the frame!!! bigger with base64 encoding)
	pFileTransForMaster->SizeBlock = (InfosGUI->TargetMonitor.RemoteWithSerialPort[ 0 ]!='\0')?100:250;
	pFileTransForMaster->NumFile = InfosGUI->TargetMonitor.TransferFileNum;
	pFileTransForMaster->SubNumFile = InfosGUI->TargetMonitor.TransferFileSubNum;
	if ( InfosGUI->TargetMonitor.TransferFileIsSend )
	{
		if ( InfosGUI->TargetMonitor.TransferFileNum==0 )
		{
//			strcpy( pFileTransForMaster->FileName, "azerty.txt" );
			// save current project compressed in the temp directory, before sending it...
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_master_current_project.clprjz",TmpDirectoryRoot );
			SaveProjectFiles( pFileTransForMaster->FileName );
		}
		else if ( InfosGUI->TargetMonitor.TransferFileNum==1 )
		{
			// get file selected before...
			strcpy( pFileTransForMaster->FileName, InfosGUI->TargetMonitor.TransferFileSelectedName );
		}
		if ( StartToReadFileToSend( pFileTransForMaster ) )
		{
			InfosGUI->TargetMonitor.RemoteFileTransfer = TRUE;
			FileTransferUpdateInfosGtk( );
		}
	}
	else
	{
		if ( InfosGUI->TargetMonitor.TransferFileNum==0 )
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_transfer_to_master.clprjz",TmpDirectoryRoot );
		else if ( InfosGUI->TargetMonitor.TransferFileNum==10 )
		{
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_transfer_log_events.csv.gz",TmpDirectoryRoot );
			// we save the filename that we can use again later to get a copy of the csv...
			strcpy( TmpLogEventsFileFromTarget, pFileTransForMaster->FileName );
		}
		else if ( InfosGUI->TargetMonitor.TransferFileNum>=20 && InfosGUI->TargetMonitor.TransferFileNum<20+NBR_FRAMES_LOG_BUFFERS )
		{
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_transfer_frames_log.txt.gz",TmpDirectoryRoot );
		}
		else if ( InfosGUI->TargetMonitor.TransferFileNum==50 )
		{
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_transfer_register_content.csv.gz",TmpDirectoryRoot );
		}
		else if ( InfosGUI->TargetMonitor.TransferFileNum==1000 )
		{
			sprintf( pFileTransForMaster->FileName, "%s/classicladder_transfer_linux_syslog_debug.txt",TmpDirectoryRoot );
		}
		pFileTransForMaster->NumBlock = -1;
		if( StartToWriteFileToReceive( pFileTransForMaster ) )
		{
			InfosGUI->TargetMonitor.RemoteFileTransfer = TRUE;
			FileTransferUpdateInfosGtk( );
		}
	}
}
// to be called by a g_idle_add() (from another thread than Gtk Main) !
// text will be freed
gboolean DisplayFileContentURI_FromMainGtk( gpointer text_URI )
{
	GError *error = NULL;
	gtk_show_uri( NULL, text_URI, GDK_CURRENT_TIME, &error);
	free( text_URI );
	return FALSE; // do it only one time.
}
void TransferFileCompletedForMaster( StrFileTransfer * pFileTrans )
{
	MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer receive completed.\n");
	// Something to do with file completely received from the slave...
	if ( pFileTrans->NumFile==0 )
	{
		// stop local motor engine if actually running...
//v0.9.20 moved in LoadProjectFiles()		StopRunIfRunning( );
//v0.9.20 moved in LoadProjectFiles()		InfosGene->LadderState = STATE_LOADING;
		InfosGene->CurrentProjectFileName[0] = '\0'; // no current project name
		MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Loading project transfered from target...\n");
		char ProjectLoadedOk = LoadProjectFiles( pFileTrans->FileName );
		g_idle_add( (GSourceFunc)UpdateAllGtkWindows, NULL );
		g_idle_add( (GSourceFunc)UpdateWindowTitleWithProjectName, NULL );
		g_idle_add( (GSourceFunc)MessageInStatusBar, ProjectLoadedOk?( _("Loaded project transfered from target.") ):( _("Failed to load project transfered from target...") ) );
	}
	else if ( pFileTrans->NumFile==10 )
	{
//eh here in a thread!!!		DisplayLogBookEventsFromCsvFile( pFileTrans->FileName );
		g_idle_add( (GSourceFunc)DisplayLogBookEventsFromCsvFile, pFileTrans->FileName );
	}
	else if ( pFileTrans->NumFile>=20 && pFileTrans->NumFile<20+NBR_FRAMES_LOG_BUFFERS )
	{
		MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Ask to display frame log received...\n");
		char * AllocText = malloc( 1+strlen(pFileTrans->FileName)+1 +9);
		if ( AllocText )
		{
			// first character = frames log window number !!!
			AllocText[0] = (char)(pFileTrans->NumFile-20);
			strcpy( &AllocText[1], pFileTrans->FileName );
			g_idle_add( DisplayFrameLogFromFileTextCompressedFromMainGtk, AllocText );
		}
	}
	else if ( pFileTrans->NumFile==50 )
	{
		char * RegisterContentFileName = (char *)malloc( 400 );
		if ( RegisterContentFileName )
		{
			sprintf( RegisterContentFileName, "%s_uncompressed", pFileTrans->FileName );
			ConvertCompressedCsvToUncompressedCsv( pFileTrans->FileName, RegisterContentFileName );
			sprintf( RegisterContentFileName, "file://%s_uncompressed", pFileTrans->FileName );
printf("REGISTER FILE TO SHOW = %s\n", RegisterContentFileName);
			g_idle_add( DisplayFileContentURI_FromMainGtk, RegisterContentFileName );
		}
	}
	else if ( pFileTrans->NumFile==1000 )
	{
		char * FileNameReceivedToShow = (char *)malloc( 400 );
		if ( FileNameReceivedToShow )
		{
			sprintf( FileNameReceivedToShow, "file://%s", pFileTrans->FileName );
printf("LINUX SYSLOG DEBUG FILE TO SHOW = %s\n", FileNameReceivedToShow);
			g_idle_add( DisplayFileContentURI_FromMainGtk, FileNameReceivedToShow );
		}
	}
}
void TransferMasterAskToAbort( void )
{
	FileTransfer[ MONITOR_SERIAL_MASTER ].AskToAbort = TRUE;
}
StrFileTransfer * GetPtrFileTransForMaster( void )
{
	return &FileTransfer[ MONITOR_SERIAL_MASTER ];
}
#endif

// file names on target/slave
void ConvertFileNumForSlave( int FileNum, int FileSubNum )
{
	FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName[ 0 ] = '\0';
	if ( FileTransfer[ MONITOR_SERIAL_SLAVE ].IsSendTransfer )
	{
		// file name to receive on target...
		if ( FileNum==0 )
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, FILEPROJECT_TRANSFERED_TO_SAVE );
		else if ( FileNum==1 )
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "%s/classicladder_embedded_soft.tar.gz",TmpDirectoryRoot );
	}
	else
	{
		// file name to send to the master...
		if ( FileNum==0 )
		{
//			strcpy( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "azerty.txt" );
			// save current project compressed in the temp directory, before sending it...
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "%s/classicladder_slave_current_project.clprjz",TmpDirectoryRoot );
			SaveProjectFiles( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName );
		}
		else if ( FileNum==10 )
		{
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "%s/classicladder_slave_log_events.csv.gz",TmpDirectoryRoot );
			SaveLogEventsTextFile( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, TRUE/*Compressed*/ );
		}
		else if ( FileNum>=20 && FileNum<20+NBR_FRAMES_LOG_BUFFERS )
		{
printf("SEEN ASK FILE FRAMES LOG %d!!!\n",FileNum-20);
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "%s/classicladder_slave_frames_log.txt.gz",TmpDirectoryRoot );
			SaveFramesLogTextCompressed( FileNum-20, FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName );
		}
		else if ( FileNum==50 )
		{
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "%s/classicladder_slave_register_content.csv.gz",TmpDirectoryRoot );
			SaveRegisterFunctionBlockContent( FileSubNum, FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, TRUE/*Compressed*/ );
		}
		else if ( FileNum==1000 )
		{
			sprintf( FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName, "/var/log/debug" );
		}
	}
}
// parameter SlaveMode just used for frame log...
void TransferFileCompletedForSlave( StrFileTransfer * pFileTrans, char SlaveMode )
{
	printf("Transfer file completed on slave...\n");
	FrameLogString( NUM_FRAME_LOG_FOR_MON(SlaveMode), 'i', "Transfer file completed on slave." );
	// We have now something to do with file completely received from the master !
	if ( pFileTrans->NumFile==0 )
	{
#ifdef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		printf("'Simulation only' of file transfer on slave side... ;-)\n");
#else
		// stop local motor engine if actually running...
//v0.9.20 moved in LoadProjectFiles()		StopRunIfRunning( );
		strcpy( InfosGene->CurrentProjectFileName, FileTransfer[ MONITOR_SERIAL_SLAVE ].FileName );
#ifndef __WIN32__
		if ( Preferences.UseSysLog )
			syslog( LOG_USER|LOG_DEBUG, "Loading new project transfered to slave...\n" );
#endif
		char ProjectLoadedOk = LoadProjectFiles( InfosGene->CurrentProjectFileName );
		if ( ProjectLoadedOk )
		{
			strcpy( Preferences.DefaultProjectFileNameToLoadAtStartup, InfosGene->CurrentProjectFileName );
#ifndef __WIN32__
			if ( Preferences.UseSysLog )
				syslog( LOG_USER|LOG_DEBUG, "Set this project as default one to load at startup.\n" );
#endif
			Preferences.LatestStateSaved = InfosGene->LadderState;
			RemountFileSystemRW( );
			SavePreferences( );
			RemountFileSystemRO( );
			HardwareActionsAfterProjectLoaded( );
		}
#endif
	}
	if ( pFileTrans->NumFile==1 )
	{
#ifdef MONITOR_TEST_ONLY_NO_RESPONSES_USE
		printf("'Simulation only' of file transfer on slave side... ;-)\n");
#else
#ifndef SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED
		printf("Soft update received on slave side, but doing with it nothing automatically.... for now! ->telnet is your friend...\n");
#else
//		LaunchScript( SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED, NAME_SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED );
		LaunchExternalCommand( SCRIPT_TO_RUN_AFTER_UPDATE_RECEIVED "\t" );
#endif
#endif
	}
}

//called in logic task...
void SecurityForAbortedTransferToSlave( void )
{
	if ( TimeSecurityAbortTransToSlave!=-1 )
	{
		TimeSecurityAbortTransToSlave = TimeSecurityAbortTransToSlave+GeneralParamsMirror./*InfosGene->GeneralParams.*/PeriodMilliSecsTaskLogic;
		if ( TimeSecurityAbortTransToSlave>5000 )
		{
			printf("Security transfer (during file receive) aborted !!??\n");
			TimeSecurityAbortTransToSlave = -1;
			CloseFileTransfer( &FileTransfer[ MONITOR_SERIAL_SLAVE ] );
			RemountFileSystemRO( );
		}
	}
}

void CloseFileTransfer( StrFileTransfer * pFileTrans )
{
	if ( pFileTrans->pFileTransferDesc )
	{
		fclose( pFileTrans->pFileTransferDesc );
		pFileTrans->pFileTransferDesc = NULL;
	}
}

void DoNextRead( StrFileTransfer * pFileTrans )
{
	int NbrBytesRead = 0;
	NbrBytesRead = fread( CurrentDatasFile, 1, pFileTrans->SizeBlock, pFileTrans->pFileTransferDesc );
	CurrentDatasFile[ NbrBytesRead ] = '\0';
//printf("%s() string=%s, len=%d\n", __FUNCTION__, CurrentDatasFile, strlen( CurrentDatasFile ) );
printf("%s() len=%d\n", __FUNCTION__, strlen( CurrentDatasFile ) );
	// inc block number for theses new datas read
	pFileTrans->NumBlock++;
	if ( NbrBytesRead==0 )
	{
		CurrentDatasFileBase64[ 0 ] = '\0';
		CloseFileTransfer( pFileTrans );
	}
	else
	{
		encode64( CurrentDatasFile, CurrentDatasFileBase64, NbrBytesRead );
	}
}

char StartToReadFileToSend( StrFileTransfer * pFileTrans )
{
	if ( pFileTrans->FileName[ 0 ]!='\0' )
	{
		pFileTrans->pFileTransferDesc = fopen( pFileTrans->FileName, "rb" );
		if ( pFileTrans->pFileTransferDesc )
		{
printf("%s(), file %s opened\n",__FUNCTION__,pFileTrans->FileName);
			// get size of the file
			long FileSize = 0;
			if ( fseek( pFileTrans->pFileTransferDesc, 0L, SEEK_END )==0 )
			{
				FileSize = ftell( pFileTrans->pFileTransferDesc );
printf("size of file read to send=%ld\n",FileSize);
			}
			rewind( pFileTrans->pFileTransferDesc );
			pFileTrans->NbrTotalBlocks = (int)( FileSize/pFileTrans->SizeBlock );
			// directly read start of datas !
			pFileTrans->NumBlock = -1;
			DoNextRead( pFileTrans);
			return TRUE;
		}
		else
		{
			printf("%s(), failed to open to read file %s!!!\n",__FUNCTION__,pFileTrans->FileName);
		}
	}
	else
	{
		printf("%s(), ERROR NO FILE NAME !!!\n",__FUNCTION__);
	}
	return FALSE;
}

char StartToWriteFileToReceive( StrFileTransfer * pFileTrans )
{
	if ( pFileTrans->FileName[ 0 ]!='\0' )
	{
		// just start a clean empty file.
		pFileTrans->pFileTransferDesc = fopen( pFileTrans->FileName, "wb" );
		if ( pFileTrans->pFileTransferDesc )
		{
printf("%s(), new file %s ready\n",__FUNCTION__,pFileTrans->FileName);
			pFileTrans->NumBlock = 0; // ok ! now ready for datas transfer from the master...
			fclose( pFileTrans->pFileTransferDesc );
			pFileTrans->pFileTransferDesc = NULL;
			return TRUE;
		}
		else
		{
			printf("%s(), failed to open to write file %s!!!\n",__FUNCTION__,pFileTrans->FileName);
		}
	}
	else
	{
		printf("%s(), ERROR NO FILE NAME !!!\n",__FUNCTION__);
	}
	return FALSE;
}

char AddDatasToFileToReceive( StrFileTransfer * pFileTrans, char * DatasToAdd )
{
	// not the end of file transfer ?
	if( DatasToAdd[0]!='\0' )
	{
printf("%s() Datas to add to the file = %s (len=%d)\n", __FUNCTION__, DatasToAdd,strlen( DatasToAdd ) );
		int NbrBytesDecoded = decode64( DatasToAdd );
		// open file to add datas to it then close (usefull if master abort during transfer...)
		pFileTrans->pFileTransferDesc = fopen( pFileTrans->FileName, "ab+" );
		if ( pFileTrans->pFileTransferDesc )
		{
			fwrite( DatasToAdd, 1, NbrBytesDecoded/*strlen( DatasToAdd )*/, pFileTrans->pFileTransferDesc );
			fclose( pFileTrans->pFileTransferDesc );
			pFileTrans->pFileTransferDesc = NULL;
			pFileTrans->NumBlock++;
printf("%s() Datas added to file\n", __FUNCTION__);
			return TRUE;
		}
		else
		{
			printf("%s() Failed to open file %s for append\n", __FUNCTION__, pFileTrans->FileName );
			pFileTrans->NumBlock = -1; //error !
		}
	}
	else
	{
		pFileTrans->NumBlock = -1; //end of transfer succesfull here !
		printf("%s() file transfer completely received (no more datas to add) !\n", __FUNCTION__);
		return TRUE;
	}
	return FALSE;
}


/* MASTER SIDE ONLY (REQUEST ASK QUESTION SEND TO SLAVE) */
/* ===================================================== */
char * MonitorCreateRequestFileTransfer( void )
{
	//create request...
	cJSON *JsonRoot;
	char * TextReq = NULL;
	StrFileTransfer * pFileTransMaster = &FileTransfer[ MONITOR_SERIAL_MASTER ];
	
	if( pFileTransMaster->AskToAbort )
	{
		InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
#ifdef GTK_INTERFACE
		g_idle_add( (GSourceFunc)FileTransferUpdateInfosGtk, NULL );
#endif
	}
	else
	{
		JsonRoot = cJSON_CreateObject();
	
		cJSON_AddStringToObject( JsonRoot, "ReqCL", "Transfer" );
	
		if ( !pFileTransMaster->StartTransferDone )
		{
			cJSON_AddStringToObject( JsonRoot, "Action", pFileTransMaster->IsSendTransfer?"SendFileToTarget":"ReceiveFileFromTarget" );
			cJSON_AddNumberToObject( JsonRoot, "NumFile", pFileTransMaster->NumFile );
			cJSON_AddNumberToObject( JsonRoot, "SubNumFile", pFileTransMaster->SubNumFile );
			if ( pFileTransMaster->IsSendTransfer )
				cJSON_AddNumberToObject( JsonRoot, "NbrTotalBlocks", pFileTransMaster->NbrTotalBlocks );
			else
				cJSON_AddNumberToObject( JsonRoot, "SizeBlock", pFileTransMaster->SizeBlock );
		}
		else if ( !pFileTransMaster->IsSendTransfer )
		{
			cJSON_AddStringToObject( JsonRoot, "Action", FlagMasterQuestionAckForSlaveReceive?"Ack":"Err" );
			if ( FlagMasterQuestionAckForSlaveReceive )
				cJSON_AddNumberToObject( JsonRoot, "NumBlock", pFileTransMaster->NumBlock-1 );
			FlagMasterQuestionAckForSlaveReceive = FALSE;
		}
		if ( pFileTransMaster->IsSendTransfer )
		{
			cJSON_AddNumberToObject( JsonRoot, "NumBlock", pFileTransMaster->NumBlock );
			cJSON_AddStringToObject( JsonRoot, "Datas", CurrentDatasFileBase64 /*CurrentDatasFile*/ );
		}

		// Print to text, Delete the cJSON, use it, release the string.
		TextReq = cJSON_Print( JsonRoot );
		cJSON_Delete( JsonRoot );
//to reuse it after now...	free( TextReq );
	}
	return TextReq;
}

void MonitorParseResponseFileTransfer( cJSON *JsonRoot, char SlaveMode )
{
	cJSON * JsonCommandAction = cJSON_GetObjectItem( JsonRoot, "Action" );
	cJSON * JsonNumBlock = cJSON_GetObjectItem( JsonRoot, "NumBlock" );
	char * ActionFrame = "";
	int NumBlockFrame = -1;
	if ( JsonCommandAction )
		ActionFrame = JsonCommandAction->valuestring;
	if ( JsonNumBlock )
		NumBlockFrame = JsonNumBlock->valueint;
		
	if ( !SlaveMode )
	{
#ifdef GTK_INTERFACE
		/* MASTER SIDE (RESPONSE ANALYSE) */
		/* ============================== */
		char LastResponseOk = FALSE;
		StrFileTransfer * pFileTransMaster = &FileTransfer[ MONITOR_SERIAL_MASTER ];
		// get values from the response of the slave...
		// --------------------------------------------
		if ( pFileTransMaster->IsSendTransfer )
		{
			/* === VERIFY ACK FROM THE SLAVE TO GO ON TO SEND NEXT DATAS === */
			if ( ActionFrame[0]!='\0' )
			{
				if ( strcmp( ActionFrame,"Ack" )==0 )
				{
					// verify block number acknowledge
					if ( NumBlockFrame == pFileTransMaster->NumBlock )
					{
						if ( !pFileTransMaster->StartTransferDone )
							pFileTransMaster->StartTransferDone = TRUE;
						MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer send test response ok.\n");
						LastResponseOk = TRUE;
						NbrErrorsMaster = 0;
						if ( pFileTransMaster->pFileTransferDesc )
						{
							DoNextRead( pFileTransMaster );
						}
						else
						{
							MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer send completed.\n");
							g_idle_add( (GSourceFunc)MessageInStatusBar, _("Transfer send completed!") );
							InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
						}
					}
					else
					{
						MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer error, not correct block number ack!\n");
					}
				}
			}
		}
		else
		{
			/* === FILE DATAS RECEIVED FROM THE SLAVE === */
			cJSON * JsonNbrBlocks = cJSON_GetObjectItem( JsonRoot, "NbrTotalBlocks" );
			if ( JsonNbrBlocks )
				pFileTransMaster->NbrTotalBlocks = JsonNbrBlocks->valueint;
			// verify block we want to receive...
			if ( NumBlockFrame == pFileTransMaster->NumBlock )
			{
				char * DatasRecvString = cJSON_GetObjectItem( JsonRoot, "Datas" )->valuestring;
				if ( !pFileTransMaster->StartTransferDone )
					pFileTransMaster->StartTransferDone = TRUE;
				MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer receive test response ok.\n");
				FlagMasterQuestionAckForSlaveReceive = TRUE;
				LastResponseOk = AddDatasToFileToReceive( pFileTransMaster, DatasRecvString );
				if ( LastResponseOk )
				{
					NbrErrorsMaster = 0;
					if ( pFileTransMaster->NumBlock==-1 )
					{
						InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
						TransferFileCompletedForMaster( pFileTransMaster );
					}
				}
			}
			else
			{
				MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer error, not datas block number we were waiting for!\n");
			}
		}
		if ( !LastResponseOk )
		{
			MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Transfer error in response test!!!!\n");
			NbrErrorsMaster++;
			if ( NbrErrorsMaster>3 )
			{
//				strcpy( InfosGene->ErrorMsgStringToDisplay, "Too much transfer errors in response with remote target..." );
				ShowMessageBoxInBackground( NULL, _("Too much transfer errors in response with remote target...") );
				MonitorWindowAddText(FRAMES_LOG_MONITOR_MASTER, "Too much transfer errors in response, aborted!!!!\n");
				InfosGUI->TargetMonitor.RemoteFileTransfer = FALSE;
			}
		}
		g_idle_add( (GSourceFunc)FileTransferUpdateInfosGtk, NULL );
#else
	printf("Transfer: error analyse for master not compiled on an embedded target!!!!!!!\n");
#endif
	}
	else
	{
		/* SLAVE SIDE (RESPONSE ANALYSE) */
		/* ============================= */
		char LastCmdOk = FALSE;
		char LastActionReceivedIsAnAck = FALSE;
		StrFileTransfer * pFileTransSlave = &FileTransfer[ MONITOR_SERIAL_SLAVE ];
		// get value send by the master
		if ( ActionFrame[0]!='\0' )
		{
			if ( strcmp( ActionFrame,"SendFileToTarget" )==0 )
			{
				cJSON * TagSubNumFile;
				// abnormal previous file not close ? (master abort ?)
				if ( pFileTransSlave->pFileTransferDesc )
				{
					printf("Transfer: strange previous file not closed!?\n");
					CloseFileTransfer( pFileTransSlave );
				}
				pFileTransSlave->IsSendTransfer = TRUE;
				pFileTransSlave->StartTransferDone = FALSE;
				pFileTransSlave->NumFile = cJSON_GetObjectItem( JsonRoot, "NumFile" )->valueint;
				TagSubNumFile = cJSON_GetObjectItem( JsonRoot,"SubNumFile" );
				if ( TagSubNumFile )
					pFileTransSlave->SubNumFile = TagSubNumFile->valueint;				
				ConvertFileNumForSlave( pFileTransSlave->NumFile, pFileTransSlave->SubNumFile );
				pFileTransSlave->NumBlock = -1;
				if ( RemountFileSystemRW( ) )
				{
					LastCmdOk = StartToWriteFileToReceive( pFileTransSlave );
				}
				TimeSecurityAbortTransToSlave = 0; // start to survey !
				if ( cJSON_GetObjectItem( JsonRoot, "NbrTotalBlocks" ) )
					cJSON_DeleteItemFromObject( JsonRoot, "NbrTotalBlocks" );
			}
			if ( strcmp( ActionFrame,"ReceiveFileFromTarget" )==0 )
			{
				cJSON * JsonSizeBlock;
				cJSON * TagSubNumFile;
				// abnormal previous file not close ? (master abort ?)
				if ( pFileTransSlave->pFileTransferDesc )
				{
					printf("Transfer: strange previous file not closed!?\n");
					CloseFileTransfer( pFileTransSlave );
				}
				pFileTransSlave->IsSendTransfer = FALSE;
				pFileTransSlave->StartTransferDone = FALSE;
				pFileTransSlave->NumFile = cJSON_GetObjectItem( JsonRoot, "NumFile" )->valueint;
				TagSubNumFile = cJSON_GetObjectItem( JsonRoot,"SubNumFile" );
				if ( TagSubNumFile )
					pFileTransSlave->SubNumFile = TagSubNumFile->valueint;				
				ConvertFileNumForSlave( pFileTransSlave->NumFile, pFileTransSlave->SubNumFile );
				JsonSizeBlock = cJSON_GetObjectItem( JsonRoot, "SizeBlock" );
				if ( JsonSizeBlock )
				{
					pFileTransSlave->SizeBlock = JsonSizeBlock->valueint;
					cJSON_DeleteItemFromObject( JsonRoot, "SizeBlock" );
				}
				LastCmdOk = StartToReadFileToSend( pFileTransSlave );
			}
			if ( strcmp( ActionFrame,"Ack" )==0 )
			{
				LastActionReceivedIsAnAck = TRUE;
			}
			cJSON_DeleteItemFromObject( JsonRoot, "Action" );
		}
		if ( pFileTransSlave->IsSendTransfer )
		{
			/* === FILE DATAS RECEIVED FROM THE MASTER === */
			TimeSecurityAbortTransToSlave = 0; // re-start survey !
printf("Look at datas block received...\n");
			if ( NumBlockFrame==pFileTransSlave->NumBlock )
			{
				char * DatasRecvString = cJSON_GetObjectItem( JsonRoot, "Datas" )->valuestring;
				LastCmdOk = AddDatasToFileToReceive( pFileTransSlave, DatasRecvString );
				if ( LastCmdOk && pFileTransSlave->NumBlock==-1 )
				{
					TimeSecurityAbortTransToSlave = -1; //sleeping...
					RemountFileSystemRO( );
					TransferFileCompletedForSlave( pFileTransSlave, SlaveMode );
				}
			}
			else
			{
				printf("Transfer: not datas block number we were waiting for (%d)!\n",pFileTransSlave->NumBlock);
			}
			// destroy long "Datas" key not necessary for the master !
			cJSON_DeleteItemFromObject( JsonRoot, "Datas" );
			// adds values responses for the response to the master !
			// ------------------------------------------------------
			cJSON_AddStringToObject( JsonRoot, "Action", LastCmdOk?"Ack":"Error" );
		}
		else
		{
			/* === VERIFY ACK FROM THE MASTER TO GO ON TO SEND NEXT DATAS === */
			if ( !pFileTransSlave->StartTransferDone )
			{
				pFileTransSlave->StartTransferDone = TRUE;
				cJSON_AddNumberToObject( JsonRoot, "NbrTotalBlocks", pFileTransSlave->NbrTotalBlocks );
			}
			else
			{
				if ( LastActionReceivedIsAnAck )
				{
printf("DEBUG SLAVE TEST ACK RECEIVED...\n");
					if ( NumBlockFrame==pFileTransSlave->NumBlock )
					{
						DoNextRead( pFileTransSlave );
						if ( !pFileTransSlave->pFileTransferDesc )
							CloseFileTransfer( pFileTransSlave );
					}
					else
					{
						printf("Transfer: not correct block number ack!\n");
					}
				}
				else
				{
					printf("Transfer: not received 'ack' from the master!\n");
				}
			}
			// destroy the "NumBlock" acq of the master, we will add the new one for theses datas!
			if ( JsonNumBlock )
				cJSON_DeleteItemFromObject( JsonRoot, "NumBlock" );
			// adds values responses for the response to the master !
			// ------------------------------------------------------
			cJSON_AddNumberToObject( JsonRoot, "NumBlock", pFileTransSlave->NumBlock );
			cJSON_AddStringToObject( JsonRoot, "Datas", CurrentDatasFileBase64 /*CurrentDatasFile*/ );
		}
	}
}

